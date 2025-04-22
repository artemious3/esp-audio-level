#include <esp_a2dp_api.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_bt_api.h>
#include <esp_err.h>
#include <nvs_flash.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "driver/gpio.h"
#include "freertos/task.h"

/*
 * GPIO outputs for Bluetooth state and for audio
 */
#define BT_STATE_LED 2
#define BT_AUDIO_LED 4
#define NUM_CB_TO_COUNT_AVG 32


/* average samples per last `NUM_CB_TO_COUNT_AVG` callbacks*/
int32_t g_average_levels[NUM_CB_TO_COUNT_AVG];
int32_t g_average;


void calculate_global_average(int32_t next_avg)
{
	for(int i = 1; i < NUM_CB_TO_COUNT_AVG; i++){
		g_average_levels[i-1] = g_average_levels[i];
	}
	g_average_levels[NUM_CB_TO_COUNT_AVG-1] = next_avg;

	int32_t acc = 0;
	for(int i = 0; i < NUM_CB_TO_COUNT_AVG; i++){
		acc += g_average_levels[i];
	}
	g_average = acc / NUM_CB_TO_COUNT_AVG;

}

void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param){
	switch (event) { 
		case ESP_A2D_CONNECTION_STATE_EVT:
			if(param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
				printf("CONNECTED!!! :>>>>\n");
				gpio_set_level(BT_STATE_LED, 1);
			} else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED){
				printf("DISCONNECTED!!! :<<<<<\n");
				gpio_set_level(BT_STATE_LED, 0);
			}
			break;
		default:
			break;
	
	}
}


void bt_data_cb(const uint8_t *buf_bytes, uint32_t len_bytes) {

	const int16_t * buf = (int16_t*) buf_bytes;
	const int32_t len = len_bytes / 2;
	int32_t acc = 0;
	int32_t average_sample = 0;

	for(int i = 0; i < len; ++i){
		acc += abs(buf[i]);
	}

	average_sample = acc / len;

	if(average_sample > g_average) {
		gpio_set_level(BT_AUDIO_LED, 1);
	} else {
		gpio_set_level(BT_AUDIO_LED, 0);
	}


	calculate_global_average(average_sample);
}

#define RETURN_ERR(str)                                 \
  if (err != ESP_OK) {                                  \
    printf("Error : %s\n", str);                        \
    printf("Description : %s\n", esp_err_to_name(err)); \
    return err;                                         \
  }

esp_err_t gpio_init() {
  esp_err_t err;

  err = gpio_reset_pin(BT_STATE_LED);
  RETURN_ERR("failed to reset gpio pin");
  err = gpio_reset_pin(BT_AUDIO_LED);
  RETURN_ERR("failed to reset gpio pin");

  err = gpio_set_direction(BT_STATE_LED, GPIO_MODE_OUTPUT);
  RETURN_ERR("failed to set gpio direction");
  err = gpio_set_direction(BT_AUDIO_LED, GPIO_MODE_OUTPUT);
  RETURN_ERR("failed to set gpio direction");

  return 0;
}

esp_err_t bt_init() {

  esp_err_t err;

  err = nvs_flash_init();
  RETURN_ERR("could not init nvs")

  esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  err = esp_bt_controller_init(&cfg);
  RETURN_ERR("could not init bluetooth controller")

  err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
  RETURN_ERR("could not enable bluetooth controller")

  err = esp_bluedroid_init();
  RETURN_ERR("could not init bluedroid")

  err = esp_bluedroid_enable();
  RETURN_ERR("could not enable bluedroid")

  err = esp_bt_gap_set_device_name("ESP-EQ");
  RETURN_ERR("could not set device name")

  err = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
  RETURN_ERR("could not make device connectable and discoverable")

  err = esp_a2d_sink_init();
  RETURN_ERR("could not init a2d sink")

  err = esp_a2d_register_callback(bt_a2dp_cb);
  RETURN_ERR("could not register callack")

  err = esp_a2d_sink_register_data_callback(bt_data_cb);
  RETURN_ERR("could not data register callack")

  return ESP_OK;
}



void app_main(void) {

  esp_err_t err;
  err = gpio_init();
  if (err)
	  abort();

  err = bt_init();
  if (err)
	  abort();

  printf("Bluetooth enabled!!! (^_^)");

  return;
}
