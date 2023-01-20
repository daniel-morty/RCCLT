#ifndef ESPNOW_BASIC_CONFIG_H
#define ESPNOW_BASIC_CONFIG_H

#include <inttypes.h>
#include <stdbool.h>

#define RF_PIN 9
#define RB_PIN 8
#define LF_PIN 7
#define LB_PIN 6
#define LASER_PIN 18

#define RF_BUT 9
#define RB_BUT 8
#define LF_BUT 7
#define LB_BUT 6
#define LASER_BUT 3

// Define the structure of your data
typedef struct __attribute__((packed)) {
	//direction to move the car
    bool rf;
    bool rb;
    bool lf;
    bool lb;
	bool shoot_laser;
} my_data_t;

// Destination MAC address
// The default address is the broadcast address, which will work out of the box, but the slave will assume every tx succeeds.
// Setting to the master's address will allow the slave to determine if sending succeeded or failed.
//   note: with default config, the master's WiFi driver will log this for you. eg. I (721) wifi:mode : sta (12:34:56:78:9a:bc)
#define MY_RECEIVER_MAC {0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88}

#define MY_ESPNOW_PMK "pmk1234567890123"
#define MY_ESPNOW_CHANNEL 1

// #define MY_ESPNOW_ENABLE_LONG_RANGE 1

#define MY_SLAVE_DEEP_SLEEP_TIME_MS 1000

#endif // ESPNOW_BASIC_CONFIG_H
