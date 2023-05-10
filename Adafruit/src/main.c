// #include <zephyr.h>
// #include <device.h>
// #include <devicetree.h>
// #include <drivers/gpio.h>

// #include <string.h>
// #include <drivers/uart.h>
// #include <drivers/i2c.h>


// #include <stdio.h>

// #define GPS_ADR 0x42
// #define BUFFER_SIZE 256

// //The devicetree node identifier for the "led0" alias.
// #define GPS_NODE DT_ALIAS(gps)


// #if DT_NODE_HAS_STATUS(GPS_NODE, okay)
// #define GPS		DT_GPIO_LABEL(GPS_NODE, gpios)
// #define PIN	    DT_GPIO_PIN(GPS_NODE, gpios)
// #define FLAGS	DT_GPIO_FLAGS(GPS_NODE, gpios)
// #else
// //A build error here means your board isn't set up to blink an LED.
// #error "Unsupported board: led0 devicetree alias is not defined"
// #define GPS	""
// #define PIN	18
// #define FLAG GPIO_OUTPUT_ACTIVE
// #endif





// uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER;

// void main(void)
// {
// 	//Activer broche 18
// 	const struct device * gps_dev;
// 	int ret;

// 	gps_dev = device_get_binding(GPS);
// 	if (gps_dev == NULL) {
//   		printk("Failed to get GPS device binding.\n");
// 	}

// 	ret = gpio_pin_configure(gps_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}

// 	ret = gpio_pin_set(gps_dev, PIN, true);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}



//     k_sleep(K_MSEC(5000));


// 	const char * res4 = DT_PROP(DT_ALIAS(gps), label);
// 	printk("label ? %s\n", res4);

// 	// Ouvrir l'I2C
// 	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));


// 	if (i2c_dev == NULL) {
//   		printk("Failed to get I2C device binding.\n");
// 	}


// 	if(!device_is_ready(i2c_dev)){
// 		printk("I2C bus %s is not ready !\n\r", i2c_dev->name);
// 	}
// 	printk("I2C bus %s is ready !\n\r", i2c_dev->name);




// 	ret = i2c_configure(i2c_dev, i2c_cfg);
//     if (ret != 0) {
//         printk("Failed to configure I2C device : %d\n", ret);
//         return;
//     }
// 	printk("Configure I2C device : %d\n", ret);



// 	uint8_t gps_cmd[] = "$GPRMC,1*04\r\n"; // Commande pour demander une trame GPRMC
// 	struct i2c_msg msg[2];
// 	uint8_t rx_buf[128]; // Buffer pour stocker les données lues du GPS

// 	msg[0].flags = I2C_MSG_WRITE;
// 	msg[0].len = sizeof(gps_cmd);
// 	msg[0].buf = gps_cmd;

// 	msg[1].flags = I2C_MSG_READ | I2C_MSG_STOP;
// 	msg[1].len = sizeof(rx_buf);
// 	msg[1].buf = rx_buf;

// 	ret = i2c_transfer(i2c_dev, msg, 2, 0x42);
// 	if (ret == 0) {
//     	// Analyser les données reçues du GPS
// 		for (int i = 0; i < sizeof(gps_cmd); i++) {
//     		printk("Received byte %d: 0x%02x\n", i, gps_cmd[i]);
// 		}
// 	}else {
// 		 printk("Failed to configure I2C device : %d\n", ret);
// 	}

	




// }