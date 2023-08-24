#include "functions.h"



//uart
//const struct device *uart_debug = DEVICE_DT_GET(DT_NODELABEL(sercom5));
const struct device *uart_debug = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
const struct device *uart_modem_sara = DEVICE_DT_GET(DT_NODELABEL(sercom2));

//i2c
//const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));




//const struct device *display_dev;
//const struct display_capabilities capabilities;


#define DELAY_MS 5000




void main(void)
{

	//display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	//init_eink_display(display_dev, capabilities);



	//printk("MESSAGE DE TEST\n");
		
	//if(init_module_sara(uart_debug, uart_modem_sara))
	init_mqtt();
	
	
	/*uint8_t last_display = 0;
	
	display_text(display_dev, capabilities, 1);
	init_button();


	while(1){
	
		printk("state_display %d\n", state_display());
	
		if(state_display() == 0 && last_display != 0){
		
			last_display = 0;
			display_text(display_dev, capabilities, 0);
		}
		else if(state_display() == 1 && last_display != 1){
		
			last_display = 1;
			display_text(display_dev, capabilities, 1);
		}
		else if(state_display() == 2 && last_display != 2){
		
			last_display = 2;
			display_text(display_dev, capabilities, 2);
			
			while(state_display() != 3)		
			

			tempsEcoule = k_uptime_get_32() - debut;

			printf("Temps écoulé : %u ms\n", tempsEcoule);

			if (tempsEcoule >= DELAY_MS) {
			// Effectuer l'action après le délai de 2 secondes
			printf("Action exécutée après 2 secondes !\n");
			set_state_display(0);
			// Réinitialiser le compteur pour le prochain délai
			debut = k_uptime_get_32();
			break;
			}
			
			
			
			set_state_display(0);
		}
		else if(state_display() == 3 && last_display != 3){
		
			last_display = 3;
			display_text(display_dev, capabilities, 3);
			
			k_sleep(K_MSEC(5000)); // pour l'uart via l'usb
					
			set_state_display(0);
		}
		
		
		k_sleep(K_MSEC(2000)); // pour l'uart via l'usb
	}*/

	
	
	//blink();
	
	//if(init_module_sara(uart_debug, uart_modem_sara))
	//	read_write_cellular_data(uart_debug, uart_modem_sara);
	
	
	//i2c_scanner(i2c_dev);
	
	//init_module_eva(i2c_dev);
	
	//read_gps_data(i2c_dev);
			
}


