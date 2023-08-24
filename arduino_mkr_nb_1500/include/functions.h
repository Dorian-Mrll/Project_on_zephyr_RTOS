#ifndef FUNCTION_H
#define FUNCTION_H


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/spi.h>


#include <zephyr/sys/printk.h>
#include <zephyr/console/console.h>


#include <zephyr/drivers/display.h>
#include <zephyr/display/cfb.h>


#include <stdio.h>
#include <string.h>


#include <zephyr/sys/util.h>

#include <zephyr/usb/usb_device.h>

// liste des commandes possibles pouvant êtres envoyées
static const uint8_t at_cmd_map[][20] = {
    "AT",
    "AT+CGPADDR",
    "AT+CGMR",
    "AT+CGMI",
    "ATI",
    "AT+CGDCONT?",
    "AT+CGCONTRDP",
    "AT+COPS?",
    "AT+CREG?",
};



// uart

// const struct uart_config uart_cfg = {
// 		.baudrate = 115200,
// 		.parity = UART_CFG_PARITY_NONE,
// 		.stop_bits = UART_CFG_STOP_BITS_1,
// 		.data_bits = UART_CFG_DATA_BITS_8,
// 		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
// };


#define DUMPSTER_NAME "17m3 BOIS"

void blink(void);


int test_device(const struct device *uart_dev);


int init_module_sara(const struct device *uart_debug, const struct device *uart_modem_sara);
int read_write_cellular_data(const struct device *uart_debug, const struct device *uart_modem_sara);

int test_poll_in(const struct device *uart_debug, const struct device *uart_modem_sara);
int test_poll_out(const struct device *uart_modem_sara, uint8_t *poll_data);

int init_module_eva(const struct device *i2c_dev);
void read_gps_data(const struct device *i2c_dev);
void config_gps(const struct device *i2c_dev);

int i2c_scanner(const struct device *i2c_dev);

void test_gpio(void);



void init_eink_display(const struct device *display_dev, struct display_capabilities capabilities);

void display_text(const struct device *display_dev, struct display_capabilities capabilities, uint8_t state);


void display(const struct device *display_dev, const char * const screen, struct display_capabilities capabilities);
void display2(const struct device *display_dev, struct display_capabilities capabilities);
void display3(const struct device *display_dev, struct display_capabilities capabilities);
void display_error(const struct device *display_dev, uint8_t state, struct display_capabilities capabilities);


void display_logo(const struct device *display_dev, struct display_capabilities capabilities);



uint8_t state_display();
void set_state_display(uint8_t state);


void init_button(void);

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

#endif
