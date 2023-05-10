// #include <zephyr.h>
// #include <device.h>
// #include <devicetree.h>

// #include <drivers/gpio.h>
// #include <drivers/uart.h>
// #include <drivers/i2c.h>


// #include <stdio.h>
// #include <string.h>

// #define I2C_ADDR_MIN 0x08 // Adresse minimale à scanner
// #define I2C_ADDR_MAX 0x77 // Adresse maximale à scanner


// // The devicetree node identifier for the "led0" alias.
// #define GPS_NODE DT_ALIAS(gps)

// #if DT_NODE_HAS_STATUS(GPS_NODE, okay)
// #define GPS		DT_GPIO_LABEL(GPS_NODE, gpios)
// #define PIN	    DT_GPIO_PIN(GPS_NODE, gpios)
// #define FLAGS	DT_GPIO_FLAGS(GPS_NODE, gpios)
// #else
// // A build error here means your board isn't set up to blink an LED.
// #error "Unsupported board: led0 devicetree alias is not defined"
// #define GPS	""
// #define PIN	18
// #define FLAG GPIO_OUTPUT_ACTIVE
// #endif




// void main(void)
// {
//     int ret;

// 	// Activer broche 18
// 	const struct device * gps_dev;

// 	gps_dev = device_get_binding(GPS);
// 	if (gps_dev == NULL) {
//   		printk("Failed to get GPS device binding.\n");
// 	}
// 	ret = gpio_pin_configure(gps_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
// 	// ret = gpio_pin_set(gps_dev, PIN, true);
// 	// if (ret != 0) {
//   	// 	printk("Failed to configure GPS device binding.\n");
// 	// }
//     k_sleep(K_MSEC(500));



//     struct i2c_msg msg;
//     uint8_t dummy_byte = 0;
//     int i2c_addr;

// 	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));

//     if (!i2c_dev) {
//         printf("Impossible d'ouvrir le périphérique I2C.\n");
//         return;
//     }

//     printf("Scanning des adresses I2C...\n");

//     for (i2c_addr = I2C_ADDR_MIN; i2c_addr <= I2C_ADDR_MAX; i2c_addr++) {
//         msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;
//         msg.len = 1;
//         msg.buf = &dummy_byte;

//         ret = i2c_transfer(i2c_dev, &msg, 1, i2c_addr);
//         if (ret == 0) {
//             printf("Adresse I2C détectée: 0x%x\n", i2c_addr);
//         }
// 		//printf("ret: %d\n", ret);
//     }

//     printf("Terminé.\n");
// }