#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "pwm_input.h"
#include "rover_driver.h"

static QueueHandle_t pwm_a_queue;
static QueueHandle_t pwm_b_queue;
static QueueHandle_t cmd_queue;

void app_main()
{
    pwm_a_queue = xQueueCreate( 10, sizeof( pwm_data_t ) );
    pwm_b_queue = xQueueCreate( 10, sizeof( pwm_data_t ) );
    cmd_queue = xQueueCreate( 10, sizeof( rover_driver_pwm_data_t ) );

    pwm_input_init(&pwm_a_queue, &pwm_a_queue);
    rover_driver_init(&cmd_queue);

    while (1) {

    }
}