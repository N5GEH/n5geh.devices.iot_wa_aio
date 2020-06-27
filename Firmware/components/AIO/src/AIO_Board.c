#include "AIO_Board.h"
#include <esp_log.h>

// ---------- Constants

#define RTD_REF			402.0
#define PWM_OFFS		0.00720000
#define PWM_GAIN		0.98930000
#define GPIO_ATTN		0.52621507	// GPIO * (R1//R3) / (R2 + R1//R3) = GPIO * (2k2//1k) / (619 + 2k2//1k) = GPIO * GPIO_ATTN
#define VREF_ATTN		0.31250000	// VREF_EXT * 1.0 / (1.0 + 2.2) = VREF_EXT * VREF_ATTN
#define AI_V_ATTN		0.09140969	// Vi = ADC / 2^12 * VREF / (3.32 / (3.32 + 33)) = ADC / 2^12 * VREF / AI_V_ATTN
#define AI_I_ATTN		44.6865752	// Ii = ADC / 2^12 * VREF / (45.3//3300)         = ADC / 2^12 * VREF / AI_I_ATTN
#define AO_V_GAIN		3.15517241	// VOUT = VDAC * (1 + 10 / 4.64) = VDAC * AO_V_GAIN

// ---------- PWM

#define PWM_HS_TIMER	LEDC_TIMER_0
#define PWM_HS_MODE		LEDC_HIGH_SPEED_MODE
#define PWM_HS_CH0_GPIO	PIN_XTR_DAC
#define PWM_HS_CH0_CHAN	LEDC_CHANNEL_0


static const char *TAG = "AIO_Board";

static aio_dev_t AIO;

// ---------- ADC Buffer
// static uint8_t	x[256];
// static float	y[256];
bool autocalibration;

// ---------- PWM descriptors
static ledc_timer_config_t	 pwm_timer;
static ledc_channel_config_t pwm_channel;

// ------------------------------------------------------
//			Configuration Section
// ------------------------------------------------------

void set_MUX(mux_state_t val)
{
	switch (val) {

	case MUX_OFF:
		ESP_LOGI(TAG, "MUX OFF");
		gpio_set_level((gpio_num_t)PIN_MUX_EN, 0);
		gpio_set_level((gpio_num_t)PIN_MUX_A0, 0);
		gpio_set_level((gpio_num_t)PIN_MUX_A1, 0);
		break;

	case MUX_ON:
		ESP_LOGI(TAG, "MUX ON");
		gpio_set_level((gpio_num_t)PIN_MUX_EN, 1);
		gpio_set_level((gpio_num_t)PIN_MUX_A0, 0);
		gpio_set_level((gpio_num_t)PIN_MUX_A1, 0);
		break;

	case MUX_VIN:
		ESP_LOGI(TAG, "MUX VIN");
		gpio_set_level((gpio_num_t)PIN_MUX_A0, 0);
		gpio_set_level((gpio_num_t)PIN_MUX_A1, 0);
		break;

	case MUX_CAL:
		ESP_LOGI(TAG, "MUX CAL");
		gpio_set_level((gpio_num_t)PIN_MUX_A0, 1);
		gpio_set_level((gpio_num_t)PIN_MUX_A1, 0);
		break;

	case MUX_AGND:
		ESP_LOGI(TAG, "MUX AGND");
		gpio_set_level((gpio_num_t)PIN_MUX_A0, 0);
		gpio_set_level((gpio_num_t)PIN_MUX_A1, 1);
		break;

	case MUX_VOUT:
		ESP_LOGI(TAG, "MUX VOUT");
		gpio_set_level((gpio_num_t)PIN_MUX_A0, 1);
		gpio_set_level((gpio_num_t)PIN_MUX_A1, 1);
		break;

	default:
		break;
	}
	vTaskDelay(500 / portTICK_PERIOD_MS);		// 5 Time Constants
}

