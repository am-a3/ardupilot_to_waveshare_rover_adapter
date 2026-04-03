#ifndef ROVER_DRIVER_H

#include "freertos/queue.h"

#define ROVER_DRIVER_H

void rover_controller_init(QueueHandle_t* cmd_queue, QueueHandle_t* pwm_a_queue, QueueHandle_t* pwm_b_queue);

#endif