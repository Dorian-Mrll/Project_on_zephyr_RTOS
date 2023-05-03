#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <string.h>
#include <drivers/uart.h>
#include <drivers/i2c.h>


#define GPS_ADR 0x42
#define BUFFER_SIZE 256

// The devicetree node identifier for the "led0" alias.
#define GPS_NODE DT_ALIAS(gps)


#if DT_NODE_HAS_STATUS(GPS_NODE, okay)
#define GPS		DT_GPIO_LABEL(GPS_NODE, gpios)
#define PIN	    DT_GPIO_PIN(GPS_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(GPS_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led0 devicetree alias is not defined"
#define GPS	""
#define PIN	18
#define FLAG GPIO_OUTPUT_ACTIVE
#endif


void main(void)
{

	// Activer broche 18
	const struct device * gps_dev;
	int ret;

	gps_dev = device_get_binding(GPS);
	if (gps_dev == NULL) {
  		printk("Failed to get GPS device binding.\n");
	}

	ret = gpio_pin_configure(gps_dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret != 0) {
  		printk("Failed to configure GPS device binding.\n");
	}

	ret = gpio_pin_set(gps_dev, PIN, true);
	if (ret != 0) {
  		printk("Failed to configure GPS device binding.\n");
	}


	const char * res4 = DT_PROP(DT_ALIAS(gps), label);
	printk("label ? %s\n", res4);

	// Ouvrir l'I2C
	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));


	if (i2c_dev == NULL) {
  		printk("Failed to get I2C device binding.\n");
	}


	if(!device_is_ready(i2c_dev)){
		printk("I2C bus %s is not ready !\n\r", i2c_dev->name);
	}
	printk("I2C bus %s is ready !\n\r", i2c_dev->name);




	
	
	ret = i2c_configure(i2c_dev, I2C_SPEED_FAST);
    if (ret != 0) {
        printk("Failed to configure I2C device\n");
        return;
    }

    ret = i2c_reg_write_byte(i2c_dev, GPS_ADR, 0x00, 0x01);
    if (ret != 0) {
        printk("GPS not detected\n");
        return;
    }

    printk("GPS detected\n");







	/*if(i2c_write(dev_sercom_3, "0xFD", sizeof("0xFD"), 0x42) != 0){
		printk("Error i2c_write.\n");
	}
	printk("Write\n\r");

	while(true){
		if(i2c_read(dev_sercom_3, buf, sizeof(buf), 0x42) != 0){
			printk("Fail to fetch sample from slave. \n");
		}
		else	printk("buf=%x\n", buf[0]);



	}*/

	/*uint8_t read_data[BUFFER_SIZE]; // Example buffer for storing GNSS/GPS data
	ret = i2c_read(dev_sercom_3, read_data, sizeof(read_data), GPS_ADR);
	if (ret != 0) {
  		printk("Failed to read I2C data. Error : %d\n", ret);
	}


	for(int i=0; i<BUFFER_SIZE; i++){

		printk("Data : %d\n", read_data[i]);
	}


	int res2 = DT_NODE_HAS_PROP(DT_NODELABEL(sercom_3), clock_frequency);
	printk("clock_frequency ? %d\n", res2);
	
	res2 = DT_PROP(DT_ALIAS(sercom_3), clock_frequency);
	printk("clock_frequency ? %d\n", res2);


	
	int res3 = DT_NODE_HAS_PROP(DT_NODELABEL(sercom_3), label);
	printk("label ? %d\n", res3);*/


}