#include "functions.h"


#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)


#define DEBOUNCE_TIMEOUT_MS 50


static const struct gpio_dt_spec back = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static const struct gpio_dt_spec next = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios, {0});
							      
static struct gpio_callback button_cb_data;
static struct gpio_callback button_cb_data2;


uint64_t last_time = 0;
uint64_t last_time2 = 0;


void back_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	
  uint64_t now = k_uptime_get();
  if ((now - last_time) > DEBOUNCE_TIMEOUT_MS)
  {
  
    	/*printk("BOUTON 1 START\n");
	printk("Button 1 pressed at %" PRIu32 "\n", k_cycle_get_32());
	printk("BOUTON 1 END\n");*/
	
	if(state_display() == 1){
		set_state_display(0);
	}
	else if(state_display() == 2){
		set_state_display(1);
	}
	else if(state_display() == 3){
		set_state_display(2);
	}
  }
  last_time = now;
}


void next_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	uint64_t now = k_uptime_get();
  if ((now - last_time2) > DEBOUNCE_TIMEOUT_MS)
  {
  	
    	/*printk("BOUTON 2 START\n");
	printk("Button 2 pressed at %" PRIu32 "\n", k_cycle_get_32());
	printk("BOUTON 2 END\n");*/
	
	if(state_display() == 0){
		set_state_display(1);
	}
	else if(state_display() == 1){
		set_state_display(2);
	}
	else if(state_display() == 2){
		set_state_display(3);
	}
  }
  last_time2 = now;
}


void init_button(){

	int ret;

	if (!gpio_is_ready_dt(&back)) {
		printk("Error: back device %s is not ready\n",
		       back.port->name);
		return;
	}
	
	ret = gpio_pin_configure_dt(&back, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, back.port->name, back.pin);
		return;
	}
	
	ret = gpio_pin_interrupt_configure_dt(&back,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, back.port->name, back.pin);
		return;
	}
	
	
	if (!gpio_is_ready_dt(&next)) {
		printk("Error: back device %s is not ready\n",
		       next.port->name);
		return;
	}
	
	ret = gpio_pin_configure_dt(&next, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, next.port->name, next.pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(&next,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, next.port->name, next.pin);
		return;
	}
	
	gpio_init_callback(&button_cb_data, back_pressed, BIT(back.pin));
	gpio_add_callback(back.port, &button_cb_data);
	printk("Set up back at %s pin %d\n", back.port->name, back.pin);
	
	
	gpio_init_callback(&button_cb_data2, next_pressed, BIT(next.pin));
	gpio_add_callback(next.port, &button_cb_data2);
	printk("Set up back 2 at %s pin %d\n", next.port->name, next.pin);
}
