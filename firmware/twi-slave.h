/* 
 * Copyright (c) 2004, Atmel Corporation
 * 
 * The sourcecode was orginally from Atmel Application Note
 * -- AVR311: TWI Slave Implementation --
 *
 * Changes to make it compile with the GNU C Compiler with the avr-libc were made by Bernhard Walle
 * Further changes made by Scott Perry
 *
 * -------------------------------------------------------------------------------------------------
 */

#pragma once

/****************************************************************************
  TWI Status/Control register definitions
****************************************************************************/

#define TWI_BUFFER_SIZE 2      // Reserves memory for the drivers transceiver buffer. 

/****************************************************************************
  Callback definitions
****************************************************************************/

// Called to solicit data for transmission
extern void (*TWI_Tx_Data_Callback)( unsigned char * , unsigned char * );

// Called to provide received data
extern void (*TWI_Rx_Data_Callback)( unsigned char * , unsigned char );

/****************************************************************************
  Function definitions
****************************************************************************/

void TWI_Slave_Initialise( unsigned char );
