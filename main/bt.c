
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

#include "sdkconfig.h"
#include "bt.h"

/* average samples per last `NUM_CB_TO_COUNT_AVG` callbacks*/
static int32_t g_average_levels[CONFIG_NUM_CB_TO_CALCULATE_AVG];
static int32_t g_average;


void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param){
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


void calculate_global_average(int32_t next_avg) {
	for(int i = 1; i < CONFIG_NUM_CB_TO_CALCULATE_AVG; i++){
		g_average_levels[i-1] = g_average_levels[i];
	}
	g_average_levels[CONFIG_NUM_CB_TO_CALCULATE_AVG-1] = next_avg;

	int32_t acc = 0;
	for(int i = 0; i < CONFIG_NUM_CB_TO_CALCULATE_AVG; i++){
		acc += g_average_levels[i];
	}
	g_average = acc / CONFIG_NUM_CB_TO_CALCULATE_AVG;

}


static void update_music_led(int32_t avg_level){
	if(avg_level> g_average) {
		gpio_set_level(CONFIG_GPIO_MUSIC_LED, 1);
	} else {
		gpio_set_level(CONFIG_GPIO_MUSIC_LED, 0);
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

	update_music_led(average_sample);

	calculate_global_average(average_sample);
}




