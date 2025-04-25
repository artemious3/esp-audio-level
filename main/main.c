
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
#include "driver/i2c_master.h"
#include "esp_lcd_io_i2c.h"

#include "sdkconfig.h"

#define RETURN_ERR(str)                                 \
  if (err != ESP_OK) {                                  \
    printf("Error : %s\n", str);                        \
    printf("Description : %s\n", esp_err_to_name(err)); \
    return err;                                         \
  }

static void gpio_init() {

  ESP_ERROR_CHECK(gpio_reset_pin(CONFIG_GPIO_MUSIC_LED));
  ESP_ERROR_CHECK(gpio_reset_pin(CONFIG_GPIO_MUSIC_LED));

  ESP_ERROR_CHECK(
      gpio_set_direction(CONFIG_GPIO_BT_STATE_LED, GPIO_MODE_OUTPUT));
  ESP_ERROR_CHECK(gpio_set_direction(CONFIG_GPIO_MUSIC_LED, GPIO_MODE_OUTPUT));

}

static void bt_init() {

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

static void lcd_init() {
  esp_err_t err;

  i2c_master_bus_handle_t i2c_bus = NULL;
  i2c_master_bus_config_t bus_cfg = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .i2c_port = 0,
      .sda_io_num = CONFIG_GPIO_I2C_SDA,
      .scl_io_num = CONFIG_GPIO_I2C_SCL,
      .flags.enable_internal_pullup = true,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &i2c_bus));

  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_i2c_config_t io_config = {.dev_addr = 0x3C,
                                             .scl_speed_hz = (400 * 1000),
                                             .control_phase_bytes = 1,
                                             .lcd_cmd_bits = 8,
                                             .lcd_param_bits = 8,
                                             .dc_bit_offset = 6};
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &i2c_bus));
}

void app_main(void) {

  gpio_init();
  bt_init();

  printf("Bluetooth enabled!!! (^_^)");


  return;
}
