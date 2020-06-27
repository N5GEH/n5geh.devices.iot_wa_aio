#ifndef _AIO_BOARD_H_
#define _AIO_BOARD_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "MAX31865.h"
#include "AIO_Conf.h"


// ---------- GPIO Definitions
#define PIN_RESET		 0

#define PIN_ADC			37	// ADC1_CH1
#define PIN_DAC			DAC_CHANNEL_2	// 26
#define PIN_CAL			 4

#define PIN_XTR_OD		12	// Output Disable
#define PIN_XTR_EF		14	// Output Failure
#define PIN_XTR_DAC		15	// Output PWM

#define PIN_MAX_CS		 5	// VSPICS0	SPI Chips Select
#define PIN_MAX_RDY		 9	//			SPI Data Ready
#define PIN_MAX_SCK		18	// VSPICLK	SPI Clock
#define PIN_MAX_MISO	19	// VSPIQ	SPI MISO
#define PIN_MAX_MOSI	23	// VSPID	SPI Chips Select

#define PIN_MUX_EN		22	// MUX Enable
#define PIN_MUX_A0		10	// MUX Select line 0
#define PIN_MUX_A1		21	// MUX Select line 1

// ---------- MUX

typedef enum {
	MUX_OFF = 0,
	MUX_ON,
	MUX_VIN,
	MUX_CAL,
	MUX_AGND,
	MUX_VOUT
} mux_state_t;

// ---------- AIO Modes

typedef enum {
	AIO_DISABLED = 0,
	AIO_VOLTAGE_0_10,
	AIO_VOLTAGE_2_10,
	AIO_CURRENT_0_20,
	AIO_CURRENT_4_20,
	AIO_PASSIVE
} aio_mode_t;

typedef enum {
	AI_CODE_TO_VOLT = 0,
	AI_VOLT_TO_PHYS,
	AI_PHYS_TO_UNIT,
	AO_UNIT_TO_PHYS,
	AO_PHYS_TO_VOLT,
	AO_VOLT_TO_CODE
} aio_conv_t;

typedef struct {
	aio_mode_t AI;
	aio_mode_t AO;

	float	AI_unit_max;
	float	AI_unit_min;
	float	AO_unit_max;
	float	AO_unit_min;

	float	AI_phys_max;
	float	AI_phys_min;
	float	AO_phys_max;
	float	AO_phys_min;

	float	AI_code_max;
	float	AI_code_min;
	float	AO_code_max;
	float	AO_code_min;

	uint8_t	OFFS_CAL;
	float	OFFS_ADC;
	float	VREF_EXT;
	float	VREF_ADC;
	float	VSUP;
	float	VSUP1;
} aio_dev_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t init_AIO(aio_conf_t *aio_conf);

void  AIO_autocalibration();
void  AIO_set(float _unit);
float AIO_get(uint16_t n);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif