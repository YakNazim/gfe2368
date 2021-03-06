/*! \file isoc_io_sample.c
 *
 */


#include <stdio.h>                      // EOF
#include <string.h>                     // memcpy

#include "gfe2368-util.h"

#include "lpc23xx.h"
#include "lpc23xx-debug.h"
#include "printf-lpc.h"

#include "lpc23xx-mam.h"
#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"
#include "lpc23xx-vic.h"
#include "lpc23xx-usb.h"

#include "gfe2368-info.h"

#include "usbapi.h"

#include "serial-fifo.h"

#include "isoc_io_sample.h"

/*! \brief Local function to handle the USB-CDC class requests
 *
 *  @param [in] pSetup
 *  @param [out] piLen
 *  @param [out] ppbData
 */
static BOOL HandleClassRequest(TSetupPacket *pSetup, int *piLen, uint8_t **ppbData)
{
	return TRUE;
}


/**
  Interrupt handler

  Simply calls the USB ISR, then signals end of interrupt to VIC
  */
static void USBIntHandler(void)
{
    USBHwISR();
    VICAddress = 0x00;    // dummy write to VIC to signal end of ISR
}

/**
	USB frame interrupt handler
	
	Called every milisecond by the hardware driver.
	
	This function is responsible for sending the first of a chain of packets
	to the host. A chain is always terminated by a short packet, either a
	packet shorter than the maximum packet size or a zero-length packet
	(as required by the windows usbser.sys driver).

 */

int delay = 0;

void USBFrameHandler(uint16_t wFrame)
{

    // send over USB
	if( isConnectedFlag ) {

		if( delay < 4000 ) {
			//FIXME need to delay a few seconds before doing isoc writes, impliment more elegant solution, status or event driven....
			delay++;
		} else {

			//Always write whatever is in our most recent isoc output data buffer, you may want to pust somthing interesting in there....
			inputIsocDataBuffer[0]++;

			USBHwEPWrite(ISOC_IN_EP, inputIsocDataBuffer, BYTES_PER_ISOC_FRAME);

			int iLen = USBHwISOCEPRead(ISOC_OUT_EP, outputIsocDataBuffer, sizeof(outputIsocDataBuffer));
			if (iLen > 0) {
				//Insert your code to do somthing interesting here....
				//DBG("z%d", b1);

				//The host sample code will send a byte indicating if the BLUE LED should be on of off.
				if( outputIsocDataBuffer[0] ) {
					// BLUE_LED_ON
				} else {
					BLUE_LED_OFF;
				}

			}
		}
	} else {
		RED_LED_ON;
	}
}

/**
	USB device status handler
	
	Resets state machine when a USB reset is received.
 */
static void USBDevIntHandler(uint8_t bDevStatus)
{

	printf_lpc(UART0, "bDevStatus: %d\n", bDevStatus);
	if ((bDevStatus & DEV_STATUS_RESET) != 0) {
	}

	bDevStat = bDevStatus;

	//FIXME not sure if this is the right way to detect being connected???
	switch(bDevStatus ) {
	case DEV_STATUS_CONNECT:
		isConnectedFlag= 1;
		break;
	case DEV_STATUS_RESET:
	case DEV_STATUS_SUSPEND:
		BLUE_LED_ON;
		isConnectedFlag= 0;
		break;
	}
}

/*************************************************************************
	main
	====
**************************************************************************/
int main(void)
{
	int x = 0;

	const int interval = 200000;

	FIO_ENABLE;

	vic_disableIRQ();

	pllstart_seventytwomhz() ;
	// pllstart_sixtymhz() ;
	//   pllstart_fourtyeightmhz() ;
	
	mam_enable();
	
	info_init();

	uart0_init_115200() ;

	uart0_putstring("\n***Starting gfe color led test***\n\n");
	uart0_putstring("\n***Board is defined as: ");

	uart0_putstring( infoquery_gfe_boardtag() );

	uart0_putstring(" ***\n");

	init_color_led();

	RED_LED_OFF;
	BLUE_LED_OFF;
	GREEN_LED_OFF;

	printf_lpc(UART0,"5 FAST flashes...red, blue then green\n");
	color_led_flash(5, RED_LED, FLASH_FAST ) ;
	RED_LED_OFF;
	color_led_flash(5, BLUE_LED,  FLASH_FAST ) ;
	BLUE_LED_OFF;
	color_led_flash(5, GREEN_LED, FLASH_FAST ) ;
	GREEN_LED_OFF;
	uart0_putstring(" ** END OF TEST ""\n");

	printf_lpc(UART0, "Initialising USB stack\n");

	// initialize stack
	USBInit();

	// register descriptors
	USBRegisterDescriptors(abDescriptors);

	// register class request handler
	USBRegisterRequestHandler(REQTYPE_TYPE_CLASS, HandleClassRequest, abClassReqData);

	// register endpoint handlers
	USBHwRegisterEPIntHandler(INT_IN_EP, NULL);

	// register frame handler
	USBHwRegisterFrameHandler(USBFrameHandler);

	// register device event handler
	USBHwRegisterDevIntHandler(USBDevIntHandler);

	inputIsocDataBuffer[0] = 0;

	printf_lpc(UART0, "Starting USB communication\n");

	VICVectPriority22  = 0x01;         // 0x0 is highest priority, 0xF is lowest
	VICVectAddr22      = (unsigned int)USBIntHandler;
	VICAddress         = 0x0;

	printf_lpc(UART0, "addresses entered\n");

	ENABLE_USB_IRQ;

	printf_lpc(UART0, "USB IRQ Enabled\n");

	vic_enableIRQ();

	printf_lpc(UART0, "vic_enable\n");

	ENABLE_USB_INT;

 	printf_lpc(UART0, "USB INT Enabled\n");



	//printf_lpc(UART0, "connect to bus\n");
//
//	// connect to bus
	USBHwConnect(TRUE);
//

	for(;;) {
		x++;

		if (x == interval) {
			RED_LED_ON;
		} else if (x >= (interval*2)) {
			x = 0;
			RED_LED_OFF;
		}
	}

	return 0;
}

