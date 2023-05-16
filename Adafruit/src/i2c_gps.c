#include "command.h"

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


#define GPS_ADR 0x42

void main(void)
{
    int ret;

	// Activer broche 18
	/*const struct device * gps_dev;

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
    k_sleep(K_MSEC(1000));*/



    test_device(i2c_dev);

    gpio_pin_configure(power_enable_eva.port, power_enable_eva.pin, GPIO_OUTPUT_ACTIVE);
    k_sleep(K_MSEC(500));


    

  



    ret = i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_MASTER);
    if (ret < 0) {
        printk("Error configuring I2C: %d\n", ret);
        return;
    }

    
    k_sleep(K_MSEC(5000));


    ret = i2c_write(i2c_dev, (uint8_t)0xFD, 1, GPS_ADR);
    if (ret != 0) {
        printk("Error writing to GPS: %d\n", ret);
        return;
    }

    uint8_t buf_size;
    ret = i2c_read(i2c_dev, &buf_size, 1, 0x42);
    if (ret < 0) {
        printk("Error reading GPS buffer size: %d\n", ret);
    } else {
        printk("GPS buffer size: %d\n", buf_size);
    }

    




    const uint8_t ubx_without_checksum[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B};

    uint8_t ck_a = 0, ck_b = 0;
    for(int i=2; i<sizeof(ubx_without_checksum)-2; i++)
    {   ck_a = ck_a + ubx_without_checksum[i];
        ck_b = ck_b + ck_a;
    }

    printk("ck_a : %d\n", ck_a);
    printk("ck_b : %d\n", ck_b);


    const uint8_t ubx[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01, ck_a, ck_b};
    

    // HEADER       : SYNC_CHAR1 : 0xB5 & SYNC_CHAR2 : 0x62
    // CLASS        : 0x06
    // ID           : 0x01
    // LENGTH       : 0x08 & 0x00
    // PAYLOAD      : (0xF0, 0x01) for GLL
    // CHECKSUM     : CK_A : 0x01 & CK_B : 0x2B


    ret = i2c_write(i2c_dev, ubx, sizeof(ubx), GPS_ADR);
    if (ret < 0) {
        printk("Error writing GPS: %d\n", ret);
    } else {
        printk("Success writing GPS\n");
    }





    uint8_t buf[buf_size];

    memset(buf, 0, buf_size);

    k_sleep(K_MSEC(1000));


    while (1) {

        for(int i=0; i<buf_size; i++){

            ret = i2c_read(i2c_dev, &buf[i], 1, GPS_ADR);

            if (ret < 0) {
                printk("Error reading GPS: %d\n", ret);
            } else {
                
                printk("%c", buf[i]);
            }
        
        }
        k_sleep(K_MSEC(1000));
    }
}