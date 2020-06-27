#include "AIO_Mqtt.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "math.h"
#include <string.h>

static float mqtt_old_val;
static float mqtt_new_val;

static mqtt_payload_t mqtt_payload;
static char mqtt_sub_topic[138] = "";
static char mqtt_pub_topic[138] = "";

static uint64_t mqtt_timer_alarm;
static TaskHandle_t mqtt_publisher_handle;
static TaskHandle_t mqtt_subscriber_handle;
static esp_mqtt_client_handle_t mqtt_client;
static timer_config_t cfg = {TIMER_ALARM_EN, TIMER_PAUSE, TIMER_INTR_LEVEL, TIMER_COUNT_UP, TIMER_AUTORELOAD_EN, TIMER_DIVIDER};

static const char *TAG = "AIO_Mqtt";
static aio_conf_t *conf;

/*	Scan MQTT payload
	for key-value pairs
 */
uint16_t mqtt_next_field(char* str, mqtt_payload_t* payload, uint16_t offset)
{
	uint16_t end = 0;
	uint16_t pos[4] = { 0,0,0,0 };
	int cnt = 0;

	switch (conf->mqtt_encode)
	{
	case MQTT_ENCODE_JSON:
		for (int i = offset; i <= strlen(str); i++) {
			if (str[i] == '\"')
				pos[cnt++] = i;
			if ((str[i] == ';') || (str[i] == '}')) {
				end = i;
				break;
			}
		}

		if (cnt < 4) {
			end = strlen(str);
			break;
		}

		memcpy(payload->key, (void*)&str[pos[0] + 1], pos[1] - pos[0] - 1);
		memcpy(payload->val, (void*)&str[pos[2] + 1], pos[3] - pos[2] - 1);
		payload->key[pos[1] - pos[0] - 1] = '\0';
		payload->val[pos[3] - pos[2] - 1] = '\0';
		ESP_LOGD(TAG, "JSON pair:\t%s:%s", payload->key, payload->val);
		break;

	case MQTT_ENCODE_UL20:
		for (int i = offset; i <= strlen(str); i++) {
			end = i;
			if ((str[i] == '|') || (str[i] == '\0'))
				pos[cnt++] = i;
			if (cnt == 2)
				break;
		}

		if (cnt < 2) {
			end = strlen(str);
			break;
		}

		memcpy(payload->key, (void*)&str[offset], pos[0] - offset);
		memcpy(payload->val, (void*)&str[pos[0] + 1], pos[1] - pos[0] - 1);
		payload->key[pos[0] - offset] = '\0';
		payload->val[pos[1] - pos[0] - 1] = '\0';
		ESP_LOGD(TAG, "UL pair:\t%s:%s", payload->key, payload->val);
		break;

	default:
		break;
	}

	offset = end + 1;
	return offset;
}

/*	MQTT payload marshall
	using key-value pairs
 */
void mqtt_marshall(char* key, float value)
{
	memset((void*)&mqtt_payload, 0, sizeof(mqtt_payload_t));

	switch (conf->mqtt_encode)
	{
	case MQTT_ENCODE_JSON:
		sprintf(mqtt_payload.str, "{\"%s\":\"%.3f\"}", key, value);
		break;
	case MQTT_ENCODE_UL20:
		sprintf(mqtt_payload.str, "%s|%.3f", key, value);
		break;
	default:
		break;
	}
	ESP_LOGI(TAG, "Marshall:\t%s=%.3f", key, value);
}

/*	MQTT payload unmarshall
	using key-value pairs
 */
void mqtt_unmarshall(char* str, char* key)
{
	uint16_t offset = 0;

	do {
		offset = mqtt_next_field(str, &mqtt_payload, offset);
		if (strcmp(mqtt_payload.key, key) == 0)
			break;
		memset(mqtt_payload.key, 0, 16);
		memset(mqtt_payload.val, 0, 16);
	} while (offset < strlen(str));

	if (strlen(mqtt_payload.val) > 0)
		ESP_LOGI(TAG, "Unmarshall:\t%s=%s", mqtt_payload.key, mqtt_payload.val);
}

/*	MQTT subscriber task
	handles incoming messages
 */
void mqtt_subscriber_task(void* arg)
{
	char out_key[64];

	while (1)
	{
		// Wait timer
		xTaskNotifyWait(0x00, 0xffffffff, NULL, portMAX_DELAY);
		ESP_LOGI(TAG, "Incoming message on topic: %s, %s", mqtt_sub_topic, mqtt_payload.str);

		memset(out_key, 0, 64);
		if (conf->mqtt_encode == MQTT_ENCODE_UL20){
			strcat(out_key, conf->mqtt_deviceid);
			strcat(out_key, "@");
			strcat(out_key, conf->out_key);
		} else
		{
			strcpy(out_key, conf->out_key);
		}
		
		// Unmarshal value-data pair
		mqtt_unmarshall(mqtt_payload.str, out_key);

		// Set data
		if (strlen(mqtt_payload.val) > 0)
			AIO_set(atof(mqtt_payload.val));		// AIO Board output
	}
	vTaskDelete(NULL);
}

/*	MQTT publisher task
	handles outgoing messages
 */
