#include "functions.h"

// 1000 msec = 1 sec
#define SLEEP_TIME_MS 800

// The devicetree node identifier for the "led0" alias.
#define LED0_NODE DT_ALIAS(led0)


static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


void blink(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led0)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	
	while (1) {
		ret = gpio_pin_toggle_dt(&led0);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
	}
}

