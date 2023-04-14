#ifndef WEBSERVER_H
#define WEBSERVER_H
/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/*** FROM SIMPLE MAIN.C***/
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
/*** END FROM SIMPLE MAIN.C***/

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN


esp_err_t hello_get_handler(httpd_req_t *req);



esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);


httpd_handle_t start_webserver(void);


esp_err_t stop_webserver(httpd_handle_t server);

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);


void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);


void wifi_init_softap(void);

#endif //WEBSERVER_H
