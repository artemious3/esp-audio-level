idf_component_register(SRCS "esp-eq.c"
	PRIV_REQUIRES bt esp_driver_gpio nvs_flash
	INCLUDE_DIRS ".")
