#ifndef IR_NEC_TRANSCEIVER_H
#define IR_NEC_TRANSCEIVER_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "ir_nec_encoder.h"

#include "espnow_basic_config.h"
#include "esp_now_custom.h"

#define EXAMPLE_IR_RESOLUTION_HZ     1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_NEC_DECODE_MARGIN 200     // Tolerance for parsing RMT symbols into bit stream


/**
 * @brief NEC timing spec
 */
#define NEC_LEADING_CODE_DURATION_0  9000
#define NEC_LEADING_CODE_DURATION_1  4500
#define NEC_PAYLOAD_ZERO_DURATION_0  560
#define NEC_PAYLOAD_ZERO_DURATION_1  560
#define NEC_PAYLOAD_ONE_DURATION_0   560
#define NEC_PAYLOAD_ONE_DURATION_1   1690
#define NEC_REPEAT_CODE_DURATION_0   9000
#define NEC_REPEAT_CODE_DURATION_1   2250

void init_ir_transceiver();
bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration);
bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols);
bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols);
bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols);
bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols);
void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num);
bool example_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data);

#endif //IR_NEC_TRANSCEIVER_H
