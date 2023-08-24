#include "functions.h"



//i2c
const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));

void main(void)
{

	//blink();
	
	//init_module_sara();
	
	i2c_scanner(i2c_dev);
	
	init_module_eva(i2c_dev);
	
	read_gps_data(i2c_dev);
}




