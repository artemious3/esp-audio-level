
#include <esp_a2dp_api.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_bt_api.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <math.h>

#include <stdint.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sdkconfig.h"
#include "bt.h"


extern float g_last_db_level;


static void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param){
	switch (event) { 
		case ESP_A2D_CONNECTION_STATE_EVT:
			if(param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
				printf("CONNECTED!!! :>>>>\n");
				gpio_set_level(CONFIG_GPIO_BT_STATE_LED, 1);
			} else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED){
				printf("DISCONNECTED!!! :<<<<<\n");
				gpio_set_level(CONFIG_GPIO_BT_STATE_LED, 0);
			}
			break;
		default:
			break;
	
	}
}



static void bt_data_cb(const uint8_t *buf_bytes, uint32_t len_bytes) {
	const int16_t * buf = (int16_t*) buf_bytes;
	const int32_t len = len_bytes / 2;
	int32_t acc = 0;
	int32_t average_sample = 0;
	float level_db;

	for(int i = 0; i < len; i++){
		acc += abs(buf[i]);
	}
	average_sample = acc / (len);
	level_db = 10 * log10((float)(average_sample) / (float)(INT16_MAX));
	g_last_db_level = level_db;
}



void bt_init() {

  ESP_ERROR_CHECK(nvs_flash_init());

  esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_bt_controller_init(&cfg));

  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));

  ESP_ERROR_CHECK(esp_bluedroid_init());

  ESP_ERROR_CHECK(esp_bluedroid_enable());

  ESP_ERROR_CHECK(esp_bt_gap_set_device_name("ESP-EQ"));

  ESP_ERROR_CHECK(esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE,
                                           ESP_BT_GENERAL_DISCOVERABLE));

  ESP_ERROR_CHECK(esp_a2d_sink_init());

  ESP_ERROR_CHECK(esp_a2d_register_callback(bt_a2dp_cb));

  ESP_ERROR_CHECK(esp_a2d_sink_register_data_callback(bt_data_cb));

}




