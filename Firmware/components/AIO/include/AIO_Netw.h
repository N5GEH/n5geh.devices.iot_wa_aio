#ifndef _AIO_NETW_H_
#define _AIO_NETW_H_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "AIO_conf.h"


#ifdef __cplusplus
extern "C" {
#endif


esp_err_t init_NETW(aio_conf_t* aio_conf);
	

#ifdef __cplusplus
}
#endif //__cplusplus

#endif