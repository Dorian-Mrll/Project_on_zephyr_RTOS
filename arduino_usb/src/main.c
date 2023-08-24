#include "functions.h"


void main(){

	const struct device *usb_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
uint32_t dtr = 0;

if (usb_enable(NULL)) {
    return;
}

	// Sleep to wait for a terminal connection.
k_sleep(K_MSEC(2500));
//uart_line_ctrl_get(usb_dev, 	UART_LINE_CTRL_DTR, &dtr);

	while (1) {
    printk("Hello from USB Serial...\n");
    k_sleep(K_MSEC(2000));
}
}
