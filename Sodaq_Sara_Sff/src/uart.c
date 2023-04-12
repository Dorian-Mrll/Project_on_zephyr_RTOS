#include <zephyr.h>
#include <device.h>
#include <drivers/uart.h>


/*void main(void)
{
    //int ret;

 

	const char *dev_name = DT_LABEL(DT_ALIAS(sercom_5));
	//const struct device *uart_dev = 
	device_get_binding(dev_name);


   	//ret = uart_configure(uart_dev, &uart_cfg);
    //if (ret) {
        //printk("Impossible de configurer la liaison série (code d'erreur %d)\n", ret);
    //}

	
	//without k_msleep the message will not be displayed correctly 
	//k_msleep(1);
    
    printk("Port UART ouvert avec succès !\n");
	
	for(int i=65; i<91; i++){
		printk("%c", (char)i);
		k_msleep(100);
	}
    printk("\nEND !\n");
}*/
