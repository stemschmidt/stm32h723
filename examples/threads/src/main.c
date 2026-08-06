#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#if defined(CONFIG_INPUT)
#include <zephyr/input/input.h>
#endif

#define NUM_LEDS 3

#define LED_STACK_SIZE 1024
#define LED_PRIORITY 5
#define BUTTON_STACK_SIZE 1024
#define BUTTON_PRIORITY 5

#if !defined(CONFIG_INPUT)
static K_THREAD_STACK_DEFINE(button_stack_area, BUTTON_STACK_SIZE);
static struct k_thread button_thread_data;
#endif

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
                current_period_ms -= 100;  // no mutex, not an atomic!
                printk("set led change period to %d\n", current_period_ms);
            }
        } else if (diff > 200 && diff < 400) {
            if (current_period_ms <= 900) {
                current_period_ms += 100;  // no mutex, not an atomic!
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

#if defined(CONFIG_INPUT)

static void key_press(struct input_event* evt, void* user_data) {
    if (evt->code == INPUT_KEY_0) {
        int64_t current_ms = k_uptime_get();
        if (evt->value == 1) {
            on_button_press(current_ms);
        } else {
            on_button_release(current_ms);
        }
    }
}

INPUT_CALLBACK_DEFINE(NULL, key_press, NULL);

#else  // #if defined(CONFIG_INPUT)

static void get_button_state(const struct gpio_dt_spec* user_button, int64_t current_ms) {
    static int64_t last_time = 0;
    static uint8_t button_state = 0;

    if (current_ms - last_time >= 10) {
        if (current_ms - last_time > 20) {
            printk("warning: button polling > 20 ms: %lld!\n", current_ms - last_time);
        }
        last_time = current_ms;

        button_state = (button_state << 1);
        /* check button state and debounce it. */
        if (gpio_pin_get_dt(user_button)) {
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

static void button_thread(void* arg1, void* arg2, void* arg3) {
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    const struct gpio_dt_spec user_button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
    if (!device_is_ready(user_button.port)) {
        printk("button is not ready! Terminate\n");
        return;
    }

    /* configure button as input. */
    gpio_pin_configure_dt(&user_button, GPIO_INPUT);

    int64_t milliseconds = 0;

    while (true) {
        milliseconds = k_uptime_get();
        get_button_state(&user_button, milliseconds);
        k_sleep(K_MSEC(10));
    }
}

#endif  // #if defined(CONFIG_INPUT)

static void update_led(const struct device* leds) {
    static int current = 0;

    /* switch off all leds. */
    for (int i = 0; i < NUM_LEDS; i++) {
        led_off(leds, i);
    }

    /* switch on current led. */
    led_on(leds, current);

    /* update current led for next invocation. */
    current = (current + 1) % NUM_LEDS;
}

static void led_thread(void* arg1, void* arg2, void* arg3) {
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    const struct device* leds = DEVICE_DT_GET(DT_PATH(leds));

    if (!device_is_ready(leds)) {
        printk("leds is not ready! Terminate\n");
        return;
    }

    while (true) {
        update_led(leds);
        k_sleep(K_MSEC(current_period_ms));  // no mutex, not an atomic!
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
#if !defined(CONFIG_INPUT)
    k_tid_t button_thread_id =
        k_thread_create(&button_thread_data, button_stack_area, K_THREAD_STACK_SIZEOF(button_stack_area), button_thread,
                        NULL, NULL, NULL, BUTTON_PRIORITY, 0, K_NO_WAIT);
#endif
    int64_t milliseconds = 0;

    while (1) {
        milliseconds = k_uptime_get();

        if (run_perform_task) {
            perform_task();
        }
    }

    return 0;
}

K_THREAD_DEFINE(led_id, LED_STACK_SIZE, led_thread, NULL, NULL, NULL, LED_PRIORITY, 0, 0);
