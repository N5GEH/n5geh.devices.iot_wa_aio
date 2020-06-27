#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"

#include "AIO_Conf.h"
#include "AIO_Board.h"
#include "AIO_Netw.h"
#include "AIO_Mqtt.h"
#include "AIO_Intr.h"
#include "MAX31865.h"

static aio_conf_t aio_conf;
static const char *TAG = "APP";


void app_main(void)
{

	ESP_LOGI(TAG, "Startup..");
	ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());

	ESP_LOGI(TAG, "Configuring Log levels");
	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
	esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
	
	esp_log_level_set("MAX31865", ESP_LOG_WARN);
	esp_log_level_set("AIO_Board", ESP_LOG_INFO);
	esp_log_level_set("AIO_Conf", ESP_LOG_INFO);
	esp_log_level_set("AIO_Netw", ESP_LOG_INFO);
	esp_log_level_set("AIO_Mqtt", ESP_LOG_INFO);
	esp_log_level_set("AIO_Intr", ESP_LOG_INFO);


	ESP_LOGI(TAG, "Init modules..");
	init_CONF(&aio_conf, AIO_CONF_INIT_MEMORY);						// Init AIO Configuration
	init_AIO (&aio_conf);											// Init AIO Board
	init_NETW(&aio_conf);											// Init Network
	init_MQTT(&aio_conf);											// Init MQTT
	init_INTR();													// Init Interrupts

    while(1){
		vTaskDelay(100 / portTICK_PERIOD_MS);
        // vTaskDelay(portMAX_DELAY);
	}
}