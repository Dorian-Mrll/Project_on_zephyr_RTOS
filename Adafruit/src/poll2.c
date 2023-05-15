#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>


#include <string.h>

#define SARAENABLE_NODE DT_ALIAS(saraenable)

#if DT_NODE_HAS_STATUS(SARAENABLE_NODE, okay)
#define SARAENABLE	DT_GPIO_LABEL(SARAENABLE_NODE, gpios)
#define PIN         DT_GPIO_PIN(SARAENABLE_NODE, gpios)
#define FLAGS	    DT_GPIO_FLAGS(SARAENABLE_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led0 devicetree alias is not defined"
#define SARAENABLE	""
#define PIN	4
#define FLAG GPIO_OUTPUT_ACTIVE
#endif


#define SARATOGGLE_NODE DT_ALIAS(saratoggle)

#if DT_NODE_HAS_STATUS(SARATOGGLE_NODE, okay)
#define SARATOGGLE  DT_GPIO_LABEL(SARATOGGLE_NODE, gpios)
#define PIN2	    DT_GPIO_PIN(SARATOGGLE_NODE, gpios)
#define FLAGS2	    DT_GPIO_FLAGS(SARATOGGLE_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led0 devicetree alias is not defined"
#define SARATOGGLE	""
#define PIN2	10
#define FLAG GPIO_OUTPUT_ACTIVE
#endif


/*const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};*/


static int test_poll_in(void);
static int test_poll_out(void);


void main(void) {


    int ret;

	// Activer broche 4 ET 10
	const struct device * saraenable_dev;
    const struct device * saratoggle_dev;

	saraenable_dev = device_get_binding(SARAENABLE);
	if (saraenable_dev == NULL) {
  		printk("Failed to get GPS device binding.\n");
	}
	ret = gpio_pin_configure(saraenable_dev, PIN, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
  		printk("Failed to configure GPS device binding.\n");
	}
    k_sleep(K_MSEC(1000));
	ret = gpio_pin_set(saraenable_dev, PIN, true);
	if (ret < 0) {
  		printk("Failed to configure GPS device binding.\n");
	}
    k_sleep(K_MSEC(1000));


    saratoggle_dev = device_get_binding(SARATOGGLE);
	if (saratoggle_dev == NULL) {
  		printk("Failed to get GPS device binding.\n");
	}
	ret = gpio_pin_configure(saratoggle_dev, PIN2, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
  		printk("Failed to configure GPS device binding.\n");
	}
    ret = gpio_pin_set(saratoggle_dev, PIN2, false);
	if (ret < 0) {
  		printk("Failed to configure GPS device binding.\n");
	}

    k_sleep(K_MSEC(1000));

    ret = gpio_pin_configure(saratoggle_dev, PIN2, GPIO_INPUT);
	if (ret < 0) {
  		printk("Failed to configure GPS device binding.\n");
	}

    k_sleep(K_MSEC(2000));





    const struct device *uart_dev1 = DEVICE_DT_GET(DT_NODELABEL(sercom5));

    if (!uart_dev1) {
        printk("Cannot get UART device 1\n");
    }

    //ret = uart_configure(uart_dev1, &uart_cfg);
    if (ret < 0) {
  		printk("Failed to configure GPS device binding.\n");
	}
    

    k_sleep(K_MSEC(2000));





    int res = test_poll_out();
    //printk("test_poll_out : %d\n", res);
    int res2 = test_poll_in();
    //printk("test_poll_in %d\n", res2);

    /*if (res == 1 && res2 == 1)
        printk("OK\n");
    else
        printk("KO\n");*/
}





static const uint8_t *poll_data = "ATI\r\n";


static int test_poll_in(void)
{
	unsigned char recv_char;
    const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(sercom2));
    const struct device *uart_dev2 = DEVICE_DT_GET(DT_NODELABEL(sercom5));

	if (!device_is_ready(uart_dev)) {
		printk("UART device not ready\n");
		return 0;
	}

	//uart_configure(uart_dev, &uart_cfg);
    //uart_configure(uart_dev2, &uart_cfg);

	//printk("Please send characters to serial console\n");

	/* Verify uart_poll_in() */
	while (1) {
		while (uart_poll_in(uart_dev, &recv_char) < 0) {
			/* Allow other thread/workqueue to work. */
			k_yield();
		}

		//printk("%c", recv_char);
        uart_poll_out(uart_dev2, recv_char);

		/*if ((recv_char == '\n') || (recv_char == '\r')) {
			printk("\n");
			break;
		}*/
	}

	return 1;
}

static int test_poll_out(void)
{
	int i;
    const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(sercom2));

	if (!device_is_ready(uart_dev)) {
		printk("UART device not ready\n");
		return 0;
	}

	//uart_configure(uart_dev, &uart_cfg);
	/* Verify uart_poll_out() */
	for (i = 0; i < strlen(poll_data); i++) {
		uart_poll_out(uart_dev, poll_data[i]);
	}

	return 1;
}