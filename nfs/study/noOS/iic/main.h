#ifndef __MAIN_H__
#define __MAIN_H__


// key.c
void key_init_interrupt(void);
void isr_eint2(void);
void isr_eint3(void);
void isr_eint16171819(void);
void key_init(void);
void key_polling(void);

// led.c
void led_blink(void);
void led1(void);
void led2(void);
void led3(void);
void led_off(void);
//iic.c
void i2c0_int_func(void);
void i2c0_init(void);

// uart.c
void uart_init(void);
#endif
