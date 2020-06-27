#include "MAX31865.h"
#include <esp_log.h>
#include <string.h>
#include <math.h>

#define CLOCK_SPEED_HZ (1000000) // 1 MHz

static const char *TAG = "MAX31865";
static max31865_t dev;

/*************************************
                SPI
 *************************************/

static esp_err_t writeRegister(uint8_t addr, uint8_t bytes, uint8_t *data)
{
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.flags = SPI_TRANS_USE_TXDATA;
	t.addr = (addr | 0x80);
	t.length = bytes * 8;
	for (uint8_t i = 0; i < bytes; i++)
		t.tx_data[i] = data[i];
	
	ESP_LOGD(TAG, "Write $0x%02x = 0x%02x", (uint8_t)t.addr, data[0]);

	return spi_device_transmit(dev.spi_dev, &t);
}

static esp_err_t readRegister(uint8_t addr, uint8_t bytes, uint8_t *data)
{
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.flags = SPI_TRANS_USE_TXDATA;
	t.addr = (addr &= 0x7F); // make sure top bit is not set
	t.length = bytes * 8;
	t.rx_buffer = data;
	for (uint8_t i = 0; i < bytes; i++)
		t.tx_data[i] = 0xFF;

	esp_err_t err = spi_device_transmit(dev.spi_dev, &t);
	if (bytes == 1)
		ESP_LOGD(TAG, "Read $%02x = 0x%02x", (uint8_t)t.addr, data[0]);
	else
		ESP_LOGD(TAG, "Read $%02x = 0x%02x 0x%02x", (uint8_t)t.addr, data[0], data[1]);

	return err;
}

/*************************************
                MAX31865
 *************************************/

static uint8_t readFault(void)
{
	ESP_LOGI(TAG, "Read Fault");
	uint8_t code = 0;
	readRegister(MAX31856_FAULTSTAT_REG, 1, &code);
	return code;
}

static void clearFault(void)
{
	ESP_LOGI(TAG, "Clear Fault");
	uint8_t code = 0;
	readRegister(MAX31856_CONFIG_REG, 1, &code);
	code &= ~0x2C;
	code |= MAX31856_CONFIG_FAULTSTAT;
	writeRegister(MAX31856_CONFIG_REG, 1, &code);
}

static void autoConvert(bool status)
{
	ESP_LOGI(TAG, "%s auto-conversion", status ? "Enable" : "Disable");

	uint8_t code = 0;
	readRegister(MAX31856_CONFIG_REG, 1, &code);

	if (status)
		code |= MAX31856_CONFIG_MODEAUTO; // enable autoconvert
	else
		code &= ~MAX31856_CONFIG_MODEAUTO; // disable autoconvert

	writeRegister(MAX31856_CONFIG_REG, 1, &code);
}

static void setWires(max31865_numwires_t wires)
{
	ESP_LOGI(TAG, "Set %d wires", dev.wires + 2);

	uint8_t code = 0;
	readRegister(MAX31856_CONFIG_REG, 1, &code);

	if (wires == MAX31865_3WIRE)
		code |= MAX31856_CONFIG_3WIRE; // 3 wires
	else
		code &= ~MAX31856_CONFIG_3WIRE; // 2 or 4 wire

	writeRegister(MAX31856_CONFIG_REG, 1, &code);
}

static void setNotch(max31865_notch_t notch)
{
	ESP_LOGI(TAG, "Set notch at %d Hz", (notch == MAX31865_50Hz ? 50 : 60));

	uint8_t code = 0;
	readRegister(MAX31856_CONFIG_REG, 1, &code);

	if (notch == MAX31865_50Hz)
		code |= MAX31856_CONFIG_FILT50HZ; // 50 Hz
	else
		code &= ~MAX31856_CONFIG_FILT50HZ; // 60 Hz

	writeRegister(MAX31856_CONFIG_REG, 1, &code);
}

static void oneShot(bool status)
{
	ESP_LOGI(TAG, "%s 1-shot mode", status ? "Enable" : "Disable");

	uint8_t code = 0;
	readRegister(MAX31856_CONFIG_REG, 1, &code);

	if (status)
		code |= MAX31856_CONFIG_1SHOT; // 1-shot
	else
		code &= ~MAX31856_CONFIG_1SHOT; // Auto-conversion

	writeRegister(MAX31856_CONFIG_REG, 1, &code);
}

