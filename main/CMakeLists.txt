idf_component_register(SRCS "main.c" "bt.c"
	PRIV_REQUIRES bt esp_driver_gpio nvs_flash
	INCLUDE_DIRS ".")
