/*#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/i2c.h>


#include <zephyr/sys/printk.h>
#include <zephyr/console/console.h>*/

#include "functions.h"

// 1000 msec = 1 sec
#define SLEEP_TIME_MS 800

// The devicetree node identifier for the "led0" alias.
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)


static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);


void blink(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led0)) {
		return;
	}
	if (!gpio_is_ready_dt(&led1)) {
		return;
	}
	if (!gpio_is_ready_dt(&led2)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	
	while (1) {
		ret = gpio_pin_toggle_dt(&led0);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
		
		/*ret = gpio_pin_toggle_dt(&led1);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);*/
		
		ret = gpio_pin_toggle_dt(&led2);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
	}
}
