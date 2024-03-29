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
#include "driver/rmt.h"

#include "esp_now_custom.h"

#include "sdkconfig.h"

#include "espnow_basic_config.h"

#define OUT_PIN_SEL ((1ULL<<RF_PIN) | (1ULL<<RB_PIN) | (1ULL<<LF_PIN) | (1ULL<<LB_PIN) | (1ULL<<LASER_PIN))

static const char *TAG = "car";
rmt_item32_t items [8];


void car_hit(uint8_t car_shooting);

void ir_rx_task(void *arg) {
    uint8_t data;
    RingbufHandle_t rb = NULL;

    // Get the RMT RX ring buffer
    rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
    rmt_rx_start(RMT_RX_CHANNEL, 1);

    while (1) {
        size_t rx_size = 0;
        rmt_item32_t *received_items = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, portMAX_DELAY);
		ESP_LOGI(TAG, "items received");
        if (received_items) {
            int num_items = rx_size / sizeof(rmt_item32_t);
			ESP_LOGI(TAG, "NUM ITEMS: %d", num_items);

            data = 0;
			ESP_LOGI(TAG, "before for loop");
            for (int i = 0; i < 8 && i < num_items; ++i) {
                if (received_items[i].level0 == 0 && received_items[i].level1 == 1) {
					ESP_LOGI(TAG, "bit %d receiverd", i);
					ESP_LOGI(TAG, "DURATION 0 : %d", received_items[i].duration0);
					ESP_LOGI(TAG, "DURATION 1 : %d", received_items[i].duration1);
                    if (received_items[i].duration0 > 500 && received_items[i].duration0 > 1500 && received_items[i].duration1 < 1500) {
                        data |= (1 << i);
                    }
                }
            }
			ESP_LOGI(TAG, "after first for loop");
			ESP_LOGI(TAG, "data receiverd %x\n\n", data);
			switch(data){
				case 0xfc:	//Inky
					if (CAR_REMOTE_PAIR != INKY) car_hit(INKY);
					break;
				case 0xf9:	//BLINKY
					if (CAR_REMOTE_PAIR != BLINKY) car_hit(BLINKY);
					break;
				case 0xf3:	//PINKY
					if (CAR_REMOTE_PAIR != PINKY) car_hit(PINKY);
					break;
				case 0xe7:	//CLYDE
					if (CAR_REMOTE_PAIR != CLYDE) car_hit(CLYDE);
					break;
				default:
					break;
			}

            // Check if stop bit is valid
            //if (num_items >= 9 && received_items[8].level0 == 1 && received_items[8].level1 == 0 && received_items[8].duration0 > 1500 && received_items[8].duration1 > 500 && received_items[8].duration1 < 1500) {
            //    car_hit(data);
            //}

            // Free the memory allocated for received_items
            vRingbufferReturnItem(rb, (void *)received_items);
			ESP_LOGI(TAG, "after memory is freed");
        }
    }
}

static void IR_init(){
	//initialize IR receiving
	rmt_config_t rmt_tx_config = {
		.channel = RMT_TX_CHANNEL,
		.gpio_num = LASER_PIN,
		.clk_div = 150,
		.rmt_mode = RMT_MODE_TX,
		.tx_config = {
			.carrier_freq_hz = 38000,
			.carrier_duty_percent = 50,
			.carrier_level = RMT_CARRIER_LEVEL_HIGH,
			.carrier_en = true,
			.loop_en = false,
			.idle_level = RMT_IDLE_LEVEL_LOW,
			.idle_output_en = true,
		},
		.mem_block_num = 1,
	};

	rmt_config_t rmt_rx_config = {
		.channel = RMT_RX_CHANNEL,
		.gpio_num = IR_RX_PIN,
		.clk_div = 150,
		.rmt_mode = RMT_MODE_RX,
		.rx_config = {
			.carrier_freq_hz = 38000,
			.carrier_duty_percent = 50,
			.carrier_level = RMT_CARRIER_LEVEL_HIGH,
			.rm_carrier = true,
			.filter_ticks_thresh = 100,
			.filter_en = true,
			.idle_threshold = 12000,
		},
		.mem_block_num = 1,
	};
	
	ESP_LOGI(TAG, "init tx channel");
	ESP_ERROR_CHECK(rmt_config(&rmt_tx_config));
	ESP_ERROR_CHECK(rmt_driver_install(rmt_tx_config.channel, 0, 0));
	ESP_LOGI(TAG, "init rx channel");
	ESP_ERROR_CHECK(rmt_config(&rmt_rx_config));
	ESP_ERROR_CHECK(rmt_driver_install(rmt_rx_config.channel, 1000, 0));


	//start the RMT receiver
	rmt_rx_start(RMT_RX_CHANNEL, true);

	//create IR receive task
	xTaskCreate(ir_rx_task, "ir_rx_task", 2048, NULL, tskIDLE_PRIORITY, NULL);
	

	//create the message for shooting it later
	uint8_t data;
	switch(CAR_REMOTE_PAIR){
		case INKY:
			data = 2;
			break;
		case BLINKY:
			data = 4;
			break;
		case PINKY:
			data = 8;
			break;
		case CLYDE:
			data = 16;
			break;
		default:
			data = 255;
	}
	memset(items, 0, sizeof(items));

	for(int i=0; i<8; ++i){
		if(data & (1 << i)){
			items[i].duration0 = 2000;
			items[i].level0 = 0;
			items[i].duration1 = 1000;
			items[i].level1 = 1;
		} else {
			items[i].duration0 = 1000;
			items[i].level0 = 0;
			items[i].duration1 = 2000;
			items[i].level1 = 1;
		}
	}
		//items[8].duration0 = 1000;
		//items[8].level0 = 0;
		//items[8].duration1 = 2000;
		//items[8].level1 = 1;

	for(int i=0; i<8; i++){
		ESP_LOGI(TAG, "BIT %d", i);
		ESP_LOGI(TAG, "duration 0: %d", items[i].duration0);
		ESP_LOGI(TAG, "duration 1: %d", items[i].duration1);
		ESP_LOGI(TAG, "");
	}
}

void car_hit(uint8_t car_shooting){ //file ????
	my_data_t data;
	data.message_type = HIT_REPORT;
	data.car_shooting = car_shooting;//TODO get id of car shooting
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

    
	//init espnow
    init_espnow_master();

	//init ir
	vTaskDelay(2000 /portTICK_PERIOD_MS);
	IR_init();


	ESP_LOGI(TAG, "before main loop");
	while(1){
		//gpio_set_level(RF_PIN, 0);
		//gpio_set_level(RB_PIN, 0);
		//gpio_set_level(LF_PIN, 0);
		//gpio_set_level(LB_PIN, 0);
		vTaskDelay(750 / portTICK_PERIOD_MS);	
	}

}
