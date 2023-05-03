// #include <zephyr.h>
// #include <device.h>
// #include <drivers/uart.h>
// #include <drivers/gpio.h>






// static volatile bool data_transmitted;
// static volatile bool data_received;
// static int char_sent;
// static const char fifo_data[] = "AT\r\n";


// #define DATA_SIZE	(sizeof(fifo_data) - 1)

// static void uart_fifo_callback(const struct device *dev, void *user_data)
// {
// 	uint8_t recvData;
// 	static int tx_data_idx;
// 	int ret;

// 	ARG_UNUSED(user_data);

// 	/* Verify uart_irq_update() */
// 	if (!uart_irq_update(dev)) {
// 		printk("retval should always be 1\n");
// 		return;
// 	}

// 	/* Verify uart_irq_tx_ready() */
// 	/* Note that TX IRQ may be disabled, but uart_irq_tx_ready() may
// 	 * still return true when ISR is called for another UART interrupt,
// 	 * hence additional check for i < DATA_SIZE.
// 	 */
// 	if (uart_irq_tx_ready(dev) && tx_data_idx < DATA_SIZE) {
// 		/* We arrive here by "tx ready" interrupt, so should always
// 		 * be able to put at least one byte into a FIFO. If not,
// 		 * well, we'll fail test.
// 		 */
// 		ret = uart_fifo_fill(dev, (uint8_t *)&fifo_data[tx_data_idx],
// 				     DATA_SIZE - char_sent);
// 		if (ret > 0) {
// 			data_transmitted = true;
// 			char_sent += ret;
// 			tx_data_idx += ret;
// 		} else {
// 			uart_irq_tx_disable(dev);
// 			return;
// 		}

// 		if (tx_data_idx == DATA_SIZE) {
// 			/* If we transmitted everything, stop IRQ stream,
// 			 * otherwise main app might never run.
// 			 */
// 			uart_irq_tx_disable(dev);
// 		}
// 	}

// 	/* Verify uart_irq_rx_ready() */
// 	if (uart_irq_rx_ready(dev)) {
// 		/* Verify uart_fifo_read() */
// 		uart_fifo_read(dev, &recvData, 1);
// 		printk("Data : %c", recvData);

// 		if ((recvData == '\n') || (recvData == '\r')) {
// 			data_received = true;
// 		}
// 	}
// }

// static int test_fifo_read(void)
// {
// 	//const struct device *const uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

//     const char *label_sercom_2 = DT_LABEL(DT_ALIAS(sercom_2));
//     const struct device *uart_dev = device_get_binding(label_sercom_2);

// 	if (!device_is_ready(uart_dev)) {
// 		printk("UART device not ready\n");
// 		return 0;
// 	}
    
// 	/* Verify uart_irq_callback_set() */
// 	uart_irq_callback_set(uart_dev, uart_fifo_callback);

// 	/* Enable Tx/Rx interrupt before using fifo */
// 	/* Verify uart_irq_rx_enable() */
// 	uart_irq_rx_enable(uart_dev);

// 	printk("Please send characters to serial console\n");

// 	data_received = false;
// 	while (data_received == false) {
// 		/* Allow other thread/workqueue to work. */
// 		k_yield();
// 	}

// 	/* Verify uart_irq_rx_disable() */
// 	uart_irq_rx_disable(uart_dev);

// 	return 1;
// }

// static int test_fifo_fill(void)
// {
// 	//const struct device *const uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

//     const char *label_sercom_2 = DT_LABEL(DT_ALIAS(sercom_2));
//     const struct device *uart_dev = device_get_binding(label_sercom_2);

// 	if (!device_is_ready(uart_dev)) {
// 		printk("UART device not ready\n");
// 		return 0;
// 	}

// 	char_sent = 0;

// 	/* Verify uart_irq_callback_set() */
// 	uart_irq_callback_set(uart_dev, uart_fifo_callback);

// 	/* Enable Tx/Rx interrupt before using fifo */
// 	/* Verify uart_irq_tx_enable() */
// 	uart_irq_tx_enable(uart_dev);

// 	k_sleep(K_MSEC(500));

// 	/* Verify uart_irq_tx_disable() */
// 	uart_irq_tx_disable(uart_dev);

// 	if (!data_transmitted) {
// 		return 1;
// 	}

// 	if (char_sent == DATA_SIZE) {
// 		return 1;
// 	} else {
// 		return 0;
// 	}

// }