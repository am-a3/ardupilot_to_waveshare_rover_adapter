#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "adapter_logic.h"
#include "pwm_input.h"
#include "rover_driver.h"

#define TAG                             "ADAPTER_LOGIC"

typedef struct adapter_logic_state_t {
    pwm_data_t pwm_a_data;
    pwm_data_t pwm_b_data;
    bool pwm_valid;
    bool pwm_a_updated;
    bool pwm_b_updated;
} adapter_logic_state_t;

static adapter_logic_state_t adapter_logic_state;

static QueueHandle_t* pwm_a_queue;
static QueueHandle_t* pwm_b_queue;

static adapter_logic_run(void)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    if ((now - adapter_logic_state.pwm_a_data.timestamp_ms) > ADAPTER_LOGIC_PWM_INPUT_TIMEOUT_MS) {
        //TODO: handle PWM A not present
    }

    if ((now - adapter_logic_state.pwm_b_data.timestamp_ms) > ADAPTER_LOGIC_PWM_INPUT_TIMEOUT_MS) {
        //TODO: handle PWM B not present
    }

    if (adapter_logic_state.pwm_a_updated || adapter_logic_state.pwm_a_updated) {
        //TODO: send data to rover
    }
}

static adapter_logic_task(void *arg)
{
    while (1) {
        pwm_data_t pwm_data_a;
        if( xQueueReceive( *pwm_a_queue,
                           &( pwm_data_a ),
                           ( TickType_t ) 0 ) == pdPASS )
        {   
            ESP_LOGI(TAG, "PWM A measurement received: %u usec", adapter_logic_state.pwm_a_data.pulse_us);
            memcpy(&adapter_logic_state.pwm_a_data, &pwm_data_a, sizeof(pwm_data_t));
            adapter_logic_state.pwm_a_updated = true;
        }

        pwm_data_t pwm_data_b;
        if( xQueueReceive( *pwm_b_queue,
                           &( pwm_data_b ),
                           ( TickType_t ) 0 ) == pdPASS )
        {
            ESP_LOGI(TAG, "PWM B measurement received: %u usec", adapter_logic_state.pwm_b_data.pulse_us);
            memcpy(&adapter_logic_state.pwm_b_data, &pwm_data_b, sizeof(pwm_data_t));
            adapter_logic_state.pwm_b_updated = true;
        }

        adapter_logic_run();
    }
}

void adapter_logic_init(QueueHandle_t* pwm_a_queue, QueueHandle_t* pwm_b_queue)
{
    pwm_a_queue = pwm_a_queue;
    pwm_b_queue = pwm_b_queue;

    xTaskCreate(adapter_logic_task, "adapter_logic_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "Adapter logic init done");
}