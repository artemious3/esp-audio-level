#include <stdint.h>
#include <esp_err.h>
#include <esp_a2dp_api.h>

void calculate_global_average(int32_t next_avg);

void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

void bt_data_cb(const uint8_t *buf_bytes, uint32_t len_bytes);
