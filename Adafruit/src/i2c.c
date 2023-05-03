// #include <zephyr.h>
// #include <device.h>
// #include <devicetree.h>

// #include <string.h>
// #include <drivers/uart.h>

// #include <drivers/i2c.h>
// #include <kernel.h>

// #define I2C_MODE_CONTROLLER BIT(4)


// uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;



// void main(void){

//     unsigned char datas[6];
//     const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));
// 	//uint32_t i2c_cfg_tmp;

// 	if (i2c_dev == NULL || !device_is_ready(i2c_dev)) {
// 		printk("I2C device is not ready\n");
// 		return;
// 	}
// 	printk("I2C device is ready\n");

// 	/* 1. Verify i2c_configure() */
// 	if (i2c_configure(i2c_dev, i2c_cfg)) {
// 		printk("I2C config failed\n");
// 		return;
// 	}
//     printk("I2C config\n");


// 	// /* 2. Verify i2c_get_config() */
// 	// if (i2c_get_config(i2c_dev, &i2c_cfg_tmp)) {
// 	// 	printk("I2C get_config failed\n");
// 	// 	return 0;
// 	// }
// 	// if (i2c_cfg != i2c_cfg_tmp) {
// 	// 	printk("I2C get_config returned invalid config\n");
// 	// 	return 0;
// 	// }


// 	k_sleep(K_MSEC(1));

// 	datas[0] = 0xFD;
    
//     int ret =  i2c_write(i2c_dev, datas, 1, 0x42);
// 	if (ret != 0) {
// 		printk("Fail to write to sensor GY271 : %d\n", ret);
// 		return;
// 	}
//     printk("Write to sensor GY271 : %d\n", ret);

// 	(void)memset(datas, 0, sizeof(datas));

// 	/* 4. verify i2c_read() */
// 	if (i2c_read(i2c_dev, datas, 6, 0x42)) {
// 		printk("Fail to fetch sample from sensor GY271\n");
// 		return;
// 	}

// 	printk("axis raw data: %d %d %d %d %d %d\n",
// 				datas[0], datas[1], datas[2],
// 				datas[3], datas[4], datas[5]);

// }