/* Basic ESPNOW Example - Slave

   This is a very basic example of sending data to the complementary Master example.
   By default, it sends an unencrypted data packet containing a random value and the state of a button to the broadcast mac address.

   To customise for your data:
    - Alter my_data_t in espnow_basic_config
    - Alter my_data_receive my_master.c
    - Alter my_data_populate my_slave.c

   For basic demonstration, do not edit this file
*/

#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_sleep.h"
#include "esp_random.h"
#include "driver/gpio.h"


#include "sdkconfig.h"

#include "espnow_basic_config.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define IN_PIN_SEL ((1ULL<<RF_BUT) | (1ULL<<RB_BUT) | (1ULL<<LF_BUT) | (1ULL<<LB_BUT) | (1ULL<<LASER_BUT))

SSD1306_t screen;

static const char *TAG = "Basic_Slave";

void my_data_populate(my_data_t *data);
void move_foreward(my_data_t *data);
void move_backward(my_data_t *data);
void move_turn_right(my_data_t *data);
void move_turn_left(my_data_t *data);

static EventGroupHandle_t s_evt_group;

#define MY_ESPNOW_WIFI_MODE WIFI_MODE_STA
#define MY_ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
// #define MY_ESPNOW_WIFI_MODE WIFI_MODE_AP
// #define MY_ESPNOW_WIFI_IF   ESP_IF_WIFI_AP

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

	if(packet->message_type != SCORE_UPDATE){
		ESP_LOGE(TAG, "wrong message_type recieved");
	} else{
		ESP_LOGD(TAG, "score has been updated. score: %d\tlife_points%d", packet->updated_score, packet->updated_life_points);

		char num_string[3] = {0};
		itoa(packet->updated_score, num_string, 10);
		//char score_string[] = {'s', 'c', 'o', 'r', 'e', ':', ' ',  packet->updated_score + 48};
		char score_string[] = {'s', 'c', 'o', 'r', 'e', ':', ' ',  num_string[0], num_string[1], num_string[2]};
		ssd1306_clear_screen(&screen, false);
		ssd1306_contrast(&screen, 0xff);
		ssd1306_display_text(&screen, 0, score_string, 10, false);

		itoa(packet->updated_life_points, num_string, 10);
		char life_points_string[] = {'l', 'i', 'f', 'e', ':', ' ',  num_string[0], num_string[1], num_string[2]};
		ssd1306_display_text(&screen, 1, life_points_string, 9, false);
	}


    //ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));
	return;
}

static void packet_sent_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (mac_addr == NULL) {
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }

    assert(status == ESP_NOW_SEND_SUCCESS || status == ESP_NOW_SEND_FAIL);
    xEventGroupSetBits(s_evt_group, BIT(status));
}

static void init_espnow_slave(void)
{
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_ERROR_CHECK( esp_netif_init() );
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(MY_ESPNOW_WIFI_MODE) );
    ESP_ERROR_CHECK( esp_wifi_start() );
#if MY_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK( esp_wifi_set_protocol(MY_ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
#endif
    ESP_ERROR_CHECK( esp_now_init() );
	ESP_LOGD(TAG,"attempting to register send_cb");
    ESP_ERROR_CHECK( esp_now_register_send_cb(packet_sent_cb) );
	ESP_LOGD(TAG,"send_cb registered, attempting to regiester recv_cb");
    ESP_ERROR_CHECK( esp_now_register_recv_cb(recv_cb) );
	ESP_LOGD(TAG, "registered!");
    ESP_ERROR_CHECK( esp_now_set_pmk((const uint8_t *)MY_ESPNOW_PMK) );

    // Alter this if you want to specify the gateway mac, enable encyption, etc
    const esp_now_peer_info_t broadcast_destination = {
        .peer_addr = CAR_MAC,
        .channel = MY_ESPNOW_CHANNEL,
        .ifidx = MY_ESPNOW_WIFI_IF
    };
    ESP_ERROR_CHECK( esp_now_add_peer(&broadcast_destination) );
}

static esp_err_t send_espnow_data(void)
{
    const uint8_t destination_mac[] = CAR_MAC;
    static my_data_t data;

	//populate data
	data.message_type = CAR_COMMAND;
	data.rf = !(gpio_get_level(RF_BUT));
	data.rb = !(gpio_get_level(RB_BUT));
	data.lf = !(gpio_get_level(LF_BUT));
	data.lb = !(gpio_get_level(LB_BUT));
	data.shoot_laser = !(gpio_get_level(LASER_BUT));

    // Send it
    esp_err_t err = esp_now_send(destination_mac, (uint8_t*)&data, sizeof(data));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Send error (%d)", err);
        return ESP_FAIL;
    }


    //ESP_LOGI(TAG, "Sent!"); //because we're sending so many messages, just log the failures
    return ESP_OK;
}


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    //uint32_t gpio_num = (uint32_t) arg;
    //xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);

	send_espnow_data();
}

void app_main(void)
{
	// for some reason just having this makes it faster
	//!note I would prefer not to have it
    s_evt_group = xEventGroupCreate();
    assert(s_evt_group);

	//zero-initialize the config structure.
	gpio_config_t io_conf = {};
	//disable interrupt
	io_conf.intr_type = GPIO_INTR_DISABLE;
	//bit mask of the pins
	io_conf.pin_bit_mask = IN_PIN_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = 1;
    //interrupt on both edges
    io_conf.intr_type = GPIO_INTR_ANYEDGE;	
	//enable configurations
	gpio_config(&io_conf);

	//TODO change thses pins
    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(RF_BUT,    gpio_isr_handler, (void*) RF_BUT   );
    gpio_isr_handler_add(RB_BUT,    gpio_isr_handler, (void*) RB_BUT   );
    gpio_isr_handler_add(LF_BUT,    gpio_isr_handler, (void*) LF_BUT   );
    gpio_isr_handler_add(LB_BUT,    gpio_isr_handler, (void*) LB_BUT   );
    gpio_isr_handler_add(LASER_BUT, gpio_isr_handler, (void*) LASER_BUT);


    init_espnow_slave();
	ESP_LOGD(TAG, "esp initialization complete");
	
#if CONFIG_I2C_INTERFACE
	ESP_LOGI(TAG, "INTERFACE is i2c");
	ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&screen, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_FLIP
	screen._flip = true;
	ESP_LOGW(TAG, "Flip upside down");
#endif

#if CONFIG_SSD1306_128x64
	ESP_LOGI(TAG, "Panel is 128x64");
	ssd1306_init(&screen, 128, 64);
#endif // CONFIG_SSD1306_128x64

	//for testing screen
	ssd1306_clear_screen(&screen, false);
	ssd1306_contrast(&screen, 0xff);
	ssd1306_display_text(&screen, 0, "Hello", 5, false);
  	vTaskDelay(3000 / portTICK_PERIOD_MS);


//	while(1){
//		//send_espnow_data();
//		//vTaskDelay(10 / portTICK_PERIOD_MS);
//	}


}
