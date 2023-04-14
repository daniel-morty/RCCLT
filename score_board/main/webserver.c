#include "webserver.h"
#include "espnow_basic_config.h"

const char *TAG = "AP web server";
extern score_t scores[4];


/* An HTTP GET handler */
esp_err_t hello_get_handler(httpd_req_t *req)
{

	char web_string[2000];
	char * raw_str = "<head>"
				"<style>"
					".score {display: inline-block; width: 48%%; margin: 5px}"
					"h1 {margin-left: 15px}"
					"h2 {margin-left: 15px}"
				"</style>"
			"</head>"
			"<body>"
				"<div class=\"score\" style=\"background-color: #33fffb\">"
					"<h1>Inky\%</h1>"
					"<h2>score: %d</h2>"
					"<h2>life: %d</h2>"
				"</div>"
				"<div class=\"score\" style=\"background-color: #ff4833\">"
					"<h1>Blinky</h1>"
					"<h2>score: %d</h2>"
					"<h2>life: %d</h2>"
				"</div>"
				"<div class=\"score\" style=\"background-color: #fb85ba\">"
					"<h1>Pinky</h1>"
					"<h2>score: %d</h2>"
					"<h2>life: %d</h2>"
				"</div>"
				"<div class=\"score\" style=\"background-color: #f0832d\">"
					"<h1>Clyde</h1>"
					"<h2>score: %d</h2>"
					"<h2>life: %d</h2>"
				"</div>"
			"</body>"
			"</html>"
			"<script>"
				"setTimeout(()=>{location.reload();}, 100);"
				"function sendRequest() {"
					"var xhttp = new XMLHttpRequest();"
					"xhttp.open(\"GET\", \"/\", true);"
					"xhttp.send();"
				"}"
			"</script>";

	sprintf( web_string, raw_str, scores[INKY].score, scores[INKY].life_points,
									scores[BLINKY].score, scores[BLINKY].life_points,
									scores[PINKY].score, scores[PINKY].life_points,
									scores[CLYDE].score, scores[CLYDE].life_points);


    httpd_resp_send(req, web_string, HTTPD_RESP_USE_STRLEN);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    } else{
        ESP_LOGI(TAG, "Request headers not lost (they should be)");
	}
    return ESP_OK;
}

const httpd_uri_t hello = {
	.uri       = "/",
	.method    = HTTP_GET,
	.handler   = hello_get_handler,
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}
//end of from http simple main.c 

void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                    .required = false,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

