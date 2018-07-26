#include <Arduino.h>
#include "AModbusTCPSlave.h"


ModbusTCPSlave::ModbusTCPSlave(void):MBServer(MB_PORT)
{
}


#ifdef MB_ESP8266
void ModbusTCPSlave::begin(const char *ssid, const char *key){
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    ledPinStatus = LOW;
    #ifdef MBDebug
    Serial.begin(115200);
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    #endif
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, key);
    
    while (WiFi.status() != WL_CONNECTED) {
        // Blink the LED
        digitalWrite(LED_PIN, ledPinStatus); // Write LED high/low
        ledPinStatus = (ledPinStatus == HIGH) ? LOW : HIGH;
        delay(100);
        #ifdef MBDebug
        Serial.print(".");
        #endif
    }
    #ifdef MBDebug
    Serial.println("");
    Serial.println("WiFi connected");
    #endif
    
    #ifdef MBDebug
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("WiFi signal");
    Serial.println(WiFi.RSSI());
    #endif
    
    // Start the server
    digitalWrite(LED_PIN, HIGH);
    MBServer.begin();
    #ifdef MBDebug
    Serial.print(F("Listening on "));
    Serial.print(MB_PORT);
    Serial.println(" ...");
    #endif
}
/*
void ModbusTCPSlave::begin(const char *ssid, const char *key,uint8_t ip[4],uint8_t gateway[4],uint8_t subnet[4]){
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    ledPinStatus = LOW;
#ifdef MBDebug
    Serial.begin(115200);
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif
    
    WiFi.config(IPAddress(ip), IPAddress(gateway), IPAddress(subnet));
    
    WiFi.begin(ssid, key);
    
    while (WiFi.status() != WL_CONNECTED) {
        // Blink the LED
        digitalWrite(LED_PIN, ledPinStatus); // Write LED high/low
        ledPinStatus = (ledPinStatus == HIGH) ? LOW : HIGH;
        delay(100);
#ifdef MBDebug
        Serial.print(".");
#endif
    }
#ifdef MBDebug
    Serial.println("");
    Serial.println("WiFi connected");
#endif
    
#ifdef MBDebug
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("WiFi signal");
    Serial.println(WiFi.RSSI());
#endif
    
    // Start the server
    digitalWrite(LED_PIN, HIGH);
    MBServer.begin();
#ifdef MBDebug
    Serial.print(F("Listening on "));
    Serial.print(MB_PORT);
    Serial.println(" ...");
#endif
}
*/
#endif
/*
void ModbusTCPSlave::Stop(){

	MBClient.close();

}

*/

