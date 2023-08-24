#include "functions.h"
/*
#define I2C_ADDR_MIN 0x08 // Adresse minimale à scanner
#define I2C_ADDR_MAX 0x77 // Adresse maximale à scanner


int i2c_scanner(const struct device *i2c_dev)
{

    if(!test_device(i2c_dev)) return -1;
    if(!init_module_eva(i2c_dev)) return -1;
    
    int ret;
    struct i2c_msg msg;
    uint8_t dummy_byte = 0;
    int i2c_addr;

    printf("Scanning des adresses I2C...\n");

    for (i2c_addr = I2C_ADDR_MIN; i2c_addr <= I2C_ADDR_MAX; i2c_addr++) {
        msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;
        msg.len = 1;
        msg.buf = &dummy_byte;

        ret = i2c_transfer(i2c_dev, &msg, 1, i2c_addr);
        if (ret == 0) {
            printf("Adresse I2C détectée: 0x%x\n", i2c_addr);
        }
		//printf("ret: %d\n", ret);
    }

    printf("Terminé.\n");
    return 1;
}
*/
