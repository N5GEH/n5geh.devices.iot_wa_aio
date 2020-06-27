#ifndef _AIO_CONF_H_
#define _AIO_CONF_H_

#define STORAGE_NAMESPACE "aio_storage"

#include "nvs_flash.h"

typedef struct {

	// WiFi AP
	char wifi_ap_ssid[32];
	char wifi_ap_pass[64];
	
	// WiFi STA
	char wifi_sta_ssid[32];
	char wifi_sta_pass[64];

	// MQTT
		// Broker
	uint8_t mqtt_encrypt;
	char	mqtt_host[32];
	uint16_t mqtt_port;

	char	mqtt_user[32];
	char	mqtt_pass[32];

		// TOPICS
	char	mqtt_apikey[32];
	char	mqtt_deviceid[32];

	char	mqtt_pub[72];
	uint8_t mqtt_pub_qos;
	char	mqtt_sub[72];
	uint8_t mqtt_sub_qos;
	uint8_t mqtt_encode;

	// AIO Interface
		//INPUT
	uint8_t in_mode;
	char	in_key[16];
	float	in_min;
	float	in_max;
	uint8_t in_log_mode;
	float	in_log_val;
	uint8_t in_rtd;
	uint8_t in_wires;

		//OUTPUT
	uint8_t out_mode;
	char	out_key[16];
	float	out_min;
	float	out_max;
} aio_conf_t;

typedef struct {
	char name[72];
	char value[72];
	uint16_t offset;
} aio_conf_field_t;

typedef enum {
	AIO_CONF_INIT_MEMORY = 0,
	AIO_CONF_INIT_DEFAULT,
	AIO_CONF_INIT_CUSTOM
} aio_conf_init_t;

static const aio_conf_t aio_conf_default = { "", "", "", "", 0, "", 0, "", "", "api-key", "dev-id", "attrs", 0, "cmd", 0, 0, 0, "", 0, 0, 0, 1000, 0, 0, 0, "", 0, 0 };
// static const aio_conf_t aio_conf_custom  = { "ESP32", "123456789", "OPPO", "asdfghjkl",0,"farmer.cloudmqtt.com",16273,"tlnmiopn","rXG8xwwEiSBJ","","urn:something:meter:002","attrs",0,"cmd",0,0,0,"temp",0.0,50.0,0,1000,0,0,0,"pres",100.0,150.0 };
static const aio_conf_t aio_conf_custom  = { "", "", "FRITZ!Box 7530 TW", "31361912602018850594",0,"farmer.cloudmqtt.com",16273,"tlnmiopn","rXG8xwwEiSBJ","1234","urn:sm","attrs",0,"cmd",0,1,5,"temp",0.0,50.0,1,0.3,0,0,1,"valve",0.0,90.0 };

