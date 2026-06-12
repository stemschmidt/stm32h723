#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

LOG_MODULE_REGISTER(main);
/* The devicetree node identifier for the "led0" alias. */

int main(void) {
  int ret = 0;

  if (!device_is_ready(led.port)) {
    return -1;
  }

  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_LOW);
  if (ret < 0) {
    return -1;
  }

  while (1) {
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0) {
      return -1;
    }
    LOG_INF(".");
    k_msleep(500);
  }

  return 0;
}
