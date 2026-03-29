#ifndef PWM_INPUT_H

#include "freertos/queue.h"

#define PWM_INPUT_H

#define PULSE_MIN_LENGTH_USEC           900
#define PULSE_MAX_LENGTH_USEC           2100

typedef struct pwm_data_t {
    pwm_input_state_t state;
    uint32_t pulse_us;
    uint32_t timestamp_ms;
} pwm_data_t;

typedef enum {
    PWM_INPUT_OK = 0,
    PWM_INPUT_OUT_OF_RANGE,
    PWM_INPUT_NOT_PRESENT
} pwm_input_state_t;

void pwm_input_init(QueueHandle_t* queue_a, QueueHandle_t* queue_b);

#endif