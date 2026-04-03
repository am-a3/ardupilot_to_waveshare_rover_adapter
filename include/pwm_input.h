#ifndef PWM_INPUT_H

#include "freertos/queue.h"

#define PWM_INPUT_H

#define PWM_IDLE_VALUE                  1500
#define PULSE_SPAN_USEC                 600
#define PULSE_MIN_LENGTH_USEC           PWM_IDLE_VALUE - PULSE_SPAN_USEC
#define PULSE_MAX_LENGTH_USEC           PWM_IDLE_VALUE + PULSE_SPAN_USEC

typedef enum {
    PWM_INPUT_OK = 0,
    PWM_INPUT_OUT_OF_RANGE,
    PWM_INPUT_NOT_PRESENT
} pwm_input_state_t;

typedef struct pwm_data_t {
    pwm_input_state_t state;
    uint32_t pulse_us;
    uint32_t timestamp_ms;
} pwm_data_t;

void pwm_input_init(QueueHandle_t* queue_a, QueueHandle_t* queue_b);

#endif