void ModbusTCPSlave::Run(){
    boolean flagClientConnected = 0;
    byte byteFN = MB_FC_NONE;
    int Start;
    int WordDataLength;
	int TotalNumbOfRegsResquest;
	int TotalNumbOfBytesRespond;
    int ByteDataLength;
    int MessageLength;
    unsigned long timer_t;
    //****************** Read from socket ****************
	//Neu 2 clients la chet ngay (24.7.2018)
    WiFiClient client_one;// = MBServer.available();
    // Check if a client has connected
	if (MBServer.hasClient()){
		client_one = MBServer.available();
		Serial.println("Client Connected IP :");
		IPAddress  ip2 = client_one.remoteIP();
		Serial.println(ip2);	
		timer_t = millis();
	}
	
    while (client_one.connected()) {
		
		
		if (millis() - timer_t >= 10000) {
			Serial.println(" ngat ket noi day");
			client_one.stop();
		}
		
		if(client_one.available())
		{
			while(client_one.available()) 
          {
            size_t sizeofimcommingdata = client_one.available();
            
            client_one.readBytes(incommingdata,sizeofimcommingdata);
            client_one.flush(); 
			//The Function Code
			byteFN = incommingdata[MB_TCP_FUNC];
			
			//print to serial port to see what i received
			//Serial.write(incommingdata,sizeofimcommingdata);
			#ifdef MBDebug
				
				Serial.println();
				Serial.print("RX: ");
				for (byte thisByte = 0; thisByte < 20; thisByte++) {
					Serial.print(incommingdata[thisByte], HEX);
					Serial.print(" ");
				}
				Serial.println();
				Serial.print("RXF: ");
				Serial.println(byteFN);
				/*
				if(String(incommingdata).startsWith("ABC",0)) 
				{
                Serial.println(" ABC ");
                }*/
			#endif
            
            
            }
		}
		// Handle request
		switch(byteFN) {
				case MB_FC_NONE:
					//client_one.stop();
						break;
				
				case MB_FC_READ_REGISTERS:  // 03 Read Holding Registers
				case MB_FC_READ_INPUT_REGISTERS:  // 04 Read Input Registers
					//The Data Address of the first register requested.
					Start = word(incommingdata[MB_TCP_REGISTER_START],incommingdata[MB_TCP_REGISTER_START+1]);
					//The total number of registers requested
					TotalNumbOfRegsResquest = word(incommingdata[MB_TCP_REGISTER_NUMBER],incommingdata[MB_TCP_REGISTER_NUMBER+1]);
					TotalNumbOfBytesRespond = TotalNumbOfRegsResquest * 2;
					incommingdata[5] = TotalNumbOfBytesRespond + 3; //[ID][FUNC][numbOfdatas] Number of bytes after this one.
					incommingdata[8] = TotalNumbOfBytesRespond;     //[numbOfdatas] Number of bytes after this one (or number of bytes of data).
					
					if(byteFN == MB_FC_READ_REGISTERS){
							for(int i = 0; i < TotalNumbOfRegsResquest; i++)
								{
									incommingdata[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
									incommingdata[10 + i * 2] =  lowByte(MBHoldingRegister[Start + i]);
								}
					}
					else{
							for(int i = 0; i < TotalNumbOfRegsResquest; i++)
								{
									incommingdata[ 9 + i * 2] = highByte(MBInputRegister[Start + i]);
									incommingdata[10 + i * 2] =  lowByte(MBInputRegister[Start + i]);
								}
					}
					
					
					MessageLength = TotalNumbOfBytesRespond + MB_TCP_NumbOfHeaderBytes;
					client_one.write((const uint8_t *)incommingdata,MessageLength);
					#ifdef MBDebug
						Serial.print("TX: ");
						for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
							Serial.print(incommingdata[thisByte], HEX);
							Serial.print(" ");
						}
						Serial.println();
					#endif
					byteFN = MB_FC_NONE;
					break;
				case MB_FC_WRITE_REGISTER:  // 06 Write Holding Register
					int Addr;
					//The Data Address of the first register requested.
					Addr = word(incommingdata[MB_TCP_REGISTER_START],incommingdata[MB_TCP_REGISTER_START+1]);
					MBHoldingRegister[Addr] = word(incommingdata[MB_TCP_POS],incommingdata[MB_TCP_POS+1]);
					//incommingdata[5] = 6; //Number of bytes after this one.
					MessageLength = MB_TCP_FC06_MESSAGE_LENGTH;
					client_one.write((const uint8_t *)incommingdata,MessageLength);
					#ifdef MBDebug
						Serial.print("TX: ");
						for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
							Serial.print(incommingdata[thisByte], HEX);
							Serial.print(" ");
						}
						Serial.println();
						Serial.print("Write Holding Register: ");
						Serial.print(Addr);
						Serial.print("=");
						Serial.println(MBHoldingRegister[Addr]);
					#endif
					byteFN = MB_FC_NONE;
					break;
				 case MB_FC_WRITE_MULTIPLE_REGISTERS:    //16 Write Holding Registers
					//000021-Tx:03 4E 00 00 00 11 01 10 00 00 00 05 0A 11 11 22 22 33 33 44 44 55 55
					//000022-Rx:03 4E 00 00 00 06 01 10 00 00 00 05
					
					int TotalNumbOfRegsToWrite;//The number of registers to write 
					Addr = word(incommingdata[MB_TCP_REGISTER_START],incommingdata[MB_TCP_REGISTER_START+1]);
					TotalNumbOfRegsToWrite = word(incommingdata[MB_TCP_POS],incommingdata[MB_TCP_POS+1]);
					
					incommingdata[5] = 6; //Number of bytes after this one. => see 000022-Rx
					for(int i = 0; i < TotalNumbOfRegsToWrite; i++)
					{
						MBHoldingRegister[Addr + i] =  word(incommingdata[ 13 + i * 2],incommingdata[14 + i * 2]);
					}
					MessageLength = MB_TCP_FC06_MESSAGE_LENGTH;//FC06 = FC16
					client_one.write((const uint8_t *)incommingdata,MessageLength);
				 
					#ifdef MBDebug
						
						for (byte thisByte = 0; thisByte < TotalNumbOfRegsToWrite; thisByte++) {
							Serial.print("Holding Registers ");
							Serial.print(thisByte+Addr, HEX);
							Serial.print(": ");
							Serial.println(MBHoldingRegister[Addr + thisByte], HEX);
							
						}

					#endif

					byteFN = MB_FC_NONE;

					break;

		}
		/*
		else if(client_one.available())
		{
			flagClientConnected = 1;
			int i = 0;
			while(client_one.available())
			{
				ByteArray[i] = client_one.read();
				i++;
			}
			client_one.flush();        
			//The Function Code
			byteFN = ByteArray[MB_TCP_FUNC];
			//The Data Address of the first register requested.
			Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
			//The total number of registers requested
			WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
			#ifdef MBDebug
			Serial.println();
			Serial.print("RX: ");
			for (byte thisByte = 0; thisByte < 20; thisByte++) {
				Serial.print(ByteArray[thisByte], HEX);
				Serial.print("-");
			}
			Serial.println();
			Serial.print("RXF: ");
			Serial.println(byteFN);

			#endif
		}
		*/
    
    // Handle request
/*
    switch(byteFN) {
        case MB_FC_NONE:
            break;
        
        case MB_FC_READ_REGISTERS:  // 03 Read Holding Registers
            ByteDataLength = WordDataLength * 2;
            ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
            ByteArray[8] = ByteDataLength;     //Number of bytes after this one (or number of bytes of data).
            for(int i = 0; i < WordDataLength; i++)
            {
                ByteArray[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
                ByteArray[10 + i * 2] =  lowByte(MBHoldingRegister[Start + i]);
            }
            MessageLength = ByteDataLength + 9;
            client_one.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
            #endif
			client_one.stop();
            byteFN = MB_FC_NONE;
            break;
            
        case MB_FC_READ_INPUT_REGISTERS:  // 04 Read Input Registers
            Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
            WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
            ByteDataLength = WordDataLength * 2;
            ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
            ByteArray[8] = ByteDataLength;     //Number of bytes after this one (or number of bytes of data).
            for(int i = 0; i < WordDataLength; i++)
            {
                ByteArray[ 9 + i * 2] = highByte(MBInputRegister[Start + i]);
                ByteArray[10 + i * 2] =  lowByte(MBInputRegister[Start + i]);
            }
            MessageLength = ByteDataLength + 9;
            client_one.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
            #endif
			client_one.stop();
            byteFN = MB_FC_NONE;
            break;
            
        case MB_FC_WRITE_REGISTER:  // 06 Write Holding Register
            MBHoldingRegister[Start] = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
            ByteArray[5] = 6; //Number of bytes after this one.
            MessageLength = 12;
            client_one.write((const uint8_t *)ByteArray,MessageLength);
            #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
                Serial.print("Write Holding Register: ");
                Serial.print(Start);
                Serial.print("=");
                Serial.println(MBHoldingRegister[Start]);
            #endif
			client_one.stop();
            byteFN = MB_FC_NONE;
            break;
            
        case MB_FC_WRITE_MULTIPLE_REGISTERS:    //16 Write Holding Registers
            ByteDataLength = WordDataLength * 2;
            ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
            for(int i = 0; i < WordDataLength; i++)
            {
                MBHoldingRegister[Start + i] =  word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
            }
            MessageLength = 12;
            client_one.write((const uint8_t *)ByteArray,MessageLength);
         
		 #ifdef MBDebug
                Serial.print("TX: ");
                for (byte thisByte = 0; thisByte <= MessageLength; thisByte++) {
                    Serial.print(ByteArray[thisByte], DEC);
                    Serial.print("-");
                }
                Serial.println();
                Serial.print("Write Holding Registers from: ");
                Serial.print(Start);
                Serial.print("=");
                Serial.println(WordDataLength);
            #endif
			
			client_one.stop();
            byteFN = MB_FC_NONE;

            break;
    }
	*/
    }
	/*
    client_one.stop();
    
    if (flagClientConnected == 1){
        #ifdef MBDebug
        Serial.println("client disonnected");
        #endif
        flagClientConnected = 0;
    }*/
}

