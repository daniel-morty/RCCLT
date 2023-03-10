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
#define EXAMPLE_IR_TX_GPIO_NUM       18
#define EXAMPLE_IR_RX_GPIO_NUM       3
#define PWM_PIN 19

#define RF_BUT 9
#define RB_BUT 8
#define LF_BUT 7
#define LB_BUT 6
#define LASER_BUT 3

//message types
#define CAR_COMMAND  1
#define HIT_REPORT   2
#define SCORE_UPDATE 3


typedef struct {
	int score;
	int life_points;
} score_t;

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
	uint8_t car_id;
} my_data_t;

#define SCORE_BOARD_MAC {0x58, 0xcf, 0x79, 0xe9, 0xc3, 0x5c}

#define PAIR_1_CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88}
#define PAIR_1_REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb0}
#define PAIR_1_REMOTE_MAC_ARR	(uint8_t[])PAIR_1_REMOTE_MAC

#define PAIR_2_CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88}//TODO UPDATE THIS
#define PAIR_2_REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb1}//TODO UPDATE THIS
#define PAIR_2_REMOTE_MAC_ARR	(uint8_t[])PAIR_2_REMOTE_MAC

#define PAIR_3_CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88}//TODO UPDATE THIS
#define PAIR_3_REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb2}//TODO UPDATE THIS
#define PAIR_3_REMOTE_MAC_ARR	(uint8_t[])PAIR_3_REMOTE_MAC

#define PAIR_4_CAR_MAC		{0xf4, 0x12, 0xfa, 0x1b, 0x84, 0x88}//TODO UPDATE THIS
#define PAIR_4_REMOTE_MAC	{0xf4, 0x12, 0xfa, 0x1b, 0x3e, 0xb3}//TODO UPDATE THIS
#define PAIR_4_REMOTE_MAC_ARR	(uint8_t[])PAIR_4_REMOTE_MAC

#if CAR_REMOTE_PAIR == 1
	#define CAR_MAC		PAIR_1_CAR_MAC	 
	#define REMOTE_MAC	PAIR_1_REMOTE_MAC
	#define CAR_ID 1
	#define REMOTE_ID 1

#elif CAR_REMOTE_PAIR == 2
	#define CAR_MAC		PAIR_2_CAR_MAC	 
	#define REMOTE_MAC	PAIR_2_REMOTE_MAC
	#define CAR_ID 2
	#define REMOTE_ID 2

#elif CAR_REMOTE_PAIR == 3
	#define CAR_MAC		PAIR_3_CAR_MAC	 
	#define REMOTE_MAC	PAIR_3_REMOTE_MAC
	#define CAR_ID 3
	#define REMOTE_ID 3

#elif CAR_REMOTE_PAIR == 4
	#define CAR_MAC		PAIR_4_CAR_MAC	 
	#define REMOTE_MAC	PAIR_4_REMOTE_MAC
	#define CAR_ID 4
	#define REMOTE_ID 4
#endif

#define MY_ESPNOW_PMK "pmk1234567890123"
#define MY_ESPNOW_CHANNEL 1

// #define MY_ESPNOW_ENABLE_LONG_RANGE 1

#define MY_SLAVE_DEEP_SLEEP_TIME_MS 1000

#define STARTING_LIFE_POINTS 9

#endif // ESPNOW_BASIC_CONFIG_H
