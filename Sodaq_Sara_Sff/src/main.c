#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <string.h>
#include <drivers/uart.h>

// The devicetree node identifier for the "sara1" alias.
#define SARA_ENABLE_NODE DT_ALIAS(sara1)

#if DT_NODE_HAS_STATUS(SARA_ENABLE_NODE, okay)
#define SARA_ENABLE     DT_GPIO_LABEL(SARA_ENABLE_NODE, gpios)
#define PIN             DT_GPIO_PIN(SARA_ENABLE_NODE, gpios)
#define FLAGS           DT_GPIO_FLAGS(SARA_ENABLE_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: sara1 devicetree alias is not defined"
#define SARA_ENABLE	""
#define PIN		0
#define FLAGS	0
#endif


// The devicetree node identifier for the "sara1" alias.
#define SARA_R4XX_TOGGLE_NODE DT_ALIAS(sara2)

#if DT_NODE_HAS_STATUS(SARA_R4XX_TOGGLE_NODE, okay)
#define SARA_R4XX_TOGGLE 	DT_GPIO_LABEL(SARA_R4XX_TOGGLE_NODE, gpios)
#define PIN2             	DT_GPIO_PIN(SARA_R4XX_TOGGLE_NODE, gpios)
#define FLAGS2           	DT_GPIO_FLAGS(SARA_R4XX_TOGGLE_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: sara1 devicetree alias is not defined"
#define SARA_R4XX_TOGGLE	""
#define PIN2	0
#define FLAGS2	0
#endif


const struct uart_config uart_cfg = {
        .baudrate = 115200,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};


void main(void) {
    const struct device *dev;
    const struct device *dev2;
    int ret;

	uint8_t buffer[32];
    uint8_t message[] = "AT+CGMR\r\n";

    dev = device_get_binding(SARA_ENABLE);
    if (dev == NULL) {
        return;
    }

	dev2 = device_get_binding(SARA_R4XX_TOGGLE);
    if (dev2 == NULL) {
        return;
    }
	
    ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_HIGH);
    if (ret < 0) {
        return;
    }

	ret = gpio_pin_configure(dev2, PIN2, GPIO_OUTPUT_LOW);
    if (ret < 0) {
        return;
    }

	k_msleep(4000);

	ret = gpio_pin_configure(dev2, PIN2, GPIO_INPUT);
    if (ret < 0) {
        return;
    }







	const char *label_sercom_5 = DT_LABEL(DT_ALIAS(sercom_5));
	const struct device *dev_sercom_5 = device_get_binding(label_sercom_5);

	const char *label_sercom_2 = DT_LABEL(DT_ALIAS(sercom_2));
	const struct device *dev_sercom_2 = device_get_binding(label_sercom_2);


    uart_configure(dev_sercom_5, &uart_cfg);
    uart_configure(dev_sercom_2, &uart_cfg);


    printk("PIN ET BROCHE INITIALISES\n");

	while (1) {

		//Ecriture sur le module sara_r412m
        for(int i=0; i < sizeof(message)-1; i++){
            uart_poll_out(dev_sercom_2, message[i]);
        }
		
        memset(buffer, 0, sizeof(buffer));  // Ajout de la ligne ici

        k_msleep(1500);
        //Reception des données
        int ret2 = uart_poll_in(dev_sercom_2, buffer);
	
	    if (ret2 == 0) {
    	    //printf("Response received: ");
        	for (int i = 0; i < sizeof(buffer)-1; i++) {
                uart_poll_out(dev_sercom_5, buffer[i]);
                //printk("I %d V %c", i, buffer[i]);
        	}
    	} else {
        	//printf("No response received\n");
    	}
        printk("\n");
        k_msleep(4000);
    }
}