static uint16_t readRTD(void)
{
	clearFault();
	vTaskDelay(10 / portTICK_PERIOD_MS);

	oneShot(true);
	vTaskDelay(65 / portTICK_PERIOD_MS);

	uint8_t buffer[2] = {0, 0};
	readRegister(MAX31856_RTDMSB_REG, 2, buffer);

	uint16_t rtd = (buffer[0] << 8) | buffer[1];
	rtd >>= 1; // remove fault

	return rtd;
}

static void max_spi_pre_transfer_callback(void)
{
    gpio_set_level(PIN_MAX_CS, 0);
}

static void max_spi_post_transfer_callback(void)
{
    gpio_set_level(PIN_MAX_CS, 1);
}

void max31865_enable(bool status)
{
	ESP_LOGI(TAG, "%s bias", status ? "Enable" : "Disable");

	uint8_t code = 0;
	readRegister(MAX31856_CONFIG_REG, 1, &code);

	if (status)
		code |= MAX31856_CONFIG_BIAS; // enable bias
	else
		code &= ~MAX31856_CONFIG_BIAS; // disable bias

	writeRegister(MAX31856_CONFIG_REG, 1, &code);
}

esp_err_t init_MAX31865(float _RTD_nominal, float _REF_resistor, max31865_numwires_t _wires)
{
	ESP_LOGI(TAG, "Init MAX31865 module");

	gpio_set_direction(PIN_MAX_CS, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_MAX_CS, 1);
	memset(&dev, 0, sizeof(dev));

	// Configure SPI bus
	spi_bus_config_t bus_cfg = {
		.miso_io_num = PIN_MAX_MISO,
		.mosi_io_num = PIN_MAX_MOSI,
		.sclk_io_num = PIN_MAX_SCK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1};

	ESP_LOGI(TAG, "Configuring SPI bus");
	ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_cfg, 0));

	// Configure device
	dev.RTD_nominal = _RTD_nominal;
	dev.REF_resistor = _REF_resistor;
	dev.wires = _wires;
	dev.spi_cfg.clock_speed_hz = 1 * 1000 * 1000;	// Clock out at 1 MHz
	dev.spi_cfg.address_bits = 8;
	dev.spi_cfg.command_bits = 0;
	dev.spi_cfg.mode = 1;				   			// SPI mode 1
	dev.spi_cfg.spics_io_num = -1;					// PIN_MAX_CS; Embedded CS control fails, using pre/post-transfer function to drive CS
	dev.spi_cfg.queue_size = 1;
	dev.spi_cfg.pre_cb  = (transaction_cb_t)max_spi_pre_transfer_callback;
	dev.spi_cfg.post_cb = (transaction_cb_t)max_spi_post_transfer_callback;

	ESP_LOGI(TAG, "Configuring SPI device descriptor");
	ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &dev.spi_cfg, &dev.spi_dev));

	max31865_enable(false);		// Disable bias
	setWires(dev.wires);	 	// Set number of RTD wires
	clearFault();			 	// Clear fault status
	autoConvert(false);		 	// Disable auto conversion
	setNotch(MAX31865_50Hz); 	// Set 50 Hz notch filter

	return ESP_OK;
}

esp_err_t free_MAX31865()
{
	return spi_bus_remove_device(dev.spi_dev);
}

float max31865_temperature()
{
	// http://www.analog.com/media/en/technical-documentation/application-notes/AN709_0.pdf

	float Z1, Z2, Z3, Z4, Rt, temp;

	Rt = readRTD();
	Rt /= 32768.0;
	Rt *= dev.REF_resistor;

	// Serial.print("\nResistance: "); Serial.println(Rt, 8);

	Z1 = -RTD_A;
	Z2 = RTD_A * RTD_A - (4 * RTD_B);
	Z3 = (4 * RTD_B) / dev.RTD_nominal;
	Z4 = 2 * RTD_B;

	temp = Z2 + (Z3 * Rt);
	temp = (sqrt(temp) + Z1) / Z4;

	if (temp >= 0)
		return temp;

	
	Rt /= dev.RTD_nominal;
	Rt *= 100; // normalize to 100 ohm

	float rpoly = Rt;

	temp = -242.02;
	temp += 2.2228 * rpoly;
	rpoly *= Rt; // square
	temp += 2.5859e-3 * rpoly;
	rpoly *= Rt; // ^3
	temp -= 4.8260e-6 * rpoly;
	rpoly *= Rt; // ^4
	temp -= 2.8183e-8 * rpoly;
	rpoly *= Rt; // ^5
	temp += 1.5243e-10 * rpoly;

	return temp;
}
