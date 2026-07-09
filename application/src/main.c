#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

static volatile unsigned int crash_reason = 0;
static volatile unsigned int violating_address = 0;

const struct device* leds = DEVICE_DT_GET(DT_NODELABEL(leds));

/*
  pc in esf contains the address where the issue occured. Use
  "/opt/toolchains/zephyr-sdk-1.0.1/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-addr2line -e build/zephyr/zephyr.elf
  0x12345678" (address in pc) to find the violating instruction.

  parameter reason:
  zephyr/include/zephyr/fatal_types.h (OS errors)
  zephyr/include/zephyr/arch/arm/arch.h (architecture specific errors)
*/
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf* esf) {
    crash_reason = reason;
    violating_address = esf->basic.pc;

/* If we have build with CONFIG_THREAD_ANALYZER to analyze stack usage, run into a breakpoint. */
#if defined(CONFIG_THREAD_ANALYZER)
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
        __asm__ volatile("bkpt #0");
    }

    while (1) {
        k_cpu_idle();
    }
#else
    sys_reboot(SYS_REBOOT_COLD);
#endif
}

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
