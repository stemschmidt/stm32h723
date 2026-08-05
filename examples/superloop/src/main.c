#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#define NUM_LEDS 3

static const struct device* leds = DEVICE_DT_GET(DT_ALIAS(leds));
// static const struct device* leds = DEVICE_DT_GET(DT_PATH(leds));
static const struct gpio_dt_spec user_button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

static bool run_perform_task = false;
static int current_period_ms = 500;
static int64_t last_button_press_event_ms;

static void on_button_press(int64_t current_ms) {
    printk("Button pressed!\n");
    last_button_press_event_ms = current_ms;
}

static void on_button_release(int64_t current_ms) {
    printk("Button released!\n");
    if (last_button_press_event_ms != 0 && current_ms > last_button_press_event_ms) {
        int64_t diff = current_ms - last_button_press_event_ms;
        if (diff > 50 && diff < 150) {
            if (current_period_ms >= 200) {
                current_period_ms -= 100;
                printk("set led change period to %d\n", current_period_ms);
            }
        } else if (diff > 200 && diff < 400) {
            if (current_period_ms <= 900) {
                current_period_ms += 100;
                printk("set led change period to %d\n", current_period_ms);
            }
        } else if (diff > 1500 && diff < 2500) {
            run_perform_task = !run_perform_task;
            printk("perform_task %s\n", run_perform_task ? "enabled" : "disabled");
        } else {
            printk("discard button press: %lld ms\n", diff);
        }
        last_button_press_event_ms = 0;
    }
}

static void get_button_state(int64_t current_ms) {
    static int64_t last_time = 0;
    static uint8_t button_state = 0;

    if (current_ms - last_time >= 10) {
        if (current_ms - last_time > 20) {
            printk("warning: button polling > 20 ms: %lld!\n", current_ms - last_time);
        }
        last_time = current_ms;

        button_state = (button_state << 1);
        /* check button state and debounce it. */
        if (gpio_pin_get_dt(&user_button)) {
            button_state |= 0x01;
        }

        switch (button_state & 0x0f) {
            case 0x07:
                on_button_press(current_ms);
                break;
            case 0x08:
                on_button_release(current_ms);
                break;
            default:
                break;
        }
    }
}

static void update_led(int64_t current_ms) {
    static int64_t last_time = 0;
    static int current = 0;

    if (current_ms - last_time >= current_period_ms) {
        //        printk("led delta: %lld\n", current_ms - last_time);
        last_time = current_ms;
        /* switch off all leds. */
        for (int i = 0; i < NUM_LEDS; i++) {
            led_off(leds, i);
        }

        /* switch on current led. */
        led_on(leds, current);

        /* update current led for next invocation. */
        current = (current + 1) % NUM_LEDS;
    }
}

static void perform_task(void) {
    uint8_t rand = 0;
    do {
        rand = sys_rand8_get() % 12;
    } while (rand == 0);
    k_sleep(K_MSEC(rand));
}

int main(void) {
    int64_t milliseconds = 0;

    if (!device_is_ready(leds)) {
        return -1;
    }

    if (!device_is_ready(user_button.port)) {
        return -1;
    }

    /* configure button as input. */
    gpio_pin_configure_dt(&user_button, GPIO_INPUT);

    while (1) {
        milliseconds = k_uptime_get();

        update_led(milliseconds);
        get_button_state(milliseconds);
        if (run_perform_task) {
            perform_task();
        }
    }

    return 0;
}
