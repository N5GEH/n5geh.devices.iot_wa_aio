#ifndef _AIO_MQTT_H_
#define _AIO_MQTT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "AIO_Conf.h"
#include "AIO_Board.h"

#define TIMER_DIVIDER 50000
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	MQTT_ENCODE_JSON = 0,
	MQTT_ENCODE_UL20
} mqtt_encode_t;

typedef enum
{
	MQTT_LOG_PERIODIC = 0,
	MQTT_LOG_THRESHOLD
} mqtt_log_mode_t;

typedef struct
{
	char key[16];
	char val[16];
	char str[32];
} mqtt_payload_t;


esp_err_t init_MQTT(aio_conf_t *aio_conf);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif