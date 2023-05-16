#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <drivers/i2c.h>


#include <stdio.h>
#include <string.h>



#include <sys/printk.h>
#include <console/console.h>



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

const struct device *uart_modem_sara = DEVICE_DT_GET(DT_NODELABEL(sercom2));
const struct device *uart_debug = DEVICE_DT_GET(DT_NODELABEL(sercom5));

// i2c
const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(sercom3));


static int test_device(const struct device *uart_dev);
static int test_poll_in(void);
static int test_poll_out(uint8_t *poll_data);





// module sara-r410
const struct gpio_dt_spec power_enable_sara = GPIO_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(sercom2), sara_r4), mdm_power_gpios);
const struct gpio_dt_spec change_voltage_sara = GPIO_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(sercom2), sara_r4), mdm_vint_gpios);


// module eva8m
//const struct gpio_dt_spec power_enable_eva = GPIO_DT_SPEC_GET(DT_CHILD(DT_NODELABEL(sercom3), eva8m_42), gps_power_gpios);