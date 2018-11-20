/**
 * \file template.c
 *
 * \brief Empty application template
 *
 * Copyright (C) 2012-2014, Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 * Modification and other use of this code is subject to Atmel's Limited
 * License Agreement (license.txt).
 *
 * $Id: template.c 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include "sys.h"
#include "phy.h"
#include "TWI_Master.h"
#include "uart.h"
#include <stdio.h>
#include <stdarg.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*- Definitions ------------------------------------------------------------*/
// Put your preprocessor definitions here

/*- Types ------------------------------------------------------------------*/
// Put your type definitions here

/*- Prototypes -------------------------------------------------------------*/
// Put your function prototypes here
/*
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_UART(void);
void SYS_Init(void);
*/

void TEMP_Config(unsigned char TWI_targetSlaveAddress);
char TEMP_Read(void);

/*- Variables --------------------------------------------------------------*/
// Put your variables here
uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
							//il faut la mettre a 0 apres avoir géré le paquet; tout message recu via wireless pendant que cette variable est a 1 sera jeté

PHY_DataInd_t ind; //cet objet contiendra les informations concernant le dernier paquet qui vient de rentrer


/*- Implementations --------------------------------------------------------*/

// Put your function implementations here

/*************************************************************************//**
*****************************************************************************/
/*
static void APP_TaskHandler(void)
{
  char receivedUart = 0;

  receivedUart = Lis_UART();  
  if(receivedUart)		//est-ce qu'un caractere a été recu par l'UART?
  {
	  Ecris_UART(receivedUart);	//envoie l'echo du caractere recu par l'UART

	  if(receivedUart == 'a')	//est-ce que le caractere recu est 'a'? 
		{
		uint8_t demonstration_string[128] = "123456789A"; //data packet bidon
		Ecris_Wireless(demonstration_string, 10); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
		}
  }

  if(receivedWireless == 1) //est-ce qu'un paquet a été recu sur le wireless? 
  {
	char buf[196];

	Ecris_UART_string( "\n\rnew trame! size: %d, RSSI: %ddBm\n\r", ind.size, ind.rssi );
	Ecris_UART_string( "contenu: %s", ind.data );	
	receivedWireless = 0; 
  }
}
*/

void TEMP_Config(unsigned char TWI_targetSlaveAddress)
{
	bool check = true;	//false if config ok
	while(check)
	{
		unsigned char sendData[5];
		sendData[0] = TWI_targetSlaveAddress<<TWI_ADR_BITS|(FALSE<<TWI_READ_BIT);	//address
		sendData[1] = 0b0000001;	//pointer register (to config)
		sendData[2] = 0b0000000;	//config 
	
		TWI_Start_Transceiver_With_Data(sendData,3);	//write config
	
		sendData[0] = 0b1001011<<TWI_ADR_BITS|(TRUE<<TWI_READ_BIT);	//address
		TWI_Start_Transceiver_With_Data(sendData,1);	//write address to read config on sensor
	
		unsigned char receivedData[5];
		receivedData[0] = 1;
		receivedData[1] = 1;
	
		TWI_Get_Data_From_Transceiver(receivedData,2);
	
		if(receivedData[1] == sendData[2])
		{
			check = FALSE;
		}
	}
}

char TEMP_Read(void)
{
	unsigned char sendData[5];
	sendData[0] = 0b1001011<<TWI_ADR_BITS|(FALSE<<TWI_READ_BIT);	//address
	sendData[1] = 0b0000000;	//pointer register (to temp)
	
	TWI_Start_Transceiver_With_Data(sendData,2);	//write config
	
	sendData[0] = 0b1001011<<TWI_ADR_BITS|(TRUE<<TWI_READ_BIT);	//address
	TWI_Start_Transceiver_With_Data(sendData,1);	//write address to read config on sensor
	
	unsigned char receivedData[5];
	receivedData[0] = 1;
	receivedData[1] = 1;
	
	TWI_Get_Data_From_Transceiver(receivedData,2);
	
	return(receivedData[1]);
}

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	//variables
	unsigned char TWI_targetSlaveAddress;
	char currentTempValue = 0;
	TWI_targetSlaveAddress = 0x4B;	//Temp sensor address
	uint16_t baudRate = 9600;
	int nbrBytes = 0;
	unsigned char NMEAGPS[RX_BUFFER_SIZE];	//receive buffer for GPS data (max size)
	
	//init protocols
	SYS_Init();
	TWI_Master_Initialise();
	InitUart_0(baudRate);
	
	//init sensors
	TEMP_Config(TWI_targetSlaveAddress); //config capteur de temperature
	
	/*//test i2c
	currentTempValue = TEMP_Read();	//test lecture temp
	printf(currentTempValue);
	*/
	
	//Main loop
	while (1)
	{
		PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
		//APP_TaskHandler(); //l'application principale roule ici
		
		if (GetRxCountUart0() == RX_BUFFER_SIZE)
		{
			nbrBytes = ExtractRxUart0(NMEAGPS,RX_BUFFER_SIZE);
			nbrBytes = FlushRxUart0();
		}
		
	}
}







//FONCTION D'INITIALISATION
/*************************************************************************//**
*****************************************************************************/
void SYS_Init(void)
{
receivedWireless = 0;
wdt_disable(); 
//init_UART();
PHY_Init(); //initialise le wireless
PHY_SetRxState(1); //TRX_CMD_RX_ON
}
//

//FONCTIONS POUR L'UART
/*
char Lis_UART(void)
{

char data = 0; 


	if(UCSR1A & (0x01 << RXC1))
	{
		data = UDR1;
	}
	
return data;
}


void Ecris_UART(char data)
{
	UDR1 = data;
	while(!(UCSR1A & (0x01 << UDRE1)));
}

void Ecris_UART_string(char const * data, ...)
{
	va_list args; 
	char buf[256];
	unsigned int index = 0;
	
	va_start(args, data);
	vsprintf(buf, data, args);	
	
	while(buf[index] != 0x00 && index<256)
		{
		Ecris_UART(buf[index]);
		index++;
		}
	
	va_end(args);
}

void init_UART(void)
{
	//baud rate register = Fcpu / (16*baud rate - 1)
	//baudrate = 9600bps //s'assurer que la fuse CLKDIV8 n'est pas activée dans les Fuses, sinon ca donne 1200bps
	UBRR1H = 0x00;
	UBRR1L = 53;
	
	UCSR1A = 0x00;
	UCSR1B = 0x18; //receiver, transmitter enable, no parity
	UCSR1C = 0x06; //8-bits per character, 1 stop bit
}

*/