float map_raw(float x, float _in_min, float _in_max, float _out_min, float _out_max)
{
	float m = (_out_max - _out_min) / (_in_max - _in_min);
	float y = (m * (x - _in_min) + _out_min);
	return y;
}

float map(float x, aio_conv_t _type)
{
	switch (_type)
	{
	case AI_CODE_TO_VOLT:
		return (AIO.VREF_ADC * x / AIO.AI_code_max);		// V = VREF * CODE / 2^12, V = [0,VREF]
		break;

	case AI_VOLT_TO_PHYS:
		if ((AIO.AO == AIO_VOLTAGE_0_10) || (AIO.AO == AIO_VOLTAGE_2_10))
			return map_raw(x, 0, AIO.VREF_ADC, 0.0, AIO.VREF_ADC / AI_V_ATTN);
		if ((AIO.AO == AIO_CURRENT_0_20) || (AIO.AO == AIO_CURRENT_4_20))
			return map_raw(x, 0, AIO.VREF_ADC, 0.0, AIO.VREF_ADC / AI_I_ATTN * 1000.0);
		break;

	case AI_PHYS_TO_UNIT:
		return map_raw(x, AIO.AI_phys_min, AIO.AI_phys_max, AIO.AI_unit_min, AIO.AI_unit_max);
		break;

	case AO_UNIT_TO_PHYS:
		return map_raw(x, AIO.AO_unit_min, AIO.AO_unit_max, AIO.AO_phys_min, AIO.AO_phys_max);
		break;

	case AO_PHYS_TO_VOLT:
		if ((AIO.AO == AIO_VOLTAGE_0_10) || (AIO.AO == AIO_CURRENT_0_20))
			return map_raw(x, AIO.AO_phys_min, AIO.AO_phys_max, 0.0 / AO_V_GAIN, 10.0 / AO_V_GAIN);
		if ((AIO.AO == AIO_VOLTAGE_2_10) || (AIO.AO == AIO_CURRENT_4_20))
			return map_raw(x, AIO.AO_phys_min, AIO.AO_phys_max, 2.0 / AO_V_GAIN, 10.0 / AO_V_GAIN);
		break;

	case AO_VOLT_TO_CODE:
		return (x - PWM_OFFS) * AIO.AO_code_max / AIO.VSUP / PWM_GAIN;		// V = CODE / 2^9 * VSUP * PWMG + PWMO, V = [0,VSUP]
		break;

	default:
		break;
	}

	return 0;
}

void set_raw_channel(uint8_t ch)
{
	pwm_channel.gpio_num = ch;
	ledc_channel_config(&pwm_channel);
}

void set_raw(uint16_t code)
{
	ledc_set_duty(pwm_channel.speed_mode, pwm_channel.channel, code & 0x01FF);
	ledc_update_duty(pwm_channel.speed_mode, pwm_channel.channel);
}

float get_raw(uint16_t n)
{
	double sum = 0;

	for (int i = 0; i < n; i++) {
		if (AIO.AI == AIO_PASSIVE)
			if(!autocalibration) {
				sum += max31865_temperature();
				continue;
			}
		
		sum += adc1_get_raw((adc1_channel_t)ADC_CHANNEL_1);
	}
	float _code = sum / ((float)n);

	return _code;
}

void AIO_set(float _unit)
{
	if (AIO.AO == AIO_DISABLED)
		return;

	if (_unit > AIO.AO_unit_max)
		_unit = AIO.AO_unit_max;
	if (_unit < AIO.AO_unit_min)
		_unit = AIO.AO_unit_min;

	float _phys = map(_unit, AO_UNIT_TO_PHYS);
	float _volt = map(_phys, AO_PHYS_TO_VOLT);
	float _code = map(_volt, AO_VOLT_TO_CODE);

	set_raw(_code);
}

float AIO_get(uint16_t n)
{
	if (AIO.AI == AIO_DISABLED)
		return 0.0 / 0.0;

	if (AIO.AI == AIO_PASSIVE)
		return get_raw(n);

	float _code = get_raw(n);
	float _volt = map(_code, AI_CODE_TO_VOLT);
	float _phys = map(_volt, AI_VOLT_TO_PHYS);
	float _unit = map(_phys, AI_PHYS_TO_UNIT);

	return _unit;
}

