#ifndef DRIVERS_hx711_H_
#define DRIVERS_H_

#include "main.h"

void drivers_hx711_init(
    GPIO_TypeDef *hx711_clk_port, uint16_t hx711_clk_pin, 
    GPIO_TypeDef * hx711_do_port, uint16_t hx711_do_pin
);

void drivers_hx711_main(void);

uint32_t drivers_hx711_get_conversion_val();

#endif // drivers_H_
