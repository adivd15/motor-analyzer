
#include "drivers_main.h"
#include "drivers_hx711.h"

uint32_t conversion_value;

GPIO_TypeDef *HX711_CLK_PORT;
uint16_t HX711_CLK_PIN;

GPIO_TypeDef *HX711_DO_PORT;
uint16_t HX711_DO_PIN;

TIM_HandleTypeDef HX711_timer;

void drivers_hx711_wait_approx_100ns(uint8_t hundred_ns);
uint32_t drivers_hx711_start_conversion();

void drivers_hx711_init(
    GPIO_TypeDef *hx711_clk_port, uint16_t hx711_clk_pin, 
    GPIO_TypeDef * hx711_do_port, uint16_t hx711_do_pin
)
{
    conversion_value = 0;

    HX711_CLK_PORT = hx711_clk_port;
    HX711_CLK_PIN = hx711_clk_pin;

    HX711_DO_PORT = hx711_do_port;
    HX711_DO_PIN = hx711_do_pin;
}

void drivers_hx711_main(void)
{
    uint32_t val = drivers_hx711_start_conversion();
    if(val)
    {
        conversion_value = val;
    }
}

uint32_t drivers_hx711_get_conversion_val()
{
    return conversion_value;
}

uint32_t drivers_hx711_start_conversion()
{
    uint32_t l_conversion_val = 0;
    //check if DO pin is low (data is ready)
    if(HAL_GPIO_ReadPin(HX711_DO_GPIO_Port, HX711_DO_PIN) == 0)
    {
        for(int i =0; i < 24; i++)
        {
            //start reading 24 bits from the adc
            HAL_GPIO_WritePin(HX711_CLK_GPIO_Port, HX711_CLK_PIN, GPIO_PIN_SET);
            //wait at least 0.1 us for DO to settle
            drivers_hx711_wait_approx_100ns(1);
            l_conversion_val = l_conversion_val << 1;
            if(HAL_GPIO_ReadPin(HX711_DO_GPIO_Port, HX711_DO_PIN))
            {
                l_conversion_val++;
            }
            //wait another 0.2 us before lowering the clock
            drivers_hx711_wait_approx_100ns(2);
            HAL_GPIO_WritePin(HX711_CLK_GPIO_Port, HX711_CLK_PIN, GPIO_PIN_RESET);
            drivers_hx711_wait_approx_100ns(2);
        }
        drivers_hx711_wait_approx_100ns(2);
        HAL_GPIO_WritePin(HX711_CLK_GPIO_Port, HX711_CLK_PIN, GPIO_PIN_SET); 
        drivers_hx711_wait_approx_100ns(2);
        HAL_GPIO_WritePin(HX711_CLK_GPIO_Port, HX711_CLK_PIN, GPIO_PIN_RESET); 
    }
    else
    {
        return 0;
    }

    return l_conversion_val ^ 0x800000;
}

void drivers_hx711_wait_approx_100ns(uint8_t hundred_ns)
{
    for (uint32_t i = 0; i < HAL_RCC_GetSysClockFreq() / 10000000 * hundred_ns; i++) 
    {
        //use inline assembly istead of simple _NOP to avoid volatile i in for loop
        __asm__("nop");
    }
}