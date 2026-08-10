#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define PRIORITY 5

static int sensor_value = 0;

static void producer_device_callback(struct k_timer* dummy);
static void device_work_handler(struct k_work* work);

/* Use semaphore to synchronize from interrupt via workqueue to thread. */
K_SEM_DEFINE(data_ready, 0, 1);

/* timer callback will be invoked in interrupt context. */
K_TIMER_DEFINE(producer_device, producer_device_callback, NULL);
/* Workqueue: Don't call printk in interrupt context. */
K_WORK_DEFINE(device_work, device_work_handler);

void device_work_handler(struct k_work* work) {
    printk("Producer: new value %d\n", sensor_value);
    /* wake consumer. */
    k_sem_give(&data_ready);
}

static void producer_device_callback(struct k_timer* dummy) {
    /* new sensor value. */
    sensor_value++;
    k_work_submit(&device_work);
}

int main(void) {
    k_timer_start(&producer_device, K_MSEC(1000), K_MSEC(1000));
    while (1) {
        /* wait for data from producer. */
        k_sem_take(&data_ready, K_FOREVER);

        printk("Consumer: consume value %d\n", sensor_value);
    }

    return 0;
}
