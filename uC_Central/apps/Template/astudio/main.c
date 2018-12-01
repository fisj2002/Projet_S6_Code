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
#include "uart.h"
#include "timer.h"
#include <string.h>
#include <stdarg.h>

/*- Definitions ------------------------------------------------------------*/
// Put your preprocessor definitions here

/*- Types ------------------------------------------------------------------*/
// Put your type definitions here
typedef struct{
	char temp;
	char mouv;
	char lon[4];
	char lat[4];
} data;

typedef struct{
	char ID;
	char state;
	data data;
	char command;
} slave;

/*- Prototypes -------------------------------------------------------------*/
// Put your function prototypes here
char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_UART(void);
void SYS_Init(void);

/*- Variables --------------------------------------------------------------*/
// Put your variables here
uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
							//il faut la mettre a 0 apres avoir géré le paquet; tout message recu via wireless pendant que cette variable est a 1 sera jeté

PHY_DataInd_t ind; //cet objet contiendra les informations concernant le dernier paquet qui vient de rentrer
char sendFlag = 0;
int timeBeforeTimeout = 0;

int slaveCount = 0;
int slavePos = 0;
slave slaves[8];
/*- Implementations --------------------------------------------------------*/

// Put your function implementations here

/*****************************************************************************
*****************************************************************************/ 

static void APP_TaskHandler(void)
{
	char receivedUART = 0;
	
	ExtractRxUart(GetUART1(), &receivedUART, 1);
	
	if (receivedUART == 'S')
	{
		char receivedCommand[2] = { 0 };
		
		while (GetRxCountUart(GetUART1()) < 2);
		
		ExtractRxUart(GetUART1(), receivedCommand, 2);
		
		switch (receivedCommand[1])
		{
			case 'A':
			case 'D':
			{
				char check;
				
				while (GetRxCountUart(GetUART1()) < 1);
				ExtractRxUart(GetUART1(), &check, 1);
				
				if (check == 'E')
				{
					slaves[receivedCommand[0]-1].command = receivedCommand[1];
				}
				break;
			}
			
			case 'F':
			{
				char check;
				
				while (GetRxCountUart(GetUART1()) < 1);
				ExtractRxUart(GetUART1(), &check, 1);
				
				if (check == 'E')
				{
					char mess[15];
					sprintf(mess, "S%c%c%c%c00000000E", slaves[receivedCommand[0]-1].ID,  slaves[receivedCommand[0]-1].state, slaves[receivedCommand[0]-1].data.temp, slaves[receivedCommand[0]-1].data.mouv);
					memcpy(mess + 5, slaves[receivedCommand[0]-1].data.lon, 4);
					memcpy(mess + 9, slaves[receivedCommand[0]-1].data.lat, 4);
					SendnUart(GetUART1(), mess, 14);
				}
				break;
			}
			
			case 'L':
			{
				char check;
				
				while (GetRxCountUart(GetUART1()) < 1);
				ExtractRxUart(GetUART1(), &check, 1);
				
				if (check == 'E')
				{
					char mess[32] = { 'S', 0xFF, 'L', slaveCount };
					for (int i = 0; i < slaveCount ; i++)
					{
						int length = strlen(mess);
						mess[length] = slaves[i].ID;
					}
					int length = strlen(mess);
					mess[length] = 'E';
					
					SendUart(GetUART1(), mess);
				}
				break;
			}		
			
//			case 'X':
//			{
//				char check;
//				ExtractRxUart(GetUART1(), &check, 1);
//				if (check == 'E')
//				{
//					SendnUart(GetUART1(), &receivedUART, 1);
//					sprintf(slaves[receivedCommand[0]-1].command, "S%c%cE", receivedCommand[0], receivedCommand[1]);
//				}
//				break;
//			}

			default:
			{
				break;
			}
		}
	}
	
	if (!sendFlag)
	{
		receivedWireless = 0;
		
		char CMD[4] = { 0 };
		sprintf(CMD, "S%c%cE", slaves[slavePos].ID, slaves[slavePos].command);
		Ecris_Wireless((uint8_t*)CMD, 4);
		
		sendFlag = 1;
	}
	else
	{
		if(receivedWireless)
		{
			if(ind.data[0] == 'S' && ind.data[1] == slaves[slavePos].ID && ind.data[2] == slaves[slavePos].command && ind.data[13] == 'E')
			{
				slaves[slavePos].state = ind.data[2];
				slaves[slavePos].data.temp = ind.data[3];
				slaves[slavePos].data.mouv = ind.data[4];
				memcpy(slaves[slavePos].data.lon, ind.data + 5, 4);
				memcpy(slaves[slavePos].data.lat, ind.data + 9, 4);
				
				timeBeforeTimeout = 0;
				slavePos = (slavePos + 1) % slaveCount;
				sendFlag = 0;
				
				slaves[slavePos].command = 'F';
			}
			receivedWireless = 0;
		}
		timeBeforeTimeout++;
	
		if(timeBeforeTimeout >= 5000)
		{
			slaves[slavePos].state = 'X';
			timeBeforeTimeout = 0;
			slavePos = (slavePos + 1) % slaveCount;
			sendFlag = 0;
		}
	}
	
	 // while (TIMER_COUNT_LOOP > TCNT0){};
	 // TCNT0 = 0;
}



/*************************************************************************//**
*****************************************************************************/
int main(void)
{
  SYS_Init();
  
  slaveCount = 2;
  slaves[0].ID = 1;
  slaves[0].command = 'F';
  slaves[1].ID = 2;
  slaves[1].command = 'F';
   
  while (1)
  {
    PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
    APP_TaskHandler(); //l'application principale roule ici
  }
}







//FONCTION D'INITIALISATION
/*************************************************************************//**
*****************************************************************************/
void SYS_Init(void)
{
	//init antenne externe
	ANT_DIV |= (1<<ANT_EXT_SW_EN); //active le mode manuel pour antenne
	ANT_DIV |= (1<<ANT_CTRL0);
	ANT_DIV &= ~(1<<ANT_CTRL1); //Passe sur antenne externe
	
	//ANT_DIV |= (1<<ANT_CTRL1);
	//ANT_DIV &= ~(1<<ANT_CTRL0); //Passe sur antenne interne
	
	receivedWireless = 0;
	wdt_disable(); 
	init_UART();
	init_Timer();
	PHY_Init(); //initialise le wireless
	PHY_SetRxState(1); //TRX_CMD_RX_ON
}
//




















//FONCTIONS POUR L'UART

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
	InitUart_0(BAUD_RATE);
	InitUart_1(BAUD_RATE);
}