static const char html_request[]	   = "<!DOCTYPE html><html><head><title>IoT Gate</title><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"><link rel=\"icon\" href=\"data:,\"><style>html{font-family: Helvetica; font-size: 90%%;}input[type=text], select{width: 100%%; padding: 7px; border: 1px solid #ccc; border-radius: 4px; resize: vertical; margin: 0px 0px 5px 0px; box-sizing: border-box;}input[type=submit]{background-color: #4CAF50; border: none; color: white; padding: 16px 32px; text-decoration: none; margin: 4px 2px; cursor: pointer; width: 100%%;}fieldset{border: 2px solid; border-radius: 3px; padding: 7px 7px 7px 7px; margin: 7px 0px;}.ext{border-color: #F00;}.container{background-color: #f2f2f2; padding: 10px;}.col{float: left; margin-top: 6px;}.row:after{content: \"\"; display: table; clear: both;}</style></head><body><div class=\"container\"><h2 align=\"center\">IoT AIO Configurator</h2><form action=\"/\" method=\"post\" enctype=\"text/plain\"><fieldset><legend>WiFi:</legend><fieldset class=\"ext\"><legend>Access Point:</legend>SSID:<br><input type=\"text\" name=\"ssid_ap\" value=\"%s\"><br>PASS:<br><input type=\"text\" name=\"pass_ap\" value=\"%s\"></fieldset><fieldset class=\"ext\"><legend>Station:</legend>SSID:<br><input type=\"text\" name=\"ssid_sta\" value=\"%s\"><br>PASS:<br><input type=\"text\" name=\"pass_sta\" value=\"%s\"></fieldset></fieldset><fieldset><legend>MQTT:</legend><fieldset class=\"ext\"><legend>IoT Agent:</legend><div class=\"row\"><div class=\"col\" style=\"width:23%%\">Crypt:<br><select name=\"scheme\"><option value=\"0\"%s>None</option><option value=\"1\"%s>TLS</option></select></div><div class=\"col\" style=\"width:60%%\">Host:<br><input type=\"text\" name=\"host\" value=\"%s\"></div><div class=\"col\" style=\"width:17%%\">Port:<br><input type=\"text\" name=\"port\" value=\"%d\"></div></div><div class=\"row\"><div class=\"col\" style=\"width:50%%\">User Name:<br><input type=\"text\" name=\"user\" value=\"%s\"></div><div class=\"col\" style=\"width:50%%\">Password:<br><input type=\"text\" name=\"key\" value=\"%s\"></div></div></fieldset><fieldset class=\"ext\"><legend>Topics:</legend>API-Key:<br><input type=\"text\" name=\"apikey\" value=\"%s\"><br>Device-ID:<br><input type=\"text\" name=\"id\" value=\"%s\"><br><div class=\"row\"><div class=\"col\" style=\"width:75%%\">Publish:<br><input type=\"text\" name=\"pub_topic\" value=\"%s\" placeholder=\"attrs\"></div><div class=\"col\" style=\"width:25%%\"><br><select name=\"pub_qos\"><option value=\"0\"%s>QoS 0 [&lt 1]</option><option value=\"1\"%s>QoS 1 [&gt 1]</option><option value=\"2\"%s>QoS 2 [=1]</option></select></div></div><div class=\"row\"><div class=\"col\" style=\"width:75%%\">Subscribe:<br><input type=\"text\" name=\"sub_topic\" value=\"%s\" placeholder=\"cmd\"></div><div class=\"col\" style=\"width:25%%\"><br><select name=\"sub_qos\"><option value=\"0\"%s>QoS 0 [&lt 1]</option><option value=\"1\"%s>QoS 1 [&gt 1]</option><option value=\"2\"%s>QoS 2 [=1]</option></select></div></div><div class=\"row\"><div class=\"col\" style=\"width:100%%\"> Encoding:<br><select name=\"encode\"><option value=\"0\"%s>JSON -{\"key\":\"value\"}</option><option value=\"1\"%s>UL2.0 - key|value</option></select></div></div><font size=\"2\"><i>&midast;TOPIC: /&ltapi-key&gt/&ltdevice-id&gt/&ltmode&gt</i></font></fieldset></fieldset><fieldset><legend>AIO Interface:</legend><fieldset class=\"ext\"><legend>INPUT:</legend><div class=\"row\"><div class=\"col\" style=\"width:100%%\">Phys:<br><select name=\"in\"><option value=\"0\"%s>Disabled</option><option value=\"1\"%s>Voltage 0-10 V</option><option value=\"2\"%s>Voltage 2-10 V</option><option value=\"3\"%s>Current 0-20 mA</option><option value=\"4\"%s>Current 4-20 mA</option><option value=\"5\"%s>Passive</option></select></div></div><div class=\"row\"><div class=\"col\" style=\"width:25%%;margin-top:30px\">Sensor: </div><div class=\"col\" style=\"width:30%%\">Attribute:<br><input type=\"text\" name=\"in_key\" value=\"%s\"></div><div class=\"col\" style=\"width:22%%\">Min:<br><input type=\"text\" name=\"in_min\" value=\"%.2f\"></div><div class=\"col\" style=\"width:23%%\">Max:<br><input type=\"text\" name=\"in_max\" value=\"%.2f\"></div></div><div class=\"row\"><div class=\"col\" style=\"width:25%%;margin-top:15px\">Mode: </div><div class=\"col\" style=\"width:30%%\"><select name=\"mode\"><option value=\"0\"%s>Periodic</option><option value=\"1\"%s>Threshold</option></select></div><div class=\"col\" style=\"width:45%%\"><input type=\"text\" name=\"mode_val\" value=\"%.2f\" placeholder=\"Rate [ms] / Value [-]\"></div></div><div class=\"row\"><div class=\"col\" style=\"width:25%%;margin-top:15px\">RTD: </div><div class=\"col\" style=\"width:30%%\"><select name=\"rtd\"><option value=\"0\"%s>PT100</option><option value=\"1\"%s>PT1000</option></select></div><div class=\"col\" style=\"width:45%%\"><select name=\"wires\"><option value=\"0\"%s>2 wires</option><option value=\"1\"%s>3 wires</option><option value=\"2\"%s>4 wires</option></select></div></div></fieldset><fieldset class=\"ext\"><legend>OUTPUT:</legend><div class=\"row\"><div class=\"col\" style=\"width:100%%\">Phys:<br><select name=\"out\"><option value=\"0\"%s>Disabled</option><option value=\"1\"%s>Voltage 0-10 V</option><option value=\"2\"%s>Voltage 2-10 V</option><option value=\"3\"%s>Current 0-20 mA</option><option value=\"4\"%s>Current 4-20 mA</option></select></div></div><div class=\"row\"><div class=\"col\" style=\"width:25%%;margin-top:30px\">Actuator: </div><div class=\"col\" style=\"width:30%%\">Command:<br><input type=\"text\" name=\"out_key\" value=\"%s\"></div><div class=\"col\" style=\"width:22%%\">Min:<br><input type=\"text\" name=\"out_min\" value=\"%.2f\"></div><div class=\"col\" style=\"width:23%%\">Max:<br><input type=\"text\" name=\"out_max\" value=\"%.2f\"></div></div></fieldset></fieldset><input type=\"submit\" value=\"Submit\"></form></div></body></html>";
static const char html_response_pass[] = "<!DOCTYPE html><html><head><title>IoT Gate</title><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"><link rel=\"icon\" href=\"data:,\"><style>html{font-family: Helvetica;}input[type=submit]{background-color: #4CAF50; border: none; color: white; padding: 16px 32px; text-decoration: none; margin: 4px 2px; cursor: pointer; width: 100%%;}.container{background-color: #f2f2f2; padding: 10px;}</style></head><body><div class=\"container\"><h2>IoT Configurator</h2><br><input type=\"submit\" value=\"SUCCESFUL\"></div></body></html>";
static const char html_response_fail[] = "<!DOCTYPE html><html><head><title>IoT Gate</title><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"><link rel=\"icon\" href=\"data:,\"><style>html{font-family: Helvetica;}input[type=submit]{background-color: #fe7070; border: none; color: white; padding: 16px 32px; text-decoration: none; margin: 4px 2px; cursor: pointer; width: 100%%;}.container{background-color: #f2f2f2; padding: 10px;}</style></head><body><div class=\"container\"><h2>IoT Configurator</h2><br><input type=\"submit\" value=\"FAIL\"></div></body></html>";

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t  export_aio_conf(aio_conf_t* conf, char* str, const char* reg_str);
esp_err_t  import_aio_conf(aio_conf_t* conf, char* str);
esp_err_t	write_aio_conf(aio_conf_t* conf);
void		print_aio_conf(aio_conf_t* conf);
esp_err_t	 init_CONF(aio_conf_t* conf, aio_conf_init_t _init);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif