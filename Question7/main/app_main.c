/*
 * Q7 – Serial RX debounce FSM (switch–case)
 * Author: <Your Name>, ID: <Your ID>
 *
 * Strategy:
 * - One valid input character (VALID_CHAR). All others invalid and ignored.
 * - A 10 ms system tick polls UART and maintains a 0..500 ms “window” counter.
 * - On each tick:
 *      If a char arrived:
 *          - If it equals VALID_CHAR AND window >= 500 ms: toggle state and reset window.
 *          - Else: ignore (invalid or within window) and reset window.
 *      If no char arrived: increase window up to 500 ms (saturate).
 * - LED reflects state (ON → LED on, OFF → LED off). GPIO2 is active-LOW on common ESP8266 boards.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

/* ======== Config ======== */
#define VALID_CHAR        'A'
#define DEBOUNCE_MS       500U
#define TICK_MS           10U

/* UART0 (USB-serial) */
#define UART_PORT         UART_NUM_0
#define UART_BAUD         115200
#define UART_RX_BUF       256
#define UART_TX_BUF       0

/* NodeMCU blue LED on GPIO2, active-LOW */
#define LED_GPIO          2
#define LED_ON_LEVEL      0
#define LED_OFF_LEVEL     1

static const char *TAG = "q7_fsm";

/* ======== FSM ======== */
typedef enum { ST_OFF = 0, ST_ON } fsm_state_t;

static inline void led_apply(fsm_state_t s) {
    gpio_set_level(LED_GPIO, (s == ST_ON) ? LED_ON_LEVEL : LED_OFF_LEVEL);
}

static void uart_init(void) {
    uart_config_t cfg = {
        .baud_rate  = UART_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT, &cfg);
    uart_driver_install(UART_PORT, UART_RX_BUF, UART_TX_BUF, 0, NULL, 0);
}

static void led_init(void) {
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
    led_apply(ST_OFF);
}

/* ======== App Task: 10 ms tick ======== */
static void fsm_task(void *arg) {
    (void)arg;
    fsm_state_t state = ST_OFF;
    uint32_t window_ms = DEBOUNCE_MS;   // start “ready”

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(TICK_MS));

        if (window_ms < DEBOUNCE_MS) {
            window_ms += TICK_MS;
            if (window_ms > DEBOUNCE_MS) window_ms = DEBOUNCE_MS;
        }

        /* non-blocking RX */
        uint8_t ch;
        int n = uart_read_bytes(UART_PORT, &ch, 1, 0);
        if (n == 1) {
            bool is_valid = (ch == (uint8_t)VALID_CHAR);
            bool ready    = (window_ms >= DEBOUNCE_MS);

            if (is_valid && ready) {
                switch (state) {
                    case ST_OFF: state = ST_ON; break;
                    case ST_ON:  state = ST_OFF; break;
                    default:     state = ST_OFF; break;
                }
                led_apply(state);
                ESP_LOGI(TAG, "ACCEPT '%c' -> state=%s", ch, (state==ST_ON)?"ON":"OFF");
                window_ms = 0;
            } else {
                if (!is_valid) {
                    ESP_LOGI(TAG, "IGNORED invalid '%c'", ch);
                } else {
                    ESP_LOGI(TAG, "IGNORED '%c' within %u ms (t=%u)",
                             ch, DEBOUNCE_MS, window_ms);
                }
                window_ms = 0;
            }
        }

        /* heartbeat each second */
        static uint32_t hb = 0;
        hb += TICK_MS;
        if (hb >= 1000) {
            hb = 0;
            ESP_LOGI(TAG, "tick; state=%s, window=%u/%u",
                     (state==ST_ON)?"ON":"OFF", window_ms, DEBOUNCE_MS);
        }
    }
}

void app_main(void) {
    esp_log_level_set("*", ESP_LOG_INFO);  // ensure INFO is printed
    uart_init();
    led_init();
    ESP_LOGI(TAG, "Q7 FSM start (valid='%c', debounce=%u ms, tick=%u ms)",
             VALID_CHAR, DEBOUNCE_MS, TICK_MS);
    xTaskCreate(fsm_task, "fsm", 3072, NULL, tskIDLE_PRIORITY + 2, NULL);
}
