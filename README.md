# ESPNOW Basic Example

This is a very basic example to get up and running with ESP-NOW using ESP-IDF. While the included example in the IDF is comprehensive and secure, this example should allow for faster understanding and testing.

The slave will simply send a random value and the state of GPIO 0 (typically attached to a button named '0', 'BOOT' or 'PROG' on most dev boards) to the broadcast address. The master will receive and log it.

To customise the payload, simply change the struct in espnow_basic_config.h, and edit my_slave.c and my_master.c to populate and parse respectively.

See espnow_basic_config.h for more.


============================================================
mac of remote/car pair 1
remote	f4:12:fa:1b:3e:b0
car:	f4:12:fa:1b:84:88
