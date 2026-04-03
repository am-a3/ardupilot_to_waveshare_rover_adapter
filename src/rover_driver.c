#include <string.h>
#include <stdio.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rover_driver.h"
#include "hw_config.h"
#include "pwm_input.h"

#define TAG                             "ROVER_DRIVER"

#define PULSE_IDLE_DEADZONE_USEC        100
#define CMD_BUFFER_LENGTH               64

static QueueHandle_t* cmd_queue;

static rover_driver_pwm_data_t last_pwm_data;

static void rover_driver_send_speed_cmd(float r_data, float l_data)
{
    ESP_LOGI(TAG, "Speed command L: %.1f R: %.1f", l_data, r_data);

    char buffer[CMD_BUFFER_LENGTH];

    int len = snprintf(buffer, sizeof(buffer),
                       "{\"T\":1,\"L\":%.1f,\"R\":%.1f}\n",
                       l_data, r_data);

    if (len > 0) {
        uart_write_bytes(ROVER_UART_PORT, buffer, len);
    }
}



static float rover_driver_scale_pwm(uint32_t data)
{
    float result = (data - PWM_IDLE_VALUE);

    if (result != 0.0) {
        result = result / (2 * PULSE_SPAN_USEC);
    }

    return result;
}

static uint32_t rover_driver_check_idle_deadzone(uint32_t value)
{
    if ((value > (PWM_IDLE_VALUE - PULSE_IDLE_DEADZONE_USEC)) || (value < (PWM_IDLE_VALUE + PULSE_IDLE_DEADZONE_USEC))) {
        return PWM_IDLE_VALUE;
    }

    return value;
}

static void rover_driver_task(void *arg)
{
    rover_driver_pwm_data_t rover_driver_pwm_data;

    while(1) {
        if( xQueueReceive( *cmd_queue,
                        &( rover_driver_pwm_data ),
                        ( TickType_t ) 0 ) == pdPASS ) {
            //Check idle deadzone:
            rover_driver_pwm_data.pwm_l_data = rover_driver_check_idle_deadzone(rover_driver_pwm_data.pwm_l_data);
            rover_driver_pwm_data.pwm_r_data = rover_driver_check_idle_deadzone(rover_driver_pwm_data.pwm_r_data);

            if ((rover_driver_pwm_data.pwm_l_data != last_pwm_data.pwm_l_data)
                || (rover_driver_pwm_data.pwm_r_data != last_pwm_data.pwm_r_data)) {
                memcpy(&last_pwm_data, &rover_driver_pwm_data, sizeof(rover_driver_pwm_data_t));

                float l_wheel_speed = rover_driver_scale_pwm(rover_driver_pwm_data.pwm_l_data);
                float r_wheel_speed = rover_driver_scale_pwm(rover_driver_pwm_data.pwm_r_data);

                //send speed command to rover:
                rover_driver_send_speed_cmd(l_wheel_speed, r_wheel_speed);
            }
        }
    }
}

void rover_driver_init(QueueHandle_t* cmd_queue)
{
    cmd_queue = cmd_queue;

    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_driver_install(ROVER_UART_PORT, 1024, 0, 0, NULL, 0);
    uart_param_config(ROVER_UART_PORT, &uart_config);
    uart_set_pin(ROVER_UART_PORT, ROVER_UART_TX_PIN, ROVER_UART_RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreate(rover_driver_task, "rover_driver_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "Rover driver init done");
}