// ------------------------------------------------------
//			Calibration
// ------------------------------------------------------

void cal_OFFS()
{
	ESP_LOGI(TAG, "OFFS calibration");

	dac_output_voltage((dac_channel_t)PIN_DAC, 0);

	// Set MUX to Vref mode
	set_MUX(MUX_AGND);

	// Voltage sweep
	float off[256];
	for (uint8_t i = 0; i < 256; i++) {
		dac_output_voltage((dac_channel_t)PIN_DAC, i);
		off[i] = get_raw(100);
		ESP_LOGD(TAG, "ADC offset search: %d\t%.3f", i, off[i]);
		if (off[i] != 0) {
			AIO.OFFS_CAL = i - 1;
			break;
		}
	}
	dac_output_voltage((dac_channel_t)PIN_DAC, 0);

	// Apply offset compensation
	dac_output_voltage((dac_channel_t)PIN_DAC, AIO.OFFS_CAL);

	ESP_LOGI(TAG, "ADC offset value: %d", AIO.OFFS_CAL);
}

void cal_VREF()
{
	ESP_LOGI(TAG, "VREF calibration");

	// Set calibration pin to high Z
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL << PIN_CAL);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	// Set MUX to Calibration mode
	set_MUX(MUX_CAL);

	// Calculate VREF
	float ADC_CODE = get_raw(1000);
	ESP_LOGD(TAG, "ADC reference code: %.3f", ADC_CODE);
	AIO.VREF_ADC = AIO.VREF_EXT * AIO.AI_code_max * VREF_ATTN / ADC_CODE;	// ADC_CODE * LSB = ADC * VREF_ADC / 2^12 = VREF_EXT * 1/(1 + 2.2) = VREF_EXT * VREF_ATTN

	// Calculate ADC Offset
	dac_output_voltage((dac_channel_t)PIN_DAC, 0);
	set_MUX(MUX_CAL);
	AIO.OFFS_ADC = map(get_raw(100), AI_CODE_TO_VOLT) - (AIO.VREF_EXT * VREF_ATTN);

	// Restore compensation
	dac_output_voltage((dac_channel_t)PIN_DAC, AIO.OFFS_CAL);
	
	ESP_LOGI(TAG, "ADC reference voltage: %.4f", AIO.VREF_ADC);
	ESP_LOGI(TAG, "ADC offset voltage: %.4f", AIO.OFFS_ADC);
}

void cal_VSUP()
{
	ESP_LOGI(TAG, "VSUP calibration");

	uint8_t	x[256];
	float	y[256];

	// Route PWM to calibration pin
	set_raw_channel(PIN_CAL);
	set_raw(0);

	// Set MUX to Calibration mode
	set_MUX(MUX_CAL);

	// Voltage sweep
	for (int i = 0; i < 256; i++) {
		set_raw(i);
		vTaskDelay(1 / portTICK_PERIOD_MS);
		x[i] = i;
		y[i] = map(get_raw(10), AI_CODE_TO_VOLT);
	}
	set_raw(0);

	// Set calibration pin to high Z
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL << PIN_CAL);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	// Route PWM to output pin
	set_raw_channel(PIN_XTR_DAC);
	set_raw(0);

	// Linear Regression (y = bx + a)
	int N = 200;
	float sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0, a, b;
	for (int i = 1; i <= N; i++)
	{
		sumX = sumX + x[i];
		sumX2 = sumX2 + x[i] * x[i];
		sumY = sumY + y[i];
		sumXY = sumXY + x[i] * y[i];
	}
	/* Calculating a and b */
	b = (N * sumXY - sumX * sumY) / (N * sumX2 - sumX * sumX);
	a = (sumY - b * sumX) / N;
	AIO.VSUP = b * 512.0 / GPIO_ATTN;

	ESP_LOGI(TAG, "Supply voltage: %.4f", AIO.VSUP);
}

