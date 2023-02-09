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

#include "sdkconfig.h"

#include "espnow_basic_config.h"

#define OUT_PIN_SEL ((1ULL<<RF_PIN) | (1ULL<<RB_PIN) | (1ULL<<LF_PIN) | (1ULL<<LB_PIN) | (1ULL<<LASER_PIN))

static const char *TAG = "score_board";
bool shoot_laser;


static esp_err_t send_espnow_data(my_data_t data);

typedef struct {
    uint8_t sender_mac_addr[ESP_NOW_ETH_ALEN];
    my_data_t data;
} recv_packet_t;

//set up data structure for keeping score
score_t scores[4];

#define MY_ESPNOW_WIFI_MODE WIFI_MODE_STA
#define MY_ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
// #define MY_ESPNOW_WIFI_MODE WIFI_MODE_AP
// #define MY_ESPNOW_WIFI_IF   ESP_IF_WIFI_AP

void log_scores(){
	for(int i=0; i<4; i++){
		ESP_LOGI(TAG, "car %d\tscore: %d\tlife_points: %d", i, scores[i].score, scores[i].life_points);
	}
}

static void send_reports(uint8_t car_shot, uint8_t car_shooting){
	//send updates to the remotes of the cars
	my_data_t data;
	data.message_type = SCORE_UPDATE;
	ESP_LOGI(TAG, "senging reports to %d[shot] and %d[shooter]", car_shot, car_shooting);

	//send update to car that was shot
	scores[car_shot-1].score = scores[car_shot-1].score + 1;		//increment score
	data.updated_score = scores[car_shot-1].score;						//no change
	data.updated_life_points = scores[car_shot-1].life_points;
	data.car_id = car_shot;
	send_espnow_data(data);


	//send update to car that fired the shot
	data.updated_score = scores[car_shooting-1].score;						//no change
	scores[car_shooting-1].life_points = scores[car_shooting-1].life_points - 1; //decrement life points
	data.updated_life_points = scores[car_shooting-1].life_points;
	data.car_id = car_shooting;
	send_espnow_data(data);
	
	log_scores();
	
	return;
}

static void recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
	//TODO clean this up, log data
    
    if(len != sizeof(my_data_t))
    {
        ESP_LOGE(TAG, "Unexpected data length: %d != %u", len, sizeof(my_data_t));
        return;
    }

	//move the car accordingly
	my_data_t *packet = data; //!note this line generates a warning. it works fine though
								//because we checked the length above

	if(packet->message_type != CAR_COMMAND){
		ESP_LOGE(TAG, "wrong message_type recieved");
	} else{
		send_reports(packet->car_shot, packet->car_shooting); //update scores and foreward too remotes
	}

    //ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
	return;
}

static esp_err_t send_espnow_data(my_data_t data)
{
	//TODO log data

	//const uint8_t destination_mac[] = {0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb0};
	uint8_t destination_mac[6];
	switch(data.car_id){
		case 1:
			for(int i=0; i<6; i++){
				destination_mac[i] = PAIR_1_REMOTE_MAC_ARR[i];
			}
		break;
		case 2:
			for(int i=0; i<6; i++){
				destination_mac[i] = PAIR_2_REMOTE_MAC_ARR[i];
			}
		break;
		case 3:
			for(int i=0; i<6; i++){
				destination_mac[i] = PAIR_3_REMOTE_MAC_ARR[i];
			}
		break;
		case 4:
			for(int i=0; i<6; i++){
				destination_mac[i] = PAIR_4_REMOTE_MAC_ARR[i];
			}
		break;
	}

    // Send it
	esp_err_t err = esp_now_send(destination_mac, (uint8_t*)&data, sizeof(data));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Send error (%d)", err);
        return ESP_FAIL;
    }


    ESP_LOGI(TAG, "Sent!");
    return ESP_OK;
}

static void init_espnow_master(void)
{
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_ERROR_CHECK( esp_netif_init());
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(MY_ESPNOW_WIFI_MODE) );
    ESP_ERROR_CHECK( esp_wifi_start() );
#if MY_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK( esp_wifi_set_protocol(MY_ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
#endif
    ESP_ERROR_CHECK( esp_now_init() );
    ESP_ERROR_CHECK( esp_now_register_recv_cb(recv_cb) );
    ESP_ERROR_CHECK( esp_now_set_pmk((const uint8_t *)MY_ESPNOW_PMK) );

    const esp_now_peer_info_t broadcast_destination1 = {
        .peer_addr = PAIR_1_REMOTE_MAC,
        .channel = MY_ESPNOW_CHANNEL,
        .ifidx = MY_ESPNOW_WIFI_IF
    };
    ESP_ERROR_CHECK( esp_now_add_peer(&broadcast_destination1) );

    const esp_now_peer_info_t broadcast_destination2 = {
        .peer_addr = PAIR_2_REMOTE_MAC,
        .channel = MY_ESPNOW_CHANNEL,
        .ifidx = MY_ESPNOW_WIFI_IF
    };
    ESP_ERROR_CHECK( esp_now_add_peer(&broadcast_destination2) );

    const esp_now_peer_info_t broadcast_destination3 = {
        .peer_addr = PAIR_3_REMOTE_MAC,
        .channel = MY_ESPNOW_CHANNEL,
        .ifidx = MY_ESPNOW_WIFI_IF
    };
    ESP_ERROR_CHECK( esp_now_add_peer(&broadcast_destination3) );

    const esp_now_peer_info_t broadcast_destination4 = {
        .peer_addr = PAIR_4_REMOTE_MAC,
        .channel = MY_ESPNOW_CHANNEL,
        .ifidx = MY_ESPNOW_WIFI_IF
    };
    ESP_ERROR_CHECK( esp_now_add_peer(&broadcast_destination4) );
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
	

	//initialize scores
	for(int i=0; i<4; i++){
		scores[i].score = 0;
		scores[i].life_points = STARTING_LIFE_POINTS;
	}
	log_scores();
	send_reports(1,2); //for testing TODO remove this
}
