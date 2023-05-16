#include "command.h"



// /*const struct uart_config uart_cfg = {
// 		.baudrate = 115200,
// 		.parity = UART_CFG_PARITY_NONE,
// 		.stop_bits = UART_CFG_STOP_BITS_1,
// 		.data_bits = UART_CFG_DATA_BITS_8,
// 		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
// };*/


// #define SOC_NODE DT_NODELABEL(sercom2)

// #define name DT_PROP(DT_CHILD(SOC_NODE, sara_r4), label)



// void main(void) {

//     static uint8_t *poll_data;
//     static const uint8_t *end = "\r\n";

//     int ret;

// 	// Activer broche 4 ET 10

// 	ret = gpio_pin_configure(power_enable_sara.port, power_enable_sara.pin, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

// 	ret = gpio_pin_configure(change_voltage_sara.port, change_voltage_sara.pin, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
// 	ret = gpio_pin_set(change_voltage_sara.port, change_voltage_sara.pin, false);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

//     k_sleep(K_MSEC(500));
// 	ret = gpio_pin_configure(change_voltage_sara.port, change_voltage_sara.pin, GPIO_INPUT);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

// 	k_sleep(K_MSEC(1000));





//     /*saratoggle_dev = device_get_binding(SARATOGGLE);
// 	if (saratoggle_dev == NULL) {
//   		printk("Failed to get GPS device binding.\n");
// 	}
// 	ret = gpio_pin_configure(saratoggle_dev, PIN2, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
//     ret = gpio_pin_set(saratoggle_dev, PIN2, false);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

//     k_sleep(K_MSEC(1000));

//     ret = gpio_pin_configure(saratoggle_dev, PIN2, GPIO_INPUT);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

//     k_sleep(K_MSEC(500));*/



// 	printk("Name : %s\n", name);

// 	printk("%d\n", test_uart(uart_modem_sara));
// 	printk("%d\n", test_uart(uart_debug));
    
    
//     console_getline_init();

// 	while (1) {
//         printk("-----------------------------------------------------------------------\n");
//         printk("Enter AT command : \n");

//         printk("command : ");
// 		poll_data = console_getline();


//         bool findCMD = false;
//         for(int i=0; i<sizeof(at_cmd_map); i++){
//             if(strcmp(poll_data, at_cmd_map[i]) == 0) findCMD=1;
//         }


//         if(findCMD){

//             strcat(poll_data, end);

//             if(test_poll_out(poll_data))    printk("Commande envoyée.\n");
//             if(test_poll_in())				printk("\nFin de la réponse.\n");
            
//         }
//         else{
//             printk("Mauvaise commande ou commande non reconnu\n");
//         }
// 	}
// }




static int test_poll_in(void)
{
    uint8_t recv_char = 'N';
	uint8_t recv_char2 = 'N';

	//printk("Début réception : \n");
	/* Verify uart_poll_in() */
	while ((recv_char != 'K') && (recv_char2 != 'O')) {
        recv_char2 = recv_char;
		while (uart_poll_in(uart_modem_sara, &recv_char) < 0) {
			/* Allow other thread/workqueue to work. */
			k_yield();
		}
        uart_poll_out(uart_debug, recv_char);
	}
    //printk("\nFin réception.\n");

	return 1;
}


static int test_poll_out(uint8_t *poll_data)
{
	int i;

	/* Verify uart_poll_out() */
	for (i = 0; i < strlen(poll_data); i++) {
		uart_poll_out(uart_modem_sara, poll_data[i]);
	}

	return 1;
}





static int test_device(const struct device *uart_dev)
{

    if (!uart_dev) {
        printk("Cannot get UART device 1\n");
        return 0;
    }

    if(!device_is_ready(uart_dev)){
        printk("Device not ready\n");
        return 0;
    }
	return 1;
}