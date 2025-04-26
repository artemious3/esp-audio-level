#include <esp_a2dp_api.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_err.h>
#include <esp_gap_bt_api.h>
#include <nvs_flash.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "bt.h"
#include "lcd.h"

#include "driver/gpio.h"
#include "sdkconfig.h"

static void gpio_init() {
  ESP_ERROR_CHECK(
      gpio_set_direction(CONFIG_GPIO_BT_STATE_LED, GPIO_MODE_OUTPUT));
  ESP_ERROR_CHECK(gpio_set_direction(CONFIG_GPIO_MUSIC_LED, GPIO_MODE_OUTPUT));
}


void app_main(void) {
  gpio_init();
  lcd_init();
  bt_init();

  printf("Initialzation performed (^_^)\n");
}