void mqtt_publisher_task(void* arg)
{
	while (1)
	{
		// Wait timer
		xTaskNotifyWait(0x00, 0xffffffff, NULL, portMAX_DELAY);

		// Get data
		mqtt_new_val = AIO_get(1);					// AIO Board input

		// Check report mode
		if (conf->in_log_mode == MQTT_LOG_THRESHOLD)
			if (fabsf(mqtt_new_val - mqtt_old_val) < conf->in_log_val)
				continue;

		mqtt_old_val = mqtt_new_val;

		// Marshal value-data pair
		mqtt_marshall(conf->in_key, mqtt_new_val);

		// Publish data
		esp_mqtt_client_publish(mqtt_client, mqtt_pub_topic, mqtt_payload.str, 0, conf->mqtt_pub_qos, 0);
		ESP_LOGI(TAG, "Outgoing message on topic: %s, %s", mqtt_pub_topic, mqtt_payload.str);
	}
	vTaskDelete(NULL);
}

/*	MQTT event handler
	Finite State Machine
 */
esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	switch (event->event_id) {
	case MQTT_EVENT_ANY:
		break;
		
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		
		if (strlen(mqtt_sub_topic) > 1)									// SUBSCRIBE Topic
			esp_mqtt_client_subscribe(mqtt_client, mqtt_sub_topic, conf->mqtt_sub_qos);
		
		if (strlen(mqtt_pub_topic) > 1)
			ESP_ERROR_CHECK(timer_start(TIMER_GROUP_0, TIMER_0));		// Start PUBLISH Timer
		break;

	case MQTT_EVENT_DISCONNECTED:
		ESP_ERROR_CHECK(timer_pause(TIMER_GROUP_0, TIMER_0));			// Pause PUBLISH Timer
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;

	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;

	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;

	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;

	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		memset(mqtt_payload.str, 0, 32);
		memcpy(mqtt_payload.str, event->data, event->data_len);			// Copy Msg Data
		xTaskNotify(mqtt_subscriber_handle, 0x00, eNoAction);
		break;

	case MQTT_EVENT_BEFORE_CONNECT:
		break;

	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	}
	return ESP_OK;
}

/*	Timer task ISR
 */
void IRAM_ATTR mqtt_timer_isr(void* arg)
{
	// Clear interrupt
	TIMERG0.int_clr_timers.t0 = 1;

	// Restart timer
	timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);

	// Notify task
	xTaskNotifyFromISR(mqtt_publisher_handle, 0x00, eNoAction, NULL);
	portYIELD_FROM_ISR();
}

/*	Init timer configuration
 */
esp_err_t init_MQTT_timer()
{
	uint16_t delay_ms = 100;

	if (conf->in_log_mode == MQTT_LOG_PERIODIC)
		if (conf->in_log_val > 100)
			delay_ms = conf->in_log_val;

	mqtt_timer_alarm = (uint64_t) delay_ms * TIMER_SCALE / 1000ULL;

	ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &cfg));
	ESP_ERROR_CHECK(timer_pause(TIMER_GROUP_0, TIMER_0));
	ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL));
	ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, mqtt_timer_alarm));
	ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
	ESP_ERROR_CHECK(timer_isr_register(TIMER_GROUP_0, TIMER_0, mqtt_timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL));

	ESP_LOGI(TAG, "Refresh interval set to: %d ms", delay_ms);

	return ESP_OK;
}

esp_err_t init_MQTT_topics()
{
	// Configure Topics
	char root[140] = "";
	
	if (strlen(conf->mqtt_apikey) > 0) {
		strcat(root, "/");
		strcat(root, conf->mqtt_apikey);
	}
	if (strlen(conf->mqtt_deviceid) > 0) {
		strcat(root, "/");
		strcat(root, conf->mqtt_deviceid);
	}
	if (strlen(conf->mqtt_pub) > 0) {
		strcpy(mqtt_pub_topic, root);
		strcat(mqtt_pub_topic, "/");
		strcat(mqtt_pub_topic, conf->mqtt_pub);
	}
	if (strlen(conf->mqtt_sub) > 0) {
		strcpy(mqtt_sub_topic, root);
		strcat(mqtt_sub_topic, "/");
		strcat(mqtt_sub_topic, conf->mqtt_sub);
	}

	ESP_LOGI(TAG, "PUB topic: %s", mqtt_pub_topic);
	ESP_LOGI(TAG, "SUB topic: %s", mqtt_sub_topic);

	return ESP_OK;
}

esp_err_t init_MQTT(aio_conf_t* aio_conf)
{
	ESP_LOGI(TAG, "Init MQTT module");

	conf = aio_conf;

	char uri[40];
	sprintf(uri, "%s://%s", ((conf->mqtt_encrypt == 0) ? "mqtt" : "mqtts"), conf->mqtt_host);

	esp_mqtt_client_config_t mqtt_cfg = {
		.event_handle = mqtt_event_handler,
		.uri = uri,
		.port = conf->mqtt_port,
		.client_id = conf->mqtt_deviceid,
		.username = conf->mqtt_user,
		.password = conf->mqtt_pass
		};

	xTaskCreate( mqtt_publisher_task,  "mqtt_publisher_task", 2048, NULL, 4, &mqtt_publisher_handle);
	xTaskCreate(mqtt_subscriber_task, "mqtt_subscriber_task", 2048, NULL, 4, &mqtt_subscriber_handle);

	ESP_ERROR_CHECK(init_MQTT_topics());
	ESP_ERROR_CHECK(init_MQTT_timer());
	mqtt_old_val = -1000.0;

	mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

	return esp_mqtt_client_start(mqtt_client);
}