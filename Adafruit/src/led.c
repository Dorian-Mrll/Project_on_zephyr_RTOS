#include "function.h"

// 1000 msec = 1 sec
#define SLEEP_TIME_MS 800

// The devicetree node identifier for the "led0" alias.
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)


#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN0	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS0	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN0    0
#define FLAGS	0
#endif


#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
#define LED1	DT_GPIO_LABEL(LED1_NODE, gpios)
#define PIN1	DT_GPIO_PIN(LED1_NODE, gpios)
#define FLAGS1	DT_GPIO_FLAGS(LED1_NODE, gpios)
#else
//	A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led1 devicetree alias is not defined"
#define LED1	""
#define PIN1	0
#define FLAGS1	0
#endif


#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
#define LED2	DT_GPIO_LABEL(LED2_NODE, gpios)
#define PIN2	DT_GPIO_PIN(LED2_NODE, gpios)
#define FLAGS2	DT_GPIO_FLAGS(LED2_NODE, gpios)
#else
//	A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led1 devicetree alias is not defined"
#define LED2	""
#define PIN2	0
#define FLAGS2	0
#endif


void blink(void)
{
	const struct device *dev, *dev1, *dev2;
	bool led_is_on = true;
	int ret;

    dev = device_get_binding(LED0);
	dev1 = device_get_binding(LED1);
	dev2 = device_get_binding(LED2);

    if(!test_device(dev)) return;
    if(!test_device(dev1)) return;
    if(!test_device(dev2)) return;

	ret = gpio_pin_configure(dev, PIN0, GPIO_OUTPUT_ACTIVE | FLAGS0);
	if (ret < 0) {
		return;
	}
    ret = gpio_pin_configure(dev1, PIN1, GPIO_OUTPUT_ACTIVE | FLAGS1);
	if (ret < 0) {
		return;
	}
    ret = gpio_pin_configure(dev2, PIN2, GPIO_OUTPUT_ACTIVE | FLAGS2);
	if (ret < 0) {
		return;
	}

	while (1) {
		gpio_pin_set(dev, PIN0, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
		gpio_pin_set(dev1, PIN1, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
		gpio_pin_set(dev2, PIN2, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
	}
}