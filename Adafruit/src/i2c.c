// // /*
// //  * Copyright (c) 2015 Intel Corporation
// //  *
// //  * SPDX-License-Identifier: Apache-2.0
// //  */

// // #include <errno.h>
// // #include <zephyr.h>
// // #include <sys/printk.h>
// // #include <device.h>
// // #include <drivers/i2c.h>
// // #include <drivers/gpio.h>



// // #define GPS_NODE DT_ALIAS(gps)


// // #if DT_NODE_HAS_STATUS(GPS_NODE, okay)
// // #define GPS		DT_GPIO_LABEL(GPS_NODE, gpios)
// // #define PIN	    DT_GPIO_PIN(GPS_NODE, gpios)
// // #define FLAGS	DT_GPIO_FLAGS(GPS_NODE, gpios)
// // #else
// // // A build error here means your board isn't set up to blink an LED.
// // #error "Unsupported board: led0 devicetree alias is not defined"
// // #define GPS	""
// // #define PIN	18
// // #define FLAG GPIO_OUTPUT_ACTIVE
// // #endif

// // /**
// //  * @file Sample app using the Fujitsu MB85RC256V FRAM through I2C.
// //  */

// // #define FRAM_I2C_ADDR 0x42

// // static int write_bytes(const struct device *i2c_dev, uint16_t addr,
// // 		       uint8_t *data, uint32_t num_bytes)
// // {
// // 	uint8_t wr_addr[2];
// // 	struct i2c_msg msgs[2];

// // 	/* FRAM address */
// // 	wr_addr[0] = (addr >> 8) & 0xFF;
// // 	wr_addr[1] = addr & 0xFF;

// // 	/* Setup I2C messages */

// // 	/* Send the address to write to */
// // 	msgs[0].buf = wr_addr;
// // 	msgs[0].len = 2U;
// // 	msgs[0].flags = I2C_MSG_WRITE;

// // 	/* Data to be written, and STOP after this. */
// // 	msgs[1].buf = data;
// // 	msgs[1].len = num_bytes;
// // 	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

// // 	return i2c_transfer(i2c_dev, &msgs[0], 2, FRAM_I2C_ADDR);
// // }

// // static int read_bytes(const struct device *i2c_dev, uint16_t addr,
// // 		      uint8_t *data, uint32_t num_bytes)
// // {
// // 	uint8_t wr_addr[2];
// // 	struct i2c_msg msgs[2];

// // 	/* Now try to read back from FRAM */

// // 	/* FRAM address */
// // 	wr_addr[0] = (addr >> 8) & 0xFF;
// // 	wr_addr[1] = addr & 0xFF;

// // 	/* Setup I2C messages */

// // 	/* Send the address to read from */
// // 	msgs[0].buf = wr_addr;
// // 	msgs[0].len = 2U;
// // 	msgs[0].flags = I2C_MSG_WRITE;

// // 	/* Read from device. STOP after this. */
// // 	msgs[1].buf = data;
// // 	msgs[1].len = num_bytes;
// // 	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

// // 	return i2c_transfer(i2c_dev, &msgs[0], 2, FRAM_I2C_ADDR);
// // }

// // void main(void)
// // {

// //     // Activer broche 18
// // 	const struct device * gps_dev;
// // 	int i, ret;
// //     gps_dev = device_get_binding(GPS);
// // 	if (gps_dev == NULL) {
// //   		printk("Failed to get GPS device binding.\n");
// // 	}
// //     printk("Get GPS device binding.\n");

// // 	ret = gpio_pin_configure(gps_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
// // 	if (ret != 0) {
// //   		printk("Failed to configure GPS device binding.\n");
// // 	}
// //     printk("Configure GPS device binding.\n");

// // 	ret = gpio_pin_set(gps_dev, PIN, true);
// // 	if (ret != 0) {
// //   		printk("Failed to set GPS device binding.\n");
// // 	}  	
// //     printk("Set GPS device binding.\n");


// //     k_sleep(K_MSEC(5000));


// // 	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));


// // 	uint8_t cmp_data[16];
// // 	uint8_t data[16];


// // 	if (!device_is_ready(i2c_dev)) {
// // 		printk("I2C: Device is not ready.\n");
// // 		return;
// // 	}
// //     printk("I2C: Device is ready.\n");

// // 	/* Do one-byte read/write */

// // 	data[0] = 0xAE;
// // 	ret = write_bytes(i2c_dev, 0x42, &data[0], 1);
// // 	if (ret) {
// // 		printk("Error writing to FRAM! error code (%d)\n", ret);
// // 		return;
// // 	} else {
// // 		printk("Wrote 0xAE to address 0x00.\n");
// // 	}

