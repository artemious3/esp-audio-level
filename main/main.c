
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

#include "bt.h"

#include "driver/gpio.h"

#define RETURN_ERR(str)                                 \
  if (err != ESP_OK) {                                  \
    printf("Error : %s\n", str);                        \
    printf("Description : %s\n", esp_err_to_name(err)); \
    return err;                                         \
  }

static esp_err_t gpio_init() {
  esp_err_t err;

  err = gpio_reset_pin(CONFIG_GPIO_MUSIC_LED);
  RETURN_ERR("failed to reset gpio pin");
  err = gpio_reset_pin(CONFIG_GPIO_MUSIC_LED);
  RETURN_ERR("failed to reset gpio pin");

  err = gpio_set_direction(CONFIG_GPIO_BT_STATE_LED, GPIO_MODE_OUTPUT);
  RETURN_ERR("failed to set gpio direction");
  err = gpio_set_direction(CONFIG_GPIO_MUSIC_LED, GPIO_MODE_OUTPUT);
  RETURN_ERR("failed to set gpio direction");

  return 0;
}


static esp_err_t bt_init() {

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
