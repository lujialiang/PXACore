//Serial Port Driver

#include "stdafx.h"
#include "comdrv.h"
#include "pxa255.h"

void SerialOutputByte(const char);
int SerialInputByte(char *);

BOOL Com1DrvInitial(struct __DRIVER_OBJECT* lpDrvObj)
{
	struct __DEVICE_OBJECT*  lpDevObject = NULL;

	lpDevObject = IOManager.CreateDevice((struct __COMMON_OBJECT*)&IOManager,
		"COM1",
		2048,
		1024,
		32,
		32,
		NULL,
		lpDrvObj);

	if(NULL == lpDevObject)  //Failed to create device object.
	{
		printf("COM Driver: Failed to create device object for COM1.");
		return FALSE;
	}

	//Initizial gpio

	/* GP39, GP40, GP41 UART(10) */
        GAFR1_L |= 0x000A8000;
        GPDR1 |= 0x00000380;
	
        /* 8-bit, 1 stop, no parity */
	rFFLCR = 0x00000003;
	
	/* Reset tx, rx FIFO. clear. FIFO enable */
	rFFFCR = 0x00000007;
        
	/* UART Enable Interrupt */
	rFFIER = 0x00000040;

	/* DLAB set=latch registers, DLAB clear= . */
        rFFLCR |= 0x00000080;
        
	/* baud rate */
        rFFDLL = SERIAL_BAUD_115200;
	
	/* DLAB clear */
	rFFLCR &= 0xFFFFFF7F;
	
        /* Transmit Shift Register, Transmit Holding Register, FIFO
	 * wait for ready */
	while (!rFFLSR & 0x00000040 )
		/* wait */ ;

	return TRUE;
}



BOOL Com2DrvEntry(struct __DRIVER_OBJECT* lpDrvObj)
{
	struct __DEVICE_OBJECT*  lpDevObject = NULL;

	lpDevObject = IOManager.CreateDevice((struct __COMMON_OBJECT*)&IOManager,
		"COM2",
		2048,
		1024,
		32,
		32,
		NULL,
		lpDrvObj);
	if(NULL == lpDevObject)  //Failed to create device object.
	{
		printf("COM Driver: Failed to create device object for COM2.");
		return FALSE;
	}
	return TRUE;

}

void SerialOutputByte(const char c)
{
	/* FIFO
	 * wait for ready */
	while ((rFFLSR & 0x00000020) == 0 )
		/* wait */ ;

	rFFTHR = ((DWORD)c & 0xFF);

	/* regardless of c=='\n' or "\n\r", the same output. */
	if (c=='\n')
		SerialOutputByte('\r');
}
	        
int SerialInputByte(char *c)
{

	/* FIFO */
	if ((rFFLSR & 0x00000001) == 0) {
		return 0;
	}
	else {
		*(volatile char *) c = (char) rFFRBR;
		return 1;
	}
}

/**
 * @brief check if serial is ready
 * @retval 1 if the data received
 */
int SerialIsReadyChar(void)
{
	/* Make sure the data is received. */
	//Check rFFLSR Value
	if (rFFLSR & 0x00000001)
	       	return 1;
	return 0;
}

/**
 * @brief Receives a character from serial device
 * @retval
 */
char SerialIsGetChar(void)
{
	/* received data */
	return (char) rFFRBR;
}
