//  #include <zephyr.h>
//  #include <device.h>
//  #include <devicetree.h>
//  #include <drivers/gpio.h>
// #include <drivers/uart.h>

// #define SARAENABLE_NODE DT_ALIAS(saraenable)

// #if DT_NODE_HAS_STATUS(SARAENABLE_NODE, okay)
// #define SARAENABLE	DT_GPIO_LABEL(SARAENABLE_NODE, gpios)
// #define PIN         DT_GPIO_PIN(SARAENABLE_NODE, gpios)
// #define FLAGS	    DT_GPIO_FLAGS(SARAENABLE_NODE, gpios)
// #else
// // A build error here means your board isn't set up to blink an LED.
// #error "Unsupported board: led0 devicetree alias is not defined"
// #define SARAENABLE	""
// #define PIN	4
// #define FLAG GPIO_OUTPUT_ACTIVE
// #endif


// #define SARATOGGLE_NODE DT_ALIAS(saratoggle)

// #if DT_NODE_HAS_STATUS(SARATOGGLE_NODE, okay)
// #define SARATOGGLE  DT_GPIO_LABEL(SARATOGGLE_NODE, gpios)
// #define PIN2	    DT_GPIO_PIN(SARATOGGLE_NODE, gpios)
// #define FLAGS2	    DT_GPIO_FLAGS(SARATOGGLE_NODE, gpios)
// #else
// // A build error here means your board isn't set up to blink an LED.
// #error "Unsupported board: led0 devicetree alias is not defined"
// #define SARATOGGLE	""
// #define PIN	10
// #define FLAG GPIO_OUTPUT_ACTIVE
// #endif



// void main(void) {


//     int ret;

// 	// Activer broche 4 ET 10
// 	const struct device * saraenable_dev;
//     const struct device * saratoggle_dev;

// 	saraenable_dev = device_get_binding(SARAENABLE);
// 	if (saraenable_dev == NULL) {
//   		printk("Failed to get GPS device binding.\n");
// 	}
// 	ret = gpio_pin_configure(saraenable_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
// 	ret = gpio_pin_set(saraenable_dev, PIN, true);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}


//     saratoggle_dev = device_get_binding(SARATOGGLE);
// 	if (saratoggle_dev == NULL) {
//   		printk("Failed to get GPS device binding.\n");
// 	}
// 	ret = gpio_pin_configure(saratoggle_dev, PIN2, GPIO_OUTPUT_ACTIVE | FLAGS2);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
// 	ret = gpio_pin_set(saratoggle_dev, PIN2, false);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}



//     k_sleep(K_MSEC(2000));

//     ret = gpio_pin_configure(saratoggle_dev, PIN2, GPIO_INPUT | FLAGS2);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

//     k_sleep(K_MSEC(1000));









//     const char *dev_name = DT_LABEL(DT_ALIAS(sercom_5));
//     const char *dev_name2 = DT_LABEL(DT_ALIAS(sercom_2));

//     const struct device *uart_dev1;
//     const struct device *uart_dev2;




//     uart_dev1 = device_get_binding(dev_name);
//     if (!uart_dev1) {
//         printk("Cannot get UART device 1\n");
//     }

//     uart_dev2 = device_get_binding(dev_name2);
//     if (!uart_dev2) {
//         printk("Cannot get UART device 2\n");
//     }

    
//     char buffer[0];

//     const char *msg = "AT";

//     for (int i = 0; i < sizeof(msg); i++) {
//         uart_poll_out(uart_dev2, msg[i]);
//     }

//     while (1) {
        
//         //Reception des données
//         int ret = uart_poll_in(uart_dev2, buffer);
//         if (ret != 0) {
//             printk("Failed to send data over UART\n");
//         }
//         else{
//             printk("Received data: %c\n", buffer[0]);
//         }
//     }
// }
