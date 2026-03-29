#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rover_driver.h"
#include "hw_config.h"

#define TAG                             "ADAPTER_LOGIC"

#define PWM_IDLE_VALUE                  1500

static QueueHandle_t* cmd_queue;

static float rover_driver_scale_pwm(uint32_t data)
{
    int32_t result = PWM_IDLE_VALUE - data;


}

static rover_driver_task(void *arg)
{
    rover_driver_pwm_data_t rover_driver_pwm_data;

    if( xQueueReceive( *cmd_queue,
                    &( rover_driver_pwm_data ),
                    ( TickType_t ) 0 ) == pdPASS ) {
    
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

    xTaskCreate(rover_driver_task, "adapter_logic_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "Rover driver init done");
}