void cal_VOUT()
{
	ESP_LOGI(TAG, "VOUT calibration");
	//
	set_raw(512);

	// Set MUX to Ouput mode
	set_MUX(MUX_VOUT);

	float ADC_CODE = get_raw(1000);
	AIO.VSUP1 = AIO.VREF_ADC * ADC_CODE / (AI_V_ATTN * AO_V_GAIN);			// VSUP * AO_V_GAIN * AI_V_ATTN = ADC = VREF * ADC_CODE

	ESP_LOGD(TAG, "Supply voltage: %.4f", AIO.VSUP1);
}

void AIO_autocalibration()
{
	ESP_LOGI(TAG, "AIO auto-calibration");

	// Enable MUX
	set_MUX(MUX_ON);

	autocalibration = true;

	// Offset
	cal_OFFS();

	// Gain
	cal_VREF();

	// Supply voltage
	cal_VSUP();

	// Output
	cal_VOUT();

	autocalibration = false;

	// Enable MUX input
	set_MUX(MUX_VIN);
}

// ------------------------------------------------------
//			Initialization
// ------------------------------------------------------

esp_err_t init_GPIO()
{
	ESP_LOGI(TAG, "GPIO init");
	//pinMode(PIN_XTR_EF,  INPUT);

	gpio_config_t io_conf;

	// Disable MUX
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = ((1ULL << PIN_MUX_EN) | (1ULL << PIN_MUX_A0) | (1ULL << PIN_MUX_A1));
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	ESP_ERROR_CHECK(gpio_config(&io_conf));

	gpio_set_level((gpio_num_t)PIN_MUX_EN, 0);
	gpio_set_level((gpio_num_t)PIN_MUX_A0, 0);
	gpio_set_level((gpio_num_t)PIN_MUX_A1, 0);

	// Disable XTR
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = (1ULL << PIN_XTR_OD);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	ESP_ERROR_CHECK(gpio_config(&io_conf));

	gpio_set_level((gpio_num_t)PIN_XTR_OD, 1);
	return ESP_OK;
}

esp_err_t init_ADC()
{
	ESP_LOGI(TAG, "ADC init");
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten((adc1_channel_t)ADC_CHANNEL_1, ADC_ATTEN_DB_0);
	return ESP_OK;
}

esp_err_t init_DAC()
{
	ESP_LOGI(TAG, "DAC init");
	dac_output_enable((dac_channel_t)PIN_DAC);
	dac_output_voltage((dac_channel_t)PIN_DAC, 0);
	return ESP_OK;
}

esp_err_t init_PWM()
{
	ESP_LOGI(TAG, "PWM init");
	pwm_timer.duty_resolution = LEDC_TIMER_9_BIT;	// resolution of PWM duty
	pwm_timer.freq_hz = 100000;						// frequency of PWM signal
	pwm_timer.speed_mode = PWM_HS_MODE;				// timer mode
	pwm_timer.timer_num = PWM_HS_TIMER;				// timer index
	ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

	pwm_channel.channel = PWM_HS_CH0_CHAN;
	pwm_channel.duty = 0;
	pwm_channel.gpio_num = PWM_HS_CH0_GPIO;
	pwm_channel.speed_mode = PWM_HS_MODE;
	pwm_channel.hpoint = 0;
	pwm_channel.timer_sel = PWM_HS_TIMER;
	ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));

	return ESP_OK;
}

esp_err_t init_RTD(aio_conf_t* conf)
{
	ESP_LOGI(TAG, "RTD init");
	float _RTD_nominal;
	max31865_numwires_t _RTD_wires;

	if (conf->in_rtd == 0)
		_RTD_nominal = 100.00;
	else
		_RTD_nominal = 1000.00;

	switch (conf->in_wires)
	{
	case 0:
		_RTD_wires = MAX31865_2WIRE;
		break;
	case 1:
		_RTD_wires = MAX31865_3WIRE;
		break;
	case 2:
		_RTD_wires = MAX31865_4WIRE;
		break;
	default:
		_RTD_wires = MAX31865_2WIRE;
		break;
	}

	return init_MAX31865(_RTD_nominal, RTD_REF, _RTD_wires);
}