// // 	data[0] = 0x86;
// // 	ret = write_bytes(i2c_dev, 0x01, &data[0], 1);
// // 	if (ret) {
// // 		printk("Error writing to FRAM! error code (%d)\n", ret);
// // 		return;
// // 	} else {
// // 		printk("Wrote 0x86 to address 0x01.\n");
// // 	}

// // 	data[0] = 0x00;
// // 	ret = read_bytes(i2c_dev, 0x00, &data[0], 1);
// // 	if (ret) {
// // 		printk("Error reading from FRAM! error code (%d)\n", ret);
// // 		return;
// // 	} else {
// // 		printk("Read 0x%X from address 0x00.\n", data[0]);
// // 	}

// // 	data[1] = 0x00;
// // 	ret = read_bytes(i2c_dev, 0x01, &data[0], 1);
// // 	if (ret) {
// // 		printk("Error reading from FRAM! error code (%d)\n", ret);
// // 		return;
// // 	} else {
// // 		printk("Read 0x%X from address 0x01.\n", data[0]);
// // 	}

// // 	/* Do multi-byte read/write */

// // 	/* get some random data, and clear out data[] */
// // 	for (i = 0; i < sizeof(cmp_data); i++) {
// // 		cmp_data[i] = k_cycle_get_32() & 0xFF;
// // 		data[i] = 0x00;
// // 	}

// // 	/* write them to the FRAM */
// // 	ret = write_bytes(i2c_dev, 0x00, cmp_data, sizeof(cmp_data));
// // 	if (ret) {
// // 		printk("Error writing to FRAM! error code (%d)\n", ret);
// // 		return;
// // 	} else {
// // 		printk("Wrote %zu bytes to address 0x00.\n", sizeof(cmp_data));
// // 	}

// // 	ret = read_bytes(i2c_dev, 0x00, data, sizeof(data));
// // 	if (ret) {
// // 		printk("Error reading from FRAM! error code (%d)\n", ret);
// // 		return;
// // 	} else {
// // 		printk("Read %zu bytes from address 0x00.\n", sizeof(data));
// // 	}

// // 	ret = 0;
// // 	for (i = 0; i < sizeof(cmp_data); i++) {
// // 		/* uncomment below if you want to see all the bytes */
// // 		printk("0x%X ?= 0x%X\n", cmp_data[i], data[i]);
// // 		if (cmp_data[i] != data[i]) {
// // 			printk("Data comparison failed @ %d.\n", i);
// // 			ret = -EIO;
// // 		}
// // 	}
// // 	if (ret == 0) {
// // 		printk("Data comparison successful.\n");
// // 	}
// // }


// #include <zephyr.h>
// #include <device.h>
// #include <drivers/i2c.h>

// /* Adresse I2C du module GPS */
// #define GPS_I2C_ADDR 0x42

// /* Registres du module GPS */
// #define GPS_REG_LATITUDE 0x01
// #define GPS_REG_LONGITUDE 0x02
// #define GPS_REG_ALTITUDE 0x03

// void main(void)
// {
//     int ret;

//     /* Initialisation de l'interface I2C */
//     const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));

//     if (i2c_dev == NULL) {
//         printk("Erreur: impossible d'initialiser l'interface I2C\n");
//         return;
//     }

//     /* Lecture des données de latitude */
//     uint8_t buf[4];
//     ret = i2c_reg_read_byte(i2c_dev, GPS_I2C_ADDR, GPS_REG_LATITUDE, buf);
//     if (ret != 0) {
//         printk("Erreur: impossible de lire les données de latitude\n");
//         return;
//     }
//     float latitude = (float)(buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) / 1000000.0;

//     /* Lecture des données de longitude */
//     ret = i2c_reg_read_byte(i2c_dev, GPS_I2C_ADDR, GPS_REG_LONGITUDE, buf);
//     if (ret != 0) {
//         printk("Erreur: impossible de lire les données de longitude\n");
//         return;
//     }
//     float longitude = (float)(buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) / 1000000.0;

//     /* Lecture des données d'altitude */
//     ret = i2c_reg_read_byte(i2c_dev, GPS_I2C_ADDR, GPS_REG_ALTITUDE, buf);
//     if (ret != 0) {
//         printk("Erreur: impossible de lire les données d'altitude\n");
//         return;
//     }
//     float altitude = (float)(buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) / 100.0;

//     /* Affichage des données */
//     printk("Latitude: %f\n", latitude);
//     printk("Longitude: %f\n", longitude);
//     printk("Altitude: %f\n", altitude);
// }
