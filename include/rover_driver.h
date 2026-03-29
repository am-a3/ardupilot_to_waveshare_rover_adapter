#ifndef ROVER_DRIVER_H

#include <stdint.h>
#include "freertos/queue.h"

#define ROVER_DRIVER_H

typedef struct rover_driver_pwm_data_t {
    uint32_t pwm_l_data;
    uint32_t pwm_r_data;
} rover_driver_pwm_data_t;

void rover_driver_init(QueueHandle_t* cmd_queue);

#endif