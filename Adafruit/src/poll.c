// #include <zephyr.h>
// #include <device.h>
// #include <devicetree.h>
// #include <drivers/gpio.h>
// #include <drivers/uart.h>


// #include <string.h>

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
// #define PIN2	10
// #define FLAG GPIO_OUTPUT_ACTIVE
// #endif


// const struct uart_config uart_cfg = {
// 		.baudrate = 115200,
// 		.parity = UART_CFG_PARITY_NONE,
// 		.stop_bits = UART_CFG_STOP_BITS_1,
// 		.data_bits = UART_CFG_DATA_BITS_8,
// 		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
// };



// void main(void) {


//     int ret;

// 	// Activer broche 4 ET 10
// 	const struct device * saraenable_dev;
//     const struct device * saratoggle_dev;

// 	saraenable_dev = device_get_binding(SARAENABLE);
// 	if (saraenable_dev == NULL) {
//   		printk("Failed to get GPS device binding.\n");
// 	}
// 	ret = gpio_pin_configure(saraenable_dev, PIN, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
//     k_sleep(K_MSEC(1000));
// 	ret = gpio_pin_set(saraenable_dev, PIN, true);
// 	if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
//     k_sleep(K_MSEC(1000));


//     saratoggle_dev = device_get_binding(SARATOGGLE);
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

//     k_sleep(K_MSEC(2000));





//     const struct device *uart_dev1 = DEVICE_DT_GET(DT_NODELABEL(sercom5));
//     const struct device *uart_dev2 = DEVICE_DT_GET(DT_NODELABEL(sercom2));

//     if (!uart_dev1) {
//         printk("Cannot get UART device 1\n");
//     }

//     if (!uart_dev2) {
//         printk("Cannot get UART device 2\n");
//     }

//     ret = uart_configure(uart_dev1, &uart_cfg);
//     if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
    
//     ret = uart_configure(uart_dev2, &uart_cfg);
//     if (ret < 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
//     k_sleep(K_MSEC(2000));

    



//     const uint8_t msg [] = "AT+CGMR\r\n";
//     //const uint8_t msg [] = {0x41, 0x54, 0x2B, 0x0D, 0x0A};

//     for (int i = 0; i < sizeof(msg); i++) {
//         uart_poll_out(uart_dev1, msg[i]);
//         k_sleep(K_MSEC(500));
//     }



//     uint8_t buf[20];
//     memset(buf, 0, sizeof(buf));
//     k_sleep(K_MSEC(500));


        
        


//     for(int i=0; i<20; i++){

//         ret = uart_poll_in(uart_dev1, &buf[i]);

//         if (ret < 0) {
//             printk("Failed to send data over UART: %d\n", ret);
//         }else {
//                printk("%c", buf[i]);
//         }
        
//     }
//         k_sleep(K_MSEC(500));




    

    
//     for(int i=0; i<20; i++){

//         printk("%c", buf[i]);  
//     }

// }
