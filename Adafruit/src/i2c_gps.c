// #include <zephyr.h>
// #include <device.h>
// #include <devicetree.h>

// #include <drivers/gpio.h>
// #include <drivers/uart.h>
// #include <drivers/i2c.h>


// #include <stdio.h>
// #include <string.h>


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


// #define GPS_ADR 0x42

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
// 	ret = gpio_pin_set(gps_dev, PIN, true);
// 	if (ret != 0) {
//   		printk("Failed to configure GPS device binding.\n");
// 	}
//     k_sleep(K_MSEC(1000));



//     const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));

//     if (!i2c_dev) {
//         printf("Impossible d'ouvrir le périphérique I2C.\n");
//         return;
//     }


//     ret = i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER);
//     if (ret < 0) {
//         printk("Error configuring I2C: %d\n", ret);
//         return;
//     }

    
//     k_sleep(K_MSEC(5000));


//     ret = i2c_write(i2c_dev, (uint8_t)0xFD, 1, GPS_ADR);
//     if (ret != 0) {
//         printk("Error writing to GPS: %d\n", ret);
//         return;
//     }








//     uint8_t buf_size;
//     ret = i2c_read(i2c_dev, &buf_size, 1, 0x42);
//     if (ret < 0) {
//         printk("Error reading GPS buffer size: %d\n", ret);
//     } else {
//         printk("GPS buffer size: %d\n", buf_size);
//     }

    

    

//     uint8_t buf[buf_size];

//     memset(buf, 0, buf_size);

//     k_sleep(K_MSEC(1000));


//     while (1) {
        

//         for(int i=0; i<buf_size; i++){

//             ret = i2c_read(i2c_dev, &buf[i], 1, 0x42);

//             if (ret < 0) {
//                 printk("Error reading GPS: %d\n", ret);
//             } else {
                
//                 printk("%c", buf[i]); // Format the first three bytes into a string
//             }
        
//         }
//         k_sleep(K_MSEC(1000));
//     }
// }