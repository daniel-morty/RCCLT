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

static const char *TAG = "car";


static esp_err_t send_espnow_data(my_data_t data);

typedef struct {
    uint8_t sender_mac_addr[ESP_NOW_ETH_ALEN];
    my_data_t data;
} recv_packet_t;


#define MY_ESPNOW_WIFI_MODE WIFI_MODE_STA
#define MY_ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
// #define MY_ESPNOW_WIFI_MODE WIFI_MODE_AP
// #define MY_ESPNOW_WIFI_IF   ESP_IF_WIFI_AP

static void car_hit(){
	//TODO send report to score_board
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

static void recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    
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
		gpio_set_level(RF_PIN, packet->rf);
		gpio_set_level(RB_PIN, packet->rb);
		gpio_set_level(LF_PIN, packet->lf);
		gpio_set_level(LB_PIN, packet->lb);
		gpio_set_level(LASER_PIN, packet->shoot_laser);
	}

	return;
}

static esp_err_t send_espnow_data(my_data_t data)
{
    const uint8_t destination_mac[] = SCORE_BOARD_MAC;


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
}
