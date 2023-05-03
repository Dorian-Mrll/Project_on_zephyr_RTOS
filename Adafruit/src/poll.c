 /*#include <zephyr.h>
 #include <device.h>
 #include <devicetree.h>
 #include <drivers/gpio.h>


void main(void) {


    const char *dev_name = DT_LABEL(DT_ALIAS(sercom_5));
    const char *dev_name2 = DT_LABEL(DT_ALIAS(sercom_2));

    const struct device *uart_dev1;
    const struct device *uart_dev2;


    uint8_t buffer[64];


    uart_dev1 = device_get_binding(dev_name);
    if (!uart_dev1) {
        printk("Cannot get UART device 1\n");
    }

    uart_dev2 = device_get_binding(dev_name2);
    if (!uart_dev2) {
        printk("Cannot get UART device 2\n");
    }


    while (1) {
        
        const char *msg = "AT";

        for (int i = 0; i < strlen(msg); i++) {
            uart_poll_out(uart_dev2, msg[i]);
        }


        //Reception des données
        int ret = uart_poll_in(uart_dev2, buffer);
        if (ret != 0) {
            printk("Failed to send data over UART\n");
        }
        else{
            printk("Received data: %s\n", buffer);
        }
    }
}
*/