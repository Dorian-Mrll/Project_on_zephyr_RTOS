// #include <zephyr.h>
// #include <device.h>
// #include <drivers/uart.h>

// #include <string.h>

// /*void main(void)
// {
//     //int ret;

 

// 	const char *dev_name = DT_LABEL(DT_ALIAS(sercom_5));
// 	//const struct device *uart_dev = 
// 	device_get_binding(dev_name);


//    	//ret = uart_configure(uart_dev, &uart_cfg);
//     //if (ret) {
//         //printk("Impossible de configurer la liaison série (code d'erreur %d)\n", ret);
//     //}

	
// 	//without k_msleep the message will not be displayed correctly 
// 	//k_msleep(1);
    
//     printk("Port UART ouvert avec succès !\n");
	
// 	for(int i=65; i<91; i++){
// 		printk("%c", (char)i);
// 		k_msleep(100);
// 	}
//     printk("\nEND !\n");
// }*/





// static const char *poll_data = "AT\r\n";


// static int test_poll_in(void)
// {
// 	unsigned char recv_char;
// 	//const struct device *const uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

// 	const char *label_sercom_2 = DT_LABEL(DT_ALIAS(sercom_5));
//     const struct device *uart_dev = device_get_binding(label_sercom_2);

// 	if (!device_is_ready(uart_dev)) {
// 		printk("UART device not ready\n");
// 		return 0;
// 	}

// 	uart_configure(uart_dev, &uart_cfg);
// 	printk("Please send characters to serial console\n");

// 	/* Verify uart_poll_in() */
// 	while (1) {
// 		while (uart_poll_in(uart_dev, &recv_char) < 0) {
// 			/* Allow other thread/workqueue to work. */
// 			k_yield();
// 			//printk("uart_poll_in %d\n", uart_poll_in(uart_dev, &recv_char));
// 		}

// 		printk("%c", recv_char);

// 		if ((recv_char == '\n') || (recv_char == '\r')) {
// 			printk("\n");
// 			break;
// 		}
// 	}

// 	return 1;
// }

// static int test_poll_out(void)
// {
// 	int i;
// 	//const struct device *const uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

// 	const char *label_sercom_2 = DT_LABEL(DT_ALIAS(sercom_5));
//     const struct device *uart_dev = device_get_binding(label_sercom_2);

// 	if (!device_is_ready(uart_dev)) {
// 		printk("UART device not ready\n");
// 		return 0;
// 	}

// 	uart_configure(uart_dev, &uart_cfg);
// 	/* Verify uart_poll_out() */
// 	for (i = 0; i < strlen(poll_data); i++) {
// 		uart_poll_out(uart_dev, poll_data[i]);
// 	}

// 	return 1;
// }