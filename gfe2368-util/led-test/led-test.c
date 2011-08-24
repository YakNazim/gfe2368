
/*
 * led-test.c
 */

#include <limits.h>
#include <stdint.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"

#include "gfe2368-info.h"
#include "gfe2368-util.h"
#include "led-test.h"

int main (void) {

    int32_t cycles = 3;

    pllstart_seventytwomhz() ;
 //   pllstart_sixtymhz() ;
 //   pllstart_fourtyeightmhz() ;

    info_init();

    uart0_init_115200() ;

    uart0_putstring("\n***Starting gfe color led test***\n\n");
    
    uart0_putstring("\n***Board is defined as: ");

    uart0_putstring( infoquery_gfe_boardtag() );

    uart0_putstring(" ***\n");

    color_led_flash(5, RED_LED,   FLASH_NORMAL ) ;
    color_led_flash(5, BLUE_LED,  FLASH_NORMAL ) ;
    color_led_flash(5, GREEN_LED, FLASH_NORMAL ) ;

    // negative numbers in itoa
    uart0_putstring("Print a negative number: ");
    uart0_putstring(util_itoa(-42, 10));
    uart0_putstring("\n");

    uart0_putstring("Print a negative number: ");
    uart0_putstring(util_itoa(-42, 16));
    uart0_putstring("\n");

    uart0_putstring("0b");
    uart0_putstring(util_itoa(cycles,2));
    uart0_putstring(" cycles.\n");

    uart0_putstring("0d");
    uart0_putstring(util_itoa(cycles,10));
    uart0_putstring(" cycles.\n");

    uart0_putstring("0x");
    uart0_putstring(util_itoa(cycles,16));
    uart0_putstring(" cycles.\n");

    uart0_putstring("\n\n***Done***\n\n");

    return(0);

}



