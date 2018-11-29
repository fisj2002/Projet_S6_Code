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
#include <stdarg.h>

/*- Definitions ------------------------------------------------------------*/
// Put your preprocessor definitions here

/*- Types ------------------------------------------------------------------*/
// Put your type definitions here
typedef struct{
	char temp;
	char mouv;
	float lon;
	float lat;
} data;

typedef struct{
	char ID;
	data data;
	char command[64];
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

int slaveCount = 0;
int slavePos = 0;
slave slaves[8];
/*- Implementations --------------------------------------------------------*/

// Put your function implementations here

/*************************************************************************//**
*****************************************************************************/ 
/*static void APP_TaskHandler(void)
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
}*/

static void APP_TaskHandler(void)
{
	char receivedUART = 0;
	
	ExtractRxUart(GetUART1(), &receivedUART, 1);
	
	if (receivedUART == 'S')
	{
		char receivedCommand[2];
		
		ExtractRxUart(GetUART1(), receivedCommand, 2);
		
		switch (receivedCommand[1])
		{
			case 'A':
			case 'a':
			{
				char check;
				ExtractRxUart(GetUART1(), &check, 1);
				if (check == 'E')
				{
					SendnUart(GetUART1(), &receivedUART, 1);
					SendUart(GetUART1(), " - Activation de l'actionneur\n\r");
					sprintf(slaves[receivedCommand[0]-1].command, "S%c%cE", receivedCommand[0], receivedCommand[1]);
				}
				break;
			}
			
			case 'D':
			case 'd':
			{
				char check;
				ExtractRxUart(GetUART1(), &check, 1);
				if (check == 'E')
				{
					SendnUart(GetUART1(), &receivedUART, 1);
					SendUart(GetUART1(), " - Désactivation de l'actionneur\n\r");
					sprintf(slaves[receivedCommand[0]-1].command, "S%c%cE", receivedCommand[0], receivedCommand[1]);
				}
				break;
			}
			case 'l':
			case 'L':
			{
				char check;
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
			case 'X':
			case 'x':
			{
				char check;
				ExtractRxUart(GetUART1(), &check, 1);
				if (check == 'E')
				{
					SendnUart(GetUART1(), &receivedUART, 1);
					SendUart(GetUART1(), " - Ajout d'une ruche\n\r");
					sprintf(slaves[receivedCommand[0]-1].command, "S%c%cE", receivedCommand[0], receivedCommand[1]);
				}
				break;
			}

			default:
			{
				SendnUart(GetUART1(), &receivedUART, 1);
				SendUart(GetUART1(), " n'est pas une commande reconnue\n\r");
				break;
			}
		}
	}
	
	int timeBeforeTimeout = 0;
	while(timeBeforeTimeout < 16000)
	{	
		PHY_TaskHandler();
		if(receivedWireless == 1)
		{
			if(ind.data[0] == 'S' && ind.data[1] == slaves[slavePos].ID && ind.data[13] == 'E')
			{
				SendnUart(GetUART1(), ind.data, 14);
				SendnUart(GetUART1(), "\r\n", 2);
				SendUart(GetUART1(), "ID: ");
				char IDstr[1];
				sprintf(IDstr, "%c", ind.data[1] + '0');
				SendnUart(GetUART1(), IDstr, 1);
				SendnUart(GetUART1(), "\r\n", 2);
				SendUart(GetUART1(), "Temp: ");
				char tempstr[4];
				sprintf(tempstr, "%d", (int8_t) ind.data[3]);
				SendUart(GetUART1(), tempstr);
				SendnUart(GetUART1(), "\r\n", 2);
				/*SendUart(GetUART1(), "\n\rLongitude de la ruche: ");
				SendnUart(GetUART1(), ind.data[2], 3);
				SendUart(GetUART1(), "\n\rLatitude de la ruche: ");
				SendnUart(GetUART1(), ind.data[2], 3);*/
				
				break;
			}
			receivedWireless = 0;
		}
		timeBeforeTimeout++;
	}
	
	if(timeBeforeTimeout >= 16000)
	{
		char err[32];
		sprintf(err, "Err slave %c\n\r", slavePos + '1');
		//SendUart(GetUART1(), err);
	}
	else
	{
		slaves[slavePos].command[0] = 0;
	}
	
	slavePos++;
	
	if (slavePos >= slaveCount) slavePos = 0;
	
	if(slaves[slavePos].command[0] != 0)
	{
		Ecris_Wireless((uint8_t*)slaves[slavePos].command, sizeof(slaves[slavePos].command)/sizeof(slaves[slavePos].command[0]));
	}
	else
	{
		char defaultCMD[4];
		sprintf(defaultCMD, "S%cFE", slaves[slavePos].ID);
		Ecris_Wireless((uint8_t*)defaultCMD, 4);
	}
	
	while (TIMER_COUNT > TCNT0){};
	TCNT0 = 0;
}



/*************************************************************************//**
*****************************************************************************/
int main(void)
{
  SYS_Init();
  
  slaveCount = 2;
  slaves[0].ID = 1;
  slaves[0].command[0] = 0;
  slaves[1].ID = 2;
  slaves[1].command[0] = 0;
   
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


