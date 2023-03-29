#define CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY 1

#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"


#include "esp_now_custom.h"
#include "ir_nec_transceiver.h"

#include "sdkconfig.h"

#include "espnow_basic_config.h"

#define OUT_PIN_SEL ((1ULL<<RF_PIN) | (1ULL<<RB_PIN) | (1ULL<<LF_PIN) | (1ULL<<LB_PIN) | (1ULL<<LASER_PIN))

static const char *TAG = "car";
bool shoot_laser;

extern rmt_channel_handle_t tx_channel;
extern rmt_channel_handle_t rx_channel;
extern rmt_encoder_handle_t nec_encoder;
extern rmt_transmit_config_t transmit_config;
extern rmt_receive_config_t receive_config;
extern rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a standard NEC frame
extern rmt_rx_done_event_data_t rx_data;
extern QueueHandle_t receive_queue;


void app_main(void)
{

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that yo_want to set
    io_conf.pin_bit_mask = OUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    
    init_espnow_master();
	init_ir_transceiver();


	ESP_LOGI(TAG, "before main loop");



	//while(true){
	//	if(shoot_laser){
	//		shoot_laser = false;
	//		ESP_LOGI(TAG, "shooting the laser from main");
	//		ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));
	//		ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
	//		if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS) {
	//			// parse the receive symbols and print the result
	//			example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);
	//		}
	//	}
	//}
}
