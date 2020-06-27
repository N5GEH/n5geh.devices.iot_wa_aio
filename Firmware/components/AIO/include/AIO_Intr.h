#ifndef _AIO_INTR_H_
#define _AIO_INTR_H_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "time.h"

#include "AIO_Board.h"
#include "AIO_Conf.h"
#include "AIO_Netw.h"


#define ESP_INTR_FLAG_DEFAULT 0

#ifdef __cplusplus
extern "C" {
#endif


esp_err_t init_INTR();


#ifdef __cplusplus
}
#endif //__cplusplus

#endif