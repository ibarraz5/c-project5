/*
 * main.c
 *
 * SER486 Project 1
 * Ivan Barraza Duran
 *
 This file implements Uart, led, and rtc are all intialized.
 Led is set to blink OK
 Details are printed to measure performance of led_update
 */

#include "uart.h"
#include "delay.h"
#include "led.h"
#include "rtc.h"




int main(void)
{
    //initialize uart, led and rtc
    uart_init();
    led_init();
    rtc_init();

    // Set default led morse code to OK
    led_set_blink("--- -.-");

    // initialize rtc date/time
    rtc_set_by_datestr("01/01/2019 00:00:00");

    // print assignment title
    uart_writestr("SER 486 Project 1 - Ivan Barraza Duran\n\r");

    // get baseline performance 
    signed long c1=0;
    delay_set(1,10000); while (!delay_isdone(1)) { c1++; }

    // measure performance led_update 
    signed long c2=0;
    delay_set(1,10000); while (!delay_isdone(1)) { led_update(); c2++; }

    // display the results 
    uart_writedec32(c1); uart_writestr(" ");
    uart_writedec32(c2); uart_writestr("\r\n");

    // set the delay 1 counter
    delay_set(1,0);

    while(1){
        if(delay_isdone(1)){
            // update the date every 500ms
            delay_set(1,500);
            uart_writestr(rtc_get_date_string());
            // \r allows the date string to update without scrolling.
            uart_writestr("\r");
        }
        led_update();
    }


    return 0;
}
