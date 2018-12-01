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
#include <halGpio.h>


/*- Definitions ------------------------------------------------------------*/
// Put your preprocessor definitions here

/*- Types ------------------------------------------------------------------*/
// Put your type definitions here
struct InfoRuche {
	char	id;	//id de la ruche donnée au debut de la connection
	char	temperature;
	float	lat;
	float	lon;
	char	mouvement;
};

/*- Prototypes -------------------------------------------------------------*/
// Put your function prototypes here

char Lis_UART(void);
void Ecris_UART(char data);
void Ecris_UART_string(char const * data, ...);
void init_UART(void);
void SYS_Init(void);


void TEMP_Config(unsigned char TWI_targetSlaveAddress);
char TEMP_Read(void);
void GPS_Read(char *dataBuffer, struct InfoRuche *dataStruct);
void MOUV_Config(void);
void MOUV_Read(struct InfoRuche *dataStruct);
void ACT_Toggle(char on);
void init_struct(struct InfoRuche *dataStruct);
void construction_Paquet(char *paquet, struct InfoRuche *dataStruct, char action);

/*- Variables --------------------------------------------------------------*/
// Put your variables here
uint8_t receivedWireless;	//cette variable deviendra 1 lorsqu'un nouveau paquet aura été recu via wireless (et copié dans "PHY_DataInd_t ind"
							//il faut la mettre a 0 apres avoir géré le paquet; tout message recu via wireless pendant que cette variable est a 1 sera jeté

PHY_DataInd_t ind; //cet objet contiendra les informations concernant le dernier paquet qui vient de rentrer

bool sensor = false;	//True quand on doit envoyer l'info des sensors
bool ledon = false;		//True quand on doit allumer la led;
bool ledoff = false;	//True quand on doit fermer la led;
bool notif = false;		//True quand on recois confirmation
char action = 0;		//valeur selon l'action effectuée
bool sendPaquet = false;
uint8_t LONG_TRAMME = 14;
/*- Implementations --------------------------------------------------------*/

// Put your function implementations here

/*************************************************************************//**
*****************************************************************************/

static void APP_TaskHandler(struct InfoRuche *dataStruct, char *paquet)
{

	if(sendPaquet)	//est-ce qu'on a un paquet à envoyer?
	{
		Ecris_Wireless((uint8_t*)paquet, LONG_TRAMME); //envoie le data packet; nombre d'éléments utiles du paquet à envoyer
		sendPaquet = false;
	}
  

  if(receivedWireless == 1) //est-ce qu'un paquet a été recu sur le wireless? 
  {
	char buf[196];
	if(ind.data[0] == 'S')
	{
		if(ind.data[1] == dataStruct->id)
		{
			if(ind.data[3] == 'E')
			{
				switch(ind.data[2]) 
				{
					case 'F'  :	//update sensor
					sensor = true;
					break;
				
					case 'A'  :	//on actionneur
					ledon = true;
					break;
				
					case 'D'  :	//off actionneur
					ledoff = true;
					break;
				
					case 'N'  :	//Notification
					notif = true;
					break;
				}
			}
		}
		else
		{
			receivedWireless = 0;
		}
		
	}
  }
}


