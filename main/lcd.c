#include "esp_lcd_io_i2c.h"
#include <esp_log.h>
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c_master.h"
#include "esp_lcd_types.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include <lcd.h>
#include <bt.h>
#include <stdint.h>


#define MAX_DB (0.f)
#define MIN_DB (-30.f)

static esp_lcd_panel_io_handle_t g_lcd_io_handle;
static esp_lcd_panel_handle_t g_lcd_handle;

/*
 * Size of buffer, that can fill whole screen
 */
#define BUFFER_SIZE ((CONFIG_SSD1306_H_RES) * (CONFIG_SSD1306_V_RES) / 8)
static uint8_t g_ones_buf [BUFFER_SIZE];
static uint8_t g_zeroes_buf [BUFFER_SIZE];



#define LCD_DELAY (1000 / 60 / portTICK_PERIOD_MS)

float g_last_db_level = MIN_DB;


static void lcd_draw_task(void* p){
	for(;;){
		int rect_width;
		rect_width = CONFIG_SSD1306_H_RES * (g_last_db_level - MIN_DB) / (MAX_DB - MIN_DB);
		rect_width = (rect_width <= 0) ? 1 : rect_width;

		esp_lcd_panel_draw_bitmap(g_lcd_handle, rect_width+1, 0, CONFIG_SSD1306_H_RES, CONFIG_SSD1306_V_RES, &g_zeroes_buf);
		esp_lcd_panel_draw_bitmap(g_lcd_handle, 0, 0, rect_width, CONFIG_SSD1306_V_RES, &g_ones_buf);

		vTaskDelay(LCD_DELAY);
	}
	
}

void lcd_init() {

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
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

  esp_lcd_panel_handle_t panel_handle = NULL;
  esp_lcd_panel_dev_config_t panel_config = {
      .bits_per_pixel = 1,
      .reset_gpio_num = -1,
  };
  esp_lcd_panel_ssd1306_config_t ssd1306_config = {
      .height = CONFIG_SSD1306_V_RES,
  };
  panel_config.vendor_config = &ssd1306_config;
  ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  ESP_LOGI("lcd_init", "Succesfulli initiated LCD");


  g_lcd_handle = panel_handle;
  g_lcd_io_handle = io_handle;
  for(int i = 0; i < BUFFER_SIZE; i++){
	  g_zeroes_buf[i] = 0;
	  g_ones_buf[i] = (uint8_t)(-1);
  }

  ESP_LOGI("lcd_init", "Prepared buffers for LCD");

  TaskHandle_t task_handle;
  xTaskCreate(lcd_draw_task, "LCD draw", 2048, NULL, 5, &task_handle);
  ESP_LOGI("lcd_init", "Started `LCD DRAW` task");
}


