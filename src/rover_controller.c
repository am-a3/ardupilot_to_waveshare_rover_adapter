#include <stdbool.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rover_driver.h"
#include "rover_controller.h"
#include "pwm_input.h"

#define TAG                             "ROVER_CONTROLLER"

#define PWM_INPUT_TIMEOUT_MSEC          100

typedef enum {
    INPUT_OK = 0,
    INPUT_NOT_AVAILABLE
} rover_controller_state_t;

static QueueHandle_t* cmd_queue;
static QueueHandle_t* pwm_a_queue;
static QueueHandle_t* pwm_b_queue;

static rover_driver_pwm_data_t rover_driver_pwm_data;
static uint32_t pwm_a_last_update = 0;
static bool pwm_a_updated = false;
static uint32_t pwm_b_last_update = 0;
static bool pwm_b_updated = false;

static rover_controller_state_t rover_controller_state;

static void rover_controller_task(void *arg)
{
    pwm_data_t pwm_data_a;
    pwm_data_t pwm_data_b;

    if( xQueueReceive( *pwm_a_queue,
                        &( pwm_data_a ),
                        ( TickType_t ) 0 ) == pdPASS ) {
        if (pwm_data_a.state == PWM_INPUT_OK) {
            rover_driver_pwm_data.pwm_r_data = pwm_data_a.pulse_us;
            pwm_a_last_update = pwm_data_a.timestamp_ms;
            pwm_a_updated = true;
        }
    }

    if( xQueueReceive( *pwm_b_queue,
                        &( pwm_data_b ),
                        ( TickType_t ) 0 ) == pdPASS ) {
        if (pwm_data_b.state == PWM_INPUT_OK) {
            rover_driver_pwm_data.pwm_l_data = pwm_data_b.pulse_us;
            pwm_b_last_update = pwm_data_b.timestamp_ms;
            pwm_b_updated = true;
        }
    }

    switch (rover_controller_state) {
        case INPUT_OK:
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

            if (((now - pwm_a_last_update) > PWM_INPUT_TIMEOUT_MSEC) || ((now - pwm_b_last_update) > PWM_INPUT_TIMEOUT_MSEC)) {
                ESP_LOGI(TAG, "PWM input timeout");

                rover_driver_pwm_data.pwm_r_data = PWM_IDLE_VALUE;
                rover_driver_pwm_data.pwm_l_data = PWM_IDLE_VALUE;
                xQueueSend(*cmd_queue, &rover_driver_pwm_data, 0);

                pwm_a_updated = false;
                pwm_b_updated = false;
                rover_controller_state = INPUT_NOT_AVAILABLE;
            }

            if (pwm_a_updated && pwm_b_updated) {
                xQueueSend(*cmd_queue, &rover_driver_pwm_data, 0);
                pwm_a_updated = false;
                pwm_b_updated = false;
            }
            break;
        case INPUT_NOT_AVAILABLE:
            if (pwm_a_updated || pwm_b_updated) {
                ESP_LOGI(TAG, "PWM input ok");
                rover_controller_state = INPUT_OK;
            }
            break;
        default:
            rover_controller_state = INPUT_NOT_AVAILABLE;
            break;
    }
}

void rover_controller_init(QueueHandle_t* cmd_queue, QueueHandle_t* pwm_a_queue, QueueHandle_t* pwm_b_queue)
{
    cmd_queue = cmd_queue;
    pwm_a_queue = pwm_a_queue;
    pwm_b_queue = pwm_b_queue;

    xTaskCreate(rover_controller_task, "rover_controller_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "Rover controller init done");
}