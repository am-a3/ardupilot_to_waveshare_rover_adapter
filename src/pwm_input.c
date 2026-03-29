#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_rx.h"
#include "esp_log.h"

#include "hw_config.h"
#include "pwm_input.h"

#define TAG_A                           "PWM_A"
#define TAG_B                           "PWM_B"

#define MEASUREMENT_QUEUE_LEN           10

static QueueHandle_t* pwm_a_queue;
static QueueHandle_t* pwm_b_queue;

static rmt_channel_handle_t init_rmt_rx(int gpio)
{
    rmt_channel_handle_t chan = NULL;

    rmt_rx_channel_config_t config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1 µs resolution
        .mem_block_symbols = 64,
        .gpio_num = gpio,
    };

    ESP_ERROR_CHECK(rmt_new_rx_channel(&config, &chan));
    ESP_ERROR_CHECK(rmt_enable(chan));

    return chan;
}

static rmt_a_rx_task(void *arg)
{
    rmt_channel_handle_t chan = init_rmt_rx(PWM_A_INPUT);

    uint8_t buf[64];

    rmt_receive_config_t cfg = {
        .signal_range_min_ns = 500,
        .signal_range_max_ns = 3000000,
    };

    pwm_data_t data = {0};

    while (1) {
        if (rmt_receive(chan, buf, sizeof(buf), &cfg) == ESP_OK) {
            rmt_symbol_word_t *sym = (rmt_symbol_word_t *)buf;
            uint32_t pulse = sym[0].duration0;

            ESP_LOGI(TAG_A, "PWM A measurement done");

            if (pulse < PULSE_MIN_LENGTH_USEC || pulse > PULSE_MAX_LENGTH_USEC) {
                data.state = PWM_INPUT_OUT_OF_RANGE;
                data.pulse_us = 0;
            }
            else {
                data.state = PWM_INPUT_OK;
                data.pulse_us = pulse;
            }

            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            data.timestamp_ms = now;

            xQueueSend(*pwm_a_queue, &data, 0);
        }

        taskYIELD();
    }
}

static rmt_b_rx_task(void *arg)
{
    rmt_channel_handle_t chan = init_rmt_rx(PWM_B_INPUT);

    uint8_t buf[64];

    rmt_receive_config_t cfg = {
        .signal_range_min_ns = 500,
        .signal_range_max_ns = 3000000,
    };

    pwm_data_t data = {0};

    while (1) {
        if (rmt_receive(chan, buf, sizeof(buf), &cfg) == ESP_OK) {
            rmt_symbol_word_t *sym = (rmt_symbol_word_t *)buf;
            uint32_t pulse = sym[0].duration0;

            ESP_LOGI(TAG_B, "PWM B measurement done");
            
            if (pulse < PULSE_MIN_LENGTH_USEC || pulse > PULSE_MAX_LENGTH_USEC) {
                data.state = PWM_INPUT_OUT_OF_RANGE;
                data.pulse_us = 0;
            }
            else {
                data.state = PWM_INPUT_OK;
                data.pulse_us = pulse;
            }

            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            data.timestamp_ms = now;

            xQueueSend(*pwm_b_queue, &data, 0);
        }

        taskYIELD();
    }
}

void pwm_input_init(QueueHandle_t* queue_a, QueueHandle_t* queue_b)
{
    pwm_a_queue = queue_a;
    pwm_b_queue = queue_b;

    xTaskCreate(rmt_a_rx_task, "rmt_a_rx_task", 4096, NULL, 10, NULL);
    xTaskCreate(rmt_b_rx_task, "rmt_b_rx_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG_A, "PWM input init done");
}
