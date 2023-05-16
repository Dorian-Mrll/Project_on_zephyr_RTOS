#include "command.h"

/* Définition de la broche utilisée */
#define GPIO_PIN 5

void main(void)
{
    int ret;
    const struct device *dev;

    /* Récupération du périphérique GPIO */
    dev = device_get_binding(DT_LABEL(DT_ALIAS(gpiob)));
    if (dev == NULL) {
        printk("Impossible de récupérer le périphérique GPIO\n");
        return;
    }

    /* Configuration de la broche en entrée */
    ret = gpio_pin_configure(dev, GPIO_PIN, GPIO_INPUT);
    if (ret < 0) {
        printk("Impossible de configurer la broche en entrée (%d)\n", ret);
        return;
    }

    /* Lecture de la valeur de la broche */
    while (1) {
        int val = gpio_pin_get(dev, GPIO_PIN);
        printk("Valeur de la broche : %d\n", val);
        k_sleep(K_MSEC(1000));
    }
}