esp_err_t init_DEV(aio_conf_t* conf)
{
	ESP_LOGI(TAG, "DEV init");
	AIO.AI = (aio_mode_t)conf->in_mode;
	AIO.AO = (aio_mode_t)conf->out_mode;

	switch (AIO.AO)
	{
	case AIO_DISABLED:
		AIO.AO_phys_min = 0;
		AIO.AO_phys_max = 0;
		break;

	case AIO_VOLTAGE_0_10:
		AIO.AO_phys_min = 0;
		AIO.AO_phys_max = 10;
		break;

	case AIO_VOLTAGE_2_10:
		AIO.AO_phys_min = 2;
		AIO.AO_phys_max = 10;
		break;

	case AIO_CURRENT_0_20:
		AIO.AO_phys_min = 0;
		AIO.AO_phys_max = 20;
		break;

	case AIO_CURRENT_4_20:
		AIO.AO_phys_min = 4;
		AIO.AO_phys_max = 20;
		break;

	default:
		AIO.AO = AIO_DISABLED;
		AIO.AO_phys_min = 0;
		AIO.AO_phys_max = 0;
		break;
	}

	switch (AIO.AI)
	{
	case AIO_DISABLED:
		AIO.AI_phys_min = 0;
		AIO.AI_phys_max = 0;
		break;

	case AIO_VOLTAGE_0_10:
		AIO.AI_phys_min = 0;
		AIO.AI_phys_max = 10;
		break;

	case AIO_VOLTAGE_2_10:
		AIO.AI_phys_min = 2;
		AIO.AI_phys_max = 10;
		break;

	case AIO_CURRENT_0_20:
		AIO.AI_phys_min = 0;
		AIO.AI_phys_max = 20;
		break;

	case AIO_CURRENT_4_20:
		AIO.AI_phys_min = 4;
		AIO.AI_phys_max = 20;
		break;

	case AIO_PASSIVE:
		AIO.AI_phys_min = 0;
		AIO.AI_phys_max = 0;
		break;

	default:
		AIO.AI = AIO_DISABLED;
		AIO.AI_phys_min = 0;
		AIO.AI_phys_max = 0;
		break;
	}

	AIO.AI_unit_min = conf->in_min;
	AIO.AI_unit_max = conf->in_max;
	AIO.AO_unit_min = conf->out_min;
	AIO.AO_unit_max = conf->out_max;

	AIO.AI_code_max = 4096.0;
	AIO.AI_code_min = 0.0;
	AIO.AO_code_max = 512.0;
	AIO.AO_code_min = 0.0;

	AIO.VREF_EXT = 2.048;
	AIO.VREF_ADC = 1.1;
	AIO.OFFS_ADC = 0.0;
	AIO.OFFS_CAL = 0;
	AIO.VSUP = 3.3;
	AIO.VSUP1 = 3.3;
	// AIO.VSUP2 = 3.3;
	
	return ESP_OK;
}

esp_err_t init_AIO(aio_conf_t* aio_conf)
{
	ESP_LOGI(TAG, "Init AIO module");

	init_GPIO();
	init_ADC();
	init_DAC();
	init_PWM();
	init_RTD(aio_conf);
	init_DEV(aio_conf);

	autocalibration = false;

	if (aio_conf->in_mode == AIO_PASSIVE)
		max31865_enable(true);		// Enable MAX31865
	if (((aio_conf->in_mode || aio_conf->out_mode) && 0x03) != AIO_DISABLED)
		AIO_autocalibration();	// Autocalibration
	if (aio_conf->out_mode != AIO_DISABLED)
		gpio_set_level((gpio_num_t)PIN_XTR_OD, 0);
	
	return ESP_OK;
}
