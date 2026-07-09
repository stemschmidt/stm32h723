#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

const struct device* leds = DEVICE_DT_GET(DT_NODELABEL(leds));

int main(void) {
    bool led_state = true;

    if (!device_is_ready(leds)) {
        return -1;
    }

    while (1) {
        if (led_state == true) {
            led_on(leds, 0);
        } else {
            led_off(leds, 0);
        }
        led_state = !led_state;
        printf("LED state: %s\n", led_state ? "ON" : "OFF");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
