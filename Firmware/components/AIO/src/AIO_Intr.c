#include "AIO_Intr.h"
#include <esp_log.h>

static const char *TAG = "AIO_Intr";

static bool intr_status;
static uint32_t intr_time_ms;
static xQueueHandle gpio_evt_queue = NULL;

/*	Asynchronous GPIO ISR
 */
static void IRAM_ATTR intr_isr_handler(void* arg)
{
	uint32_t gpio_num = (uint32_t)arg;
	xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

/*	Synchronous GPIO ISR
	Finite State Machine
 */
static void intr_task_handler(void* arg)
{
	uint32_t io_num;
	while(1)
	{
		if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
			printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level((gpio_num_t)io_num));

			if (intr_status) {
				intr_time_ms = (uint32_t)(clock() * 1000 / CLOCKS_PER_SEC) - intr_time_ms;

				if (intr_time_ms < 1000)
					esp_restart();

				if ((intr_time_ms > 1000) && (intr_time_ms < 5000)) {
					wifi_config_t cfg;
					esp_wifi_get_config(ESP_IF_WIFI_AP, &cfg);
					if (cfg.ap.ssid_hidden == 1) {
						cfg.ap.ssid_hidden = 0;
						ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &cfg));
						ESP_ERROR_CHECK(esp_wifi_start());
					}
				}

				if (intr_time_ms > 5000) {
					write_aio_conf((aio_conf_t*)&aio_conf_default);
					vTaskDelay(500 / portTICK_PERIOD_MS);
					esp_restart();
				}
			}
			else {
				intr_time_ms = (uint32_t)(clock() * 1000/ CLOCKS_PER_SEC);
			}

			intr_status = !intr_status;
		}
	}
}

esp_err_t init_INTR()
{
	ESP_LOGI(TAG, "Init GPIO interrupt module");

	intr_status = false;
	intr_time_ms = 0;

	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << PIN_RESET),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_ANYEDGE
	};
	ESP_ERROR_CHECK(gpio_config(&io_conf));

	//create a queue to handle gpio event from isr
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	
	//start gpio task
	xTaskCreate(intr_task_handler, "intr_task_handler", 2048, NULL, 10, NULL);

	//install gpio isr service
	ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
	
	//hook isr handler for specific gpio pin
	return gpio_isr_handler_add((gpio_num_t)PIN_RESET, intr_isr_handler, (void*)PIN_RESET);
}