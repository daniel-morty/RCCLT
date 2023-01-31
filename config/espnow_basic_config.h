#ifndef ESPNOW_BASIC_CONFIG_H
#define ESPNOW_BASIC_CONFIG_H

#include <inttypes.h>
#include <stdbool.h>

//change this value to compile for different care remote pairs
#define CAR_REMOTE_PAIR 1

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

//message types
#define CAR_COMMAND  1
#define HIT_REPORT   2
#define SCORE_UPDATE 3

// Define the structure of your data
typedef struct __attribute__((packed)) {
	//message type
	uint8_t message_type;

	//commands to the car
	//use if message type is CAR_COMMAND
    bool rf;
    bool rb;
    bool lf;
    bool lb;
	bool shoot_laser;

	//hit report 
	//use if mssage type is HIT_REPORT
	uint8_t car_shooting;
	uint8_t car_shot;

	//score update
	//use if message type is SCORE_UPDATE
	int updated_score;
	int updated_life_points;
} my_data_t;

#define SCORE_BOARD_MAC {0x58, 0xcf, 0x79, 0xe9, 0xc3, 0x5c}

#if CAR_REMOTE_PAIR == 1
	#define CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88}
	#define REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb0}
	#define CAR_ID 1
	#define REMOTE_ID 1

#elif CAR_REMOTE_PAIR == 2
	#define CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88} //TODO UPDATE THIS
	#define REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb0}//TODO UPDATE THIS
	#define CAR_ID 2
	#define REMOTE_ID 2

#elif CAR_REMOTE_PAIR == 3
	#define CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88} //TODO UPDATE THIS
	#define REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb0}//TODO UPDATE THIS
	#define CAR_ID 3
	#define REMOTE_ID 3

#elif CAR_REMOTE_PAIR == 4
	#define CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88} //TODO UPDATE THIS
	#define REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb0}//TODO UPDATE THIS
	#define CAR_ID 4
	#define REMOTE_ID 4
#endif

#define MY_ESPNOW_PMK "pmk1234567890123"
#define MY_ESPNOW_CHANNEL 1

// #define MY_ESPNOW_ENABLE_LONG_RANGE 1

#define MY_SLAVE_DEEP_SLEEP_TIME_MS 1000

#endif // ESPNOW_BASIC_CONFIG_H
