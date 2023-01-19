#include "esp_log.h"

#include "espnow_basic_config.h"
#include "inttypes.h"
#include "driver/gpio.h"


static const char *TAG = "car";

// Your task to handle received my_data_t
//void my_data_receive(const uint8_t *sender_mac_addr, const my_data_t *data)
//{
//	//report the data
//	ESP_LOGD(TAG, "data recieved");
//	ESP_LOGD(TAG, "data is:");
//	ESP_LOGD(TAG, "\trf: %d", data->rf);
//	ESP_LOGD(TAG, "\trb: %d", data->rb);
//	ESP_LOGD(TAG, "\tlf: %d", data->lf);
//	ESP_LOGD(TAG, "\tlb: %d", data->lb);
//
//	//move the car accordingly
//	gpio_set_level(RF_PIN, data->rf);
//	gpio_set_level(RB_PIN, data->rb);
//	gpio_set_level(LF_PIN, data->lf);
//	gpio_set_level(LB_PIN, data->lb);
//	
//}
