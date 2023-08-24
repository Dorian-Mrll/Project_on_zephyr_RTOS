#ifndef FUNCTION_H
#define FUNCTION_H


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/i2c.h>


#include <zephyr/sys/printk.h>
#include <zephyr/console/console.h>


#include <stdio.h>
#include <string.h>


// liste des commandes possibles pouvant êtres envoyées
static const uint8_t at_cmd_map[][15] = {
    "AT",
    "AT+CGMR",
    "AT+CGMI",
    "ATI"
};



// uart

// const struct uart_config uart_cfg = {
// 		.baudrate = 115200,
// 		.parity = UART_CFG_PARITY_NONE,
// 		.stop_bits = UART_CFG_STOP_BITS_1,
// 		.data_bits = UART_CFG_DATA_BITS_8,
// 		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
// };


void blink(void);


int test_device(const struct device *uart_dev);


int init_module_sara(void);
int test_poll_in(void);
int test_poll_out(uint8_t *poll_data);

int init_module_eva(const struct device *i2c_dev);
void read_gps_data(const struct device *i2c_dev);
void config_gps(const struct device *i2c_dev);

int i2c_scanner(const struct device *i2c_dev);

void test_gpio(void);

#endif
