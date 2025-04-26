idf_component_register(SRCS "main.c" "bt.c" "lcd.c"
	PRIV_REQUIRES bt esp_driver_gpio nvs_flash esp_driver_i2c esp_lcd
	INCLUDE_DIRS ".")
