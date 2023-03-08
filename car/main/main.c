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

#include "ir_nec_transceiver.h"
#include "esp_now_custom.h"

#include "sdkconfig.h"

#include "espnow_basic_config.h"

#define OUT_PIN_SEL ((1ULL<<RF_PIN) | (1ULL<<RB_PIN) | (1ULL<<LF_PIN) | (1ULL<<LB_PIN) | (1ULL<<LASER_PIN))

static const char *TAG = "car";
bool shoot_laser;




const ir_nec_scan_code_t scan_code = { // this needs to be moved to the file that actually
										//shoots the laser (calls rmt_transmit)
	.address = 0x0440,
	.command = CAR_ID, //replace with id of car TODO
};


static void car_hit(){ //file ????
	my_data_t data;
	data.message_type = HIT_REPORT;
	data.car_shooting = 1;//TODO get id of car shooting
	data.car_shot = CAR_ID;

	//send it
	ESP_LOGI(TAG, "sending hit report");
	esp_err_t err = send_espnow_data(data);
	if(err != ESP_OK){
	ESP_LOGE(TAG, "error sending hit_report message");
	}
	return;
}




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

//from here to bottom is ir stuff. clean it up TODO
	rmt_channel_handle_t tx_channel = NULL;
	rmt_encoder_handle_t nec_encoder = NULL;
	// this example won't send NEC frames in a loop
	rmt_transmit_config_t transmit_config = {
		.loop_count = 0, // no loop
	};
	rmt_channel_handle_t rx_channel = NULL;
	// save the received RMT symbols
	rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a standard NEC frame
	rmt_rx_done_event_data_t rx_data;
	// the following timing requirement is based on NEC protocol
	rmt_receive_config_t receive_config = {
		.signal_range_min_ns = 1250,     // the shortest duration for NEC signal is 560us, 1250ns < 560us, valid signal won't be treated as noise
		.signal_range_max_ns = 12000000, // the longest duration for NEC signal is 9000us, 12000000ns > 9000us, the receive won't stop early
	};


    ESP_LOGI(TAG, "create RMT RX channel");
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = EXAMPLE_IR_RESOLUTION_HZ,
        .mem_block_symbols = 64, // amount of RMT symbols that the channel can store at a time
        .gpio_num = EXAMPLE_IR_RX_GPIO_NUM,
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    ESP_LOGI(TAG, "register RX done callback");
    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = example_rmt_rx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));


    ESP_LOGI(TAG, "create RMT TX channel");
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = EXAMPLE_IR_RESOLUTION_HZ,
        .mem_block_symbols = 64, // amount of RMT symbols that the channel can store at a time
        .trans_queue_depth = 4,  // number of transactions that allowed to pending in the background, this example won't queue multiple transactions, so queue depth > 1 is sufficient
        .gpio_num = EXAMPLE_IR_TX_GPIO_NUM,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    ESP_LOGI(TAG, "modulate carrier to TX channel");
    rmt_carrier_config_t carrier_cfg = {
        .duty_cycle = 0.33,
        .frequency_hz = 38000, // 38KHz
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));


    ESP_LOGI(TAG, "install IR NEC encoder");
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = EXAMPLE_IR_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));

    ESP_LOGI(TAG, "enable RMT TX and RX channels");
    ESP_ERROR_CHECK(rmt_enable(tx_channel));
    ESP_ERROR_CHECK(rmt_enable(rx_channel));

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
    if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS) {
        // parse the receive symbols and print the result
        example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);
    }

	ESP_LOGI(TAG, "before main loop");



	while(true){
		if(shoot_laser){
			shoot_laser = false;
			ESP_LOGI(TAG, "shooting the laser from main");
			ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));
			ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
			if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS) {
				// parse the receive symbols and print the result
				example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);
			}
		}
	}
}
