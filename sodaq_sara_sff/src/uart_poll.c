#include "functions.h"

#define SOC_NODE DT_NODELABEL(sercom2)
#define name DT_PROP(DT_CHILD(SOC_NODE, sara_r4), label)

//uart
const struct device *uart_debug = DEVICE_DT_GET(DT_NODELABEL(sercom5));
const struct device *uart_modem_sara = DEVICE_DT_GET(DT_NODELABEL(sercom2));


int init_module_sara(void)
{
	// module SARA-R4XX
	const struct gpio_dt_spec power_enable_sara = GPIO_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(sercom2), sara_r4), mdm_power_gpios);
	const struct gpio_dt_spec change_voltage_sara = GPIO_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(sercom2), sara_r4), mdm_vint_gpios);

    static uint8_t *poll_data;
    static const uint8_t *end = "\r\n";

    int ret;

	// Activer broche 4 ET 10
	ret = gpio_pin_configure(power_enable_sara.port, power_enable_sara.pin, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
  		printk("Failed to configure SARA device binding.\n");
        return -1;
	}

	ret = gpio_pin_configure(change_voltage_sara.port, change_voltage_sara.pin, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
  		printk("Failed to configure SARA device binding.\n");
        return -1;
	}
	ret = gpio_pin_set(change_voltage_sara.port, change_voltage_sara.pin, false);
	if (ret < 0) {
  		printk("Failed to configure SARA device binding.\n");
        return -1;
	}

    k_sleep(K_MSEC(500));
	ret = gpio_pin_configure(change_voltage_sara.port, change_voltage_sara.pin, GPIO_INPUT);
	if (ret < 0) {
  		printk("Failed to configure SARA device binding.\n");
        return -1;
	}

	k_sleep(K_MSEC(1000));



	// verifie configruration device
	printk("Name : %s\n", name);

	if(!test_device(uart_modem_sara))    return -1;
	if(!test_device(uart_debug))         return -1;
    
    
    console_getline_init();

	while (1) {
        printk("-----------------------------------------------------------------------\n");
        printk("Enter AT command : \n");

        printk("command : ");
		poll_data = console_getline();


        bool findCMD = false;
        for(int i=0; i<sizeof(at_cmd_map); i++){
            if(strcmp(poll_data, at_cmd_map[i]) == 0) findCMD=1;
        }


        if(findCMD){

            strcat(poll_data, end);

            if(test_poll_out(poll_data))    printk("Commande envoyée.\n");
            if(test_poll_in())				printk("\nFin de la réponse.\n");
            
        }
        else{
            printk("Mauvaise commande ou commande non reconnu\n");
        }
	}
    return 1;
}


int test_poll_in(void)
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


int test_poll_out(uint8_t *poll_data)
{
	int i;

	/* Verify uart_poll_out() */
	for (i = 0; i < strlen(poll_data); i++) {
		uart_poll_out(uart_modem_sara, poll_data[i]);
	}
	return 1;
}




// vérifie si le noeud récupéré n'est pas NULL et si il est prêt a être utilisé
int test_device(const struct device *uart_dev)
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