void TEMP_Config(unsigned char TWI_targetSlaveAddress)
{
	bool check = true;	//false if config ok
	while(check)
	{
		char sendData[5];
		sendData[0] = TWI_targetSlaveAddress<<TWI_ADR_BITS|(FALSE<<TWI_READ_BIT);	//address
		sendData[1] = 0b0000001;	//pointer register (to config)
		sendData[2] = 0b0000000;	//config 
	
		TWI_Start_Transceiver_With_Data(sendData,3);	//write config
	
		sendData[0] = 0b1001011<<TWI_ADR_BITS|(TRUE<<TWI_READ_BIT);	//address
		TWI_Start_Transceiver_With_Data(sendData,1);	//write address to read config on sensor
	
		char receivedData[5];
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
	char sendData[5];
	sendData[0] = 0b1001011<<TWI_ADR_BITS|(FALSE<<TWI_READ_BIT);	//address
	sendData[1] = 0b0000000;	//pointer register (to temp)
	
	TWI_Start_Transceiver_With_Data(sendData,2);	//write config
	
	sendData[0] = 0b1001011<<TWI_ADR_BITS|(TRUE<<TWI_READ_BIT);	//address
	TWI_Start_Transceiver_With_Data(sendData,1);	//write address to read config on sensor
	
	char receivedData[5];
	receivedData[0] = 1;
	receivedData[1] = 1;
	
	TWI_Get_Data_From_Transceiver(receivedData,2);
	
	return(receivedData[1]);
}

void GPS_Read(char *dataBuffer, struct InfoRuche *dataStruct)
{
	char *temp;
	temp = dataBuffer;
	int i = 0;
	
	//char temp[90] = "$GPGGA,025548.00,4523.46907,N,07153.85530,W,1,04,5.79,226.5,M,-31.5,M,,*61";
	while(i <= 1019)	//check pour trouver le préfixe $GPGGA dans le buffer de data du GPS
	{
		if(temp[i] == '$' && temp[i + 1] == 'G' && temp[i + 2] == 'P' && temp[i + 3] == 'G' && +temp[i + 4] == 'G' && temp[i + 5] == 'A')
		{
			break;
		}
		i++;
	}
	
	i += 7;

	while(temp[i] != ',')	//incrémente jusqu'a la fin de l'heure
	{
		i++;		
	}
	
	i +=1;	//incrémente pour arriver au debut de latitude
	
	if(temp[i] != ',')
	{
		dataStruct->lat = ((temp[i]-48)*10);
		i++;
		dataStruct->lat += temp[i]-48;
		i++;
		dataStruct->lat += ((((float)temp[i]-48)*10) + ((float)temp[i+1]-48) + (((float)temp[i+3]-48)/10) + (((float)temp[i+4]-48)/100) + (((float)temp[i+5]-48)/1000) + (((float)temp[i+6]-48)/10000)+ (((float)temp[i+7]-48)/100000))/60;
		i += 9;	//incrémente pour arriver au Nord ou Sud
	
		if(temp[i] == 'S')
		{
			dataStruct->lat = (dataStruct->lat) * -1;
		}
		
		i += 2;	//incrémente pour arriver au debut de longitude
		
		dataStruct->lon = ((temp[i]-48)*100);
		i++;
		dataStruct->lon += ((temp[i]-48)*10);
		i++;
		dataStruct->lon += temp[i]-48;
		i++;
		dataStruct->lon += ((((float)temp[i]-48)*10) + ((float)temp[i+1]-48) + (((float)temp[i+3]-48)/10) + (((float)temp[i+4]-48)/100) + (((float)temp[i+5]-48)/1000) + (((float)temp[i+6]-48)/10000)+ (((float)temp[i+7]-48)/100000))/60;
		i += 9;	//incrémente pour arriver au Est ou Ouest
		
		if(temp[i] == 'W')
		{
			dataStruct->lon = (dataStruct->lon) * -1;
		}
	}
}

void MOUV_Config(void)
{
	/*
	DDRE = DDRE|0b00001000;	//PE3 output
	DDRE = DDRE&0b11111011;	//PE2 input
	PORTE = PORTE&0b00001000;	//PE3 active high
	*/
	DDRD = DDRD|0b01000000;	//PD6 output
	DDRD = DDRD&0b11101111;	//PD4 input
}

void MOUV_Read(struct InfoRuche *dataStruct)
{
	char x = 0;
	x = (PIND&0b00010000)>>PIND4;
	if(x)
	{
		dataStruct->mouvement = 1;
	}
	else
	{
		dataStruct->mouvement = 0;
	}
	
}

void ACT_Toggle(char on)
{
	if(on)
	{
		PORTD = PORTD|0b01000000;	//PD6 active high
	}
	else
	{
		PORTD = PORTD&0b00000000;	//PD6 active high
	}
	
}

void init_struct(struct InfoRuche *dataStruct)
{
	dataStruct->id = 1;
	dataStruct->lat = 1;
	dataStruct->lon = 1;
	dataStruct->temperature = 1;
	dataStruct->mouvement = 0;
}

void construction_Paquet(char *paquet, struct InfoRuche *dataStruct, char action)
{
	
	char *latChar;
	char *lonChar;
	
	latChar = &dataStruct->lat;
	lonChar = &dataStruct->lon;
	
	paquet[0] = 'S';
	paquet[1] = dataStruct->id;
	paquet[2] = action;
	paquet[3] = dataStruct->temperature;
	paquet[4] = dataStruct->mouvement;
	paquet[5] = latChar[0];
	paquet[6] = latChar[1];
	paquet[7] = latChar[2];
	paquet[8] = latChar[3];
	paquet[9] = lonChar[0];
	paquet[10] = lonChar[1];
	paquet[11] = lonChar[2];
	paquet[12] = lonChar[3];
	paquet[13] = 'E';
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
	char paquet[LONG_TRAMME]; //déclaration du tableau pour storer la tramme à envoyer
	
	//init antenne externe
	ANT_DIV |= (1<<ANT_EXT_SW_EN); //active le mode manuel pour antenne
	ANT_DIV |= (1<<ANT_CTRL0);
	ANT_DIV &= ~(1<<ANT_CTRL1); //Passe sur antenne externe
	
	//ANT_DIV |= (1<<ANT_CTRL1);
	//ANT_DIV &= ~(1<<ANT_CTRL0); //Passe sur antenne interne
	
	
	
	//init protocols
	SYS_Init();
	TWI_Master_Initialise();
	InitUart_0(baudRate);
	
	//init sensors
	TEMP_Config(TWI_targetSlaveAddress); //config capteur de temperature
	MOUV_Config();	//config capteur de mouvement
	
	
	//init de la structure de données et son pointeur
	struct InfoRuche data;
	struct InfoRuche *dataPtr;
	dataPtr = &data;
	init_struct(dataPtr);
	
	//Main loop
	while (1)
	{
		PHY_TaskHandler(); //stack wireless: va vérifier s'il y a un paquet recu
		APP_TaskHandler(dataPtr, paquet); //l'application principale roule ici
		
		//==========================================
		//update des sensor/update de la structure
		//==========================================
		if (GetRxCountUart0() == RX_BUFFER_SIZE)	//lecture GPS quand buffer UART plein
		{
			nbrBytes = ExtractRxUart0(NMEAGPS,RX_BUFFER_SIZE);
			nbrBytes = FlushRxUart0();
			GPS_Read(NMEAGPS, dataPtr);
		}
		
		if(!(data.mouvement))
		{
			MOUV_Read(dataPtr);
		}
		
		data.temperature = TEMP_Read();
		
		//==========================================
		//Action selon l'état reçu
		//==========================================
		
		if(ledon)	//allume led
		{
			ACT_Toggle(1);
			ledon = false;
			action = 'A';
		}
		
		if(ledoff)	//ferme led
		{
			ACT_Toggle(0);
			ledoff = false;
			action = 'D';
		}
		
		if(sensor)	//set action pour data sensor
		{
			action = 'F';
			sensor = false;
		}
		
		//ajouter alarme
		
		
		//==========================================
		//Construction de la tramme selon l'Action
		//==========================================
		//receivedWireless = true;
		if(receivedWireless)
		{
			construction_Paquet(paquet, dataPtr, action);
			receivedWireless = 0;
			sendPaquet = true;
			data.mouvement = 0;
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


