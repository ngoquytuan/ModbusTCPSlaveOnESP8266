/*
    ModbusTCPSlave.h - an Arduino library for a Modbus TCP slave.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	http://www.simplymodbus.ca/TCP.htm
	The equivalent request to this Modbus RTU example

                 11 03 006B 0003 7687
	in Modbus TCP is:

				0001 0000 0006 11 03 04 006B 0003

	[Header bytes] : 9 bytes
	0001: Transaction Identifier
	0000: Protocol Identifier
	0006: Message Length (6 bytes to follow)
	11: The Unit Identifier  (17 = 11 hex)
	04: The number of data bytes to follow
	03: The Function Code (read Analog Output Holding Registers)
	[Data bytes] : 
	006B: The Data Address of the first register requested. (40108-40001 = 107 =6B hex)
	0003: The total number of registers requested. (read 3 registers 40108 to 40110)
	ex for fc06
	Master Tx:03 2D 00 00 00 06 01 06 00 00 11 11
	Slave TX: 03 2D 00 00 00 06 01 06 00 00 11 11

	
	
*/

// Note: The Arduino IDE does not respect conditional included
// header files in the main sketch so you have to select your
// here.
//
#ifndef ModbusTCPSlave_h
#define ModbusTCPSlave_h

#define MB_PORT 502  
#define MB_ESP8266
//#define MBDebug     //Serial1 debug enable

#include "Arduino.h"
#define LED_PIN LED_BUILTIN
#include <ESP8266WiFi.h>


#define maxInputRegister      8
#define maxHoldingRegister    8

//
// MODBUS Function Codes
//
#define MB_FC_NONE 						0
#define MB_FC_READ_COILS 				1
#define MB_FC_READ_DISCRETE_INPUT 		2	
#define MB_FC_READ_REGISTERS 			3              //implemented
#define MB_FC_READ_INPUT_REGISTERS 		4        //implemented
#define MB_FC_WRITE_COIL 				5
#define MB_FC_WRITE_REGISTER 			6              //implemented
#define MB_FC_WRITE_MULTIPLE_COILS 		15
#define MB_FC_WRITE_MULTIPLE_REGISTERS 	16   //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 						0
#define MB_EC_ILLEGAL_FUNCTION 			1
#define MB_EC_ILLEGAL_DATA_ADDRESS 		2
#define MB_EC_ILLEGAL_DATA_VALUE 		3
#define MB_EC_SLAVE_DEVICE_FAILURE 		4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID          			0
#define MB_TCP_PID          			2
#define MB_TCP_LEN          			4
#define MB_TCP_UID          			6
#define MB_TCP_FUNC         			7  //The Function Code
#define MB_TCP_REGISTER_START         	8  //The Data Address of the first register requested.
#define MB_TCP_POS         				10 //The position of the value to write 
#define MB_TCP_REGISTER_NUMBER         	10 //The total number of registers requested
#define MAX_size_of_imcommingdata		100
#define MB_TCP_NumbOfHeaderBytes        9//[see above ^]
#define MB_TCP_FC06_MESSAGE_LENGTH      12//[see above ^]
class ModbusTCPSlave
{
public:
	ModbusTCPSlave(void);
    void begin(const char *ssid, const char *key);
    void begin(const char *ssid, const char *key,uint8_t ip[4],uint8_t gateway[4],uint8_t subnet[4]);
    void Run();
	void Stop();
    unsigned int  MBInputRegister[maxInputRegister];
    unsigned int  MBHoldingRegister[maxHoldingRegister];
	WiFiServer MBServer;
private: 
    byte ByteArray[260];
	char incommingdata[MAX_size_of_imcommingdata];
    bool ledPinStatus = LOW;
    
    
};

#endif
