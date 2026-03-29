#ifndef ADAPTER_LOGIC_H

#define ADAPTER_LOGIC_H

#define ADAPTER_LOGIC_PWM_INPUT_TIMEOUT_MS                  100

#include "freertos/queue.h"

void adapter_logic_init(QueueHandle_t* pwm_a_queue, QueueHandle_t* pwm_b_queue);

#endif