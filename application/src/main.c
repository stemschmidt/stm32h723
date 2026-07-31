#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>

LOG_MODULE_REGISTER(main);

#define USE_CUSTOM_ERROR_HANDLER 0
#define DEBUG_FATAL_ERROR_HANDLER 0

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

const struct device* leds = DEVICE_DT_GET(DT_NODELABEL(leds));
#if USE_CUSTOM_ERROR_HANDLER
static volatile unsigned int fatal_error_reason = 0;
static volatile unsigned int fatal_error_pc = 0;
static volatile unsigned int fatal_error_lr = 0;
/*
  pc in esf contains the address where the issue occured. Use
  "/opt/toolchains/zephyr-sdk-1.0.1/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-addr2line -e build/zephyr/zephyr.elf
  0x12345678" (address in pc) to find the violating instruction.

  parameter reason:
  zephyr/include/zephyr/fatal_types.h (OS errors)
  zephyr/include/zephyr/arch/arm/arch.h (architecture specific errors)
*/
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf* esf) {
    fatal_error_reason = reason;
    fatal_error_pc = esf->basic.pc;
    fatal_error_lr = esf->basic.lr;

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
#endif

#if DEBUG_FATAL_ERROR_HANDLER
#define STACK_OVERFLOW_ARRAY 4096
static void stack_overflow(void) {
    volatile uint8_t dummy[STACK_OVERFLOW_ARRAY];
    for (int i = 0; i < STACK_OVERFLOW_ARRAY; i++) {
        dummy[i] = i;
    }
}

static void invalid_read(void) { volatile uint32_t x = *(volatile uint32_t*)0xFFFFFFFF; }

static void invalid_write(void) { *(volatile uint32_t*)0xFFFFFFFF = 0; }
#endif

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
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
