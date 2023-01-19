#include "esp_system.h"
#include "esp_log.h"
#include "esp_random.h"
// This example needs rtc driver to read button state
#include "driver/rtc_io.h"

#include "espnow_basic_config.h"

static const char *TAG = "My_Slave";

// Your function to populate a my_data_t to send

void move_foreward(my_data_t *data){
    data->rf = 1;
    data->rb = 0;
    data->lf = 1;
    data->lb = 0;
}

void move_backward(my_data_t *data){
    data->rf = 0;
    data->rb = 1;
    data->lf = 0;
    data->lb = 1;
}

void move_turn_right(my_data_t *data){
    data->rf = 0;
    data->rb = 1;
    data->lf = 1;
    data->lb = 0;
}

void move_turn_left(my_data_t *data){
    data->rf = 1;
    data->rb = 0;
    data->lf = 0;
    data->lb = 1;
}
