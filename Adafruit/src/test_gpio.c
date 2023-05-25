#include "function.h"

struct gpio_dt_spec tab_spec[20] = {
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 0),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 1),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 2),

    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 3),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 4),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 5),

    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 6),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 7),

    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 8),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 9),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 10),
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(other), gpios, 11),
}; 



void test_gpio(void)
{
    int ret;
    int i=0;
    bool isGPIO = true;

    while(isGPIO){

        struct gpio_dt_spec spec = tab_spec[i];

        /* Récupération du périphérique GPIO */
        if (spec.port == NULL) {
            printk("Impossible de récupérer le périphérique GPIO\n");
            isGPIO = false;
            return;
        }
        printk("_______________________________________\n");
        printk("Name %s, Pin %d, Flag %d\n", spec.port->name, spec.pin, spec.dt_flags);
        printk("api %p, config %p, data %p\n", spec.port->api, spec.port->config, spec.port->data);

        ret = gpio_pin_configure(spec.port, spec.pin, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
            printk("Impossible de configurer le GPIO (code d'erreur %d)\n", ret);
            return;
        }

        ret = gpio_pin_set(spec.port, spec.pin, 1);
        if (ret < 0) {
            printk("Impossible de configurer le GPIO (code d'erreur %d)\n", ret);
            return;
        }else   printk("GPIO allumé\n");
        k_msleep(500);

        ret = gpio_pin_set(spec.port, spec.pin, 0);
        if (ret < 0) {
            printk("Impossible de configurer le GPIO (code d'erreur %d)\n", ret);
            return;
        }else   printk("GPIO éteint\n");
        k_msleep(500);
    
        ret = gpio_pin_configure(spec.port, spec.pin, GPIO_INPUT);
        if (ret < 0) {
            printk("Impossible de configurer le GPIO (code d'erreur %d)\n", ret);
            return;
        }
        k_msleep(500);

        ret = gpio_pin_configure(spec.port, spec.pin, GPIO_DISCONNECTED);
        if (ret < 0) {
            printk("Impossible de configurer la broche en entrée (%d)\n", ret);
            return;
        }
        k_msleep(500);

        i++;
    }
}
