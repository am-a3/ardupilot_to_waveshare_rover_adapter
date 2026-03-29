#include "freertos/queue.h"

static QueueHandle_t pwm_a_queue;
static QueueHandle_t pwm_b_queue;

void app_main()
{
    pwm_input_init(&pwm_a_queue, &pwm_a_queue);

    
}