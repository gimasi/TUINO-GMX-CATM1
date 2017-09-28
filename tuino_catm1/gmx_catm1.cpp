/* _____  _____  __  __             _____  _____ 
  / ____||_   _||  \/  |    /\     / ____||_   _|
 | |  __   | |  | \  / |   /  \   | (___    | |  
 | | |_ |  | |  | |\/| |  / /\ \   \___ \   | |  
 | |__| | _| |_ | |  | | / ____ \  ____) | _| |_ 
  \_____||_____||_|  |_|/_/    \_\|_____/ |_____|
  (c) 2017 GIMASI SA                                               

 * gmx_catm1.cpp
 *
 *  Created on: September 27, 2017
 *      Author: Massimo Santoli 
 *      Co-Author: Dario Mader
 *      Brief: Tuino 1 driver for GMX-CATM1 module family
 *      Version: 1.1
 *    
 *      License: it's free - do whatever you want! ( provided you leave the credits)
 *
 *    
 */
#include "Arduino.h"
#include "gmx_catm1.h"
#include "Regexp.h"

#define DEBUG 0

String gmxSerialString;

// RegExp Engine
MatchState ms;
char buf[512]; 

//
String dummyResponse;

byte gmxNB_interface = GMX_UART_INTERFACE;

void(*_NBRing)();

String _udp_socket_ip;
String _upd_port;

//
// ISR 
//
ISR(PCINT1_vect) {     
  if (digitalRead(GMX_INT)==0) {
    if (_NBRing)
      _NBRing();
  }
}

void _resetGMX(){

  pinMode(GMX_RESET,OUTPUT);
  // Reset 
  digitalWrite(GMX_RESET,HIGH);
  delay(100);
  digitalWrite(GMX_RESET,LOW);
  delay(100);
  digitalWrite(GMX_RESET,HIGH);
}


void _log(String data )
{
  #ifdef DEBUG
    Serial.println(data);
  #endif
}

void _sendCmd(String in) {
  int len = in.length();
  long int start_timeout;

 Serial.println("----->");
 Serial.print("SENDING: ");
 Serial.println(in);
 Serial.println("----->");

     
  //  send data
  for (int i=0; i<len; i++) {
    if ( gmxNB_interface == GMX_UART_INTERFACE )
    {
      Serial1.write(in.charAt(i));
    }
    delay(1);
  }
  
  delay(100);

  
  if ( gmxNB_interface == GMX_UART_INTERFACE )
  {
     start_timeout = millis();
     while(Serial1.available()==0){
      if (( millis() - start_timeout ) > GMX_UART_TIMEOUT  )
      {
        Serial.println("TIMEOUT on :"+in);
        break;
      }
     }
  }
}

byte _parseResponse(String& response) {
  
  char cmd[128];
  
  gmxSerialString="";
  
  // Read Serial
  while (Serial1.available()>0) 
  {
      while (Serial1.available()>0) 
      {
        char c = Serial1.read(); 
        gmxSerialString += c;
      }
      
      delay(10);
  }

  gmxSerialString.toCharArray(cmd,gmxSerialString.length());

     Serial.println("<-----");
     Serial.print("RESPONSE: ");
     Serial.println(gmxSerialString);
     Serial.println("<-----");
     
     
  // Parse Response
  ms.Target(cmd);
  char result = ms.Match ("(.*)\r\nOK", 0);
  if ( result == REGEXP_MATCHED )
  { 
    ms.GetCapture (buf, 0);
  
    response = String(buf);
     
    // remove second \r\n => Not very elegant to optimize
    response.toCharArray(cmd,response.length());
    response = String(cmd);
     
    return (GMXCATM1_OK);
  }

  result = ms.Match ("(.*)\r\nSEND OK", 0);
  if ( result == REGEXP_MATCHED )
  { 
    ms.GetCapture (buf, 0);
  
    response = String(buf);
     
    // remove second \r\n => Not very elegant to optimize
    response.toCharArray(cmd,response.length());
    response = String(cmd);
     
    return (GMXCATM1_OK);
  }


  // Check for Errors
  result = ms.Match ("(.*)\r\nERROR", 0);
  if ( result == REGEXP_MATCHED )
  {
    return(GMXCATM1_AT_ERROR);
  }
    
  return(GMXCATM1_AT_GENERIC_ERROR);
}


/* GMXLR Commands Interface */

byte gmxCATM1_init(String upd_addre, String udp_port, void( *callback)()) 
{
    String response;
    byte init_status = GMXCATM1_KO;
    
    _log("GMXCATM1 Init");

    // RESET SETUP
    pinMode(GMX_RESET,OUTPUT);
    digitalWrite(GMX_RESET,HIGH);

    // Boot
    pinMode(GMX_GPIO1,OUTPUT);
    pinMode(GMX_GPIO2,OUTPUT);
    pinMode(GMX_GPIO3,OUTPUT);
    pinMode(GMX_GPIO4,OUTPUT);
    pinMode(GMX_GPIO5,OUTPUT);
  
    digitalWrite(GMX_GPIO1,LOW);
    digitalWrite(GMX_GPIO2,LOW);
    digitalWrite(GMX_GPIO3,LOW);

    // Power On
    digitalWrite(GMX_GPIO4,HIGH);
    //digitalWrite(GMX_GPIO5,HIGH);
    delay(500);
    digitalWrite(GMX_GPIO4,LOW);
    //digitalWrite(GMX_GPIO5,LOW);
    delay(500);
    digitalWrite(GMX_GPIO4,HIGH);
    //digitalWrite(GMX_GPIO5,HIGH);
  
  
    digitalWrite(GMX_GPIO1,HIGH);
    delay(500);
    digitalWrite(GMX_GPIO1,LOW);    
    digitalWrite(GMX_GPIO2,HIGH);
    delay(500);
    digitalWrite(GMX_GPIO2,LOW);
    digitalWrite(GMX_GPIO3,HIGH);
    delay(500);
    digitalWrite(GMX_GPIO3,LOW);
    digitalWrite(GMX_GPIO1,HIGH);
    delay(500);
    digitalWrite(GMX_GPIO1,LOW);
    digitalWrite(GMX_GPIO2,HIGH);
    delay(500);
    digitalWrite(GMX_GPIO2,LOW);
    digitalWrite(GMX_GPIO3,HIGH);
    delay(500); 
    digitalWrite(GMX_GPIO3,LOW);
  
    pinMode(GMX_GPIO1,OUTPUT);
    pinMode(GMX_GPIO2,OUTPUT);
    pinMode(GMX_GPIO3,OUTPUT);
    digitalWrite(GMX_GPIO1,LOW);
    digitalWrite(GMX_GPIO2,LOW);
    digitalWrite(GMX_GPIO3,LOW);

    _resetGMX();

    // Init Interface
    if ( gmxNB_interface == GMX_UART_INTERFACE )
    {
        // Setup Serial
        if(Serial1) {
            Serial1.begin(GMX_UART_SPEED);
           _log("GMX Serial Interface");
            init_status = GMXCATM1_OK;
        } 
        else 
        {
          return(GMXCATM1_NO_SERIAL);
        }
    }

    // Setup Interrupt PIN for Rx
    *digitalPinToPCICR(GMX_INT) |= (1<<digitalPinToPCICRbit(GMX_INT));
    *digitalPinToPCMSK(GMX_INT) |= (1 << digitalPinToPCMSKbit(GMX_INT));

    // set RX callback
    _NBRing = callback;

    // delay to wait BootUp of GMX-LR
    delay(GMX_BOOT_DELAY);
    
    _sendCmd( "AT\r" );
    _parseResponse(response);
    _sendCmd( "AT\r" );
    _parseResponse(response);
    
}

/* Version */
byte gmxCATM1_getVersion(String& version)
{
   _sendCmd( "AT+CGMR\r" );
   return( _parseResponse(version) );
}

/* IMEI */
byte gmxCATM1_getIMEI(String& imei)
{
   _sendCmd( "AT+CGSN\r" );
   return( _parseResponse(imei) );
}


byte gmxCATM1_getCSQ(String& csq)
{
   _sendCmd( "AT+CSQ\r" );
   return( _parseResponse(csq) );
}

void gmxCATM1_startSwisscom() 
{

    // activate CAT-M1
    _sendCmd("AT+QCFG=\"nwscanseq\",02,1\r");
    _parseResponse(dummyResponse);

    _sendCmd("AT+QCFG=\"iotopmode\",0,1\r");
    _parseResponse(dummyResponse);


    _sendCmd("AT+QICSGP=1,1,\"gprs.swisscom.ch\",\"\",\"\",1\r");
    _parseResponse(dummyResponse);

    _sendCmd("AT+QIACT=1\r");
    _parseResponse(dummyResponse);

    _sendCmd("AT+CREG=2\r");
    _parseResponse(dummyResponse);
}


/* Utility */

byte gmxCATM1_getIpAddress(String& ipaddress)
{
 _sendCmd("AT+QIACT?\r");
 return( _parseResponse(ipaddress) );
}

byte gmxCATM1_getNetworkInfo(String& nwinfo)
{
 _sendCmd("AT+QNWINFO\r");
 return( _parseResponse(nwinfo) );
}



/* Check Network */

byte gmxCATM1_isNetworkJoined(void)
{
  byte status;
  char cmd[16];
  String tmp;
  
   _sendCmd( "AT+CREG?\r" );

   if ( (status = _parseResponse(dummyResponse)) == GMXCATM1_OK ) 
  {

    Serial.println("DEBUG AT+CREG");
    Serial.println( dummyResponse );
    Serial.println("END DEBUG***");
    

      byte index = dummyResponse.indexOf(":");
      if ( index != -1 )
      {
        tmp = dummyResponse.substring(index+1,dummyResponse.length());

        tmp.toCharArray(cmd,tmp.length());
        
        if (cmd[0] == '0'){
          return CATM1_NETWORK_NOT_JOINED;
        }
        if (cmd[0] == '1'){
          return CATM1_NETWORK_JOINED;
        }
      }
      
  }   

  return (status); 
}




// TX & RX Data

byte gmxCATM1_TXData(char *data) 
{

  char c;

   _sendCmd("AT+QIOPEN=1,0,\"UDP\",\"5.79.89.3\",9200,0,1\r");
   _parseResponse(dummyResponse);
 
   _sendCmd("AT+QISEND=0\r");

   // Wait for > char
   while (Serial1.available()>0) 
   { 
      while ( c != '>')
      {
        c = Serial1.read();   
      }
   }

  int len = sizeof(data);
     
  //  send data
  for (int i=0; i<len; i++) 
  {
    Serial1.write(data[i]);
    delay(1);
  }

  // Send CTRL-Z
  Serial1.write(0x26);
  _parseResponse(dummyResponse);

  _sendCmd("AT+QICLOSE=0\r");

  return( _parseResponse(dummyResponse) );
  
}



void gmxCATM1_Reset(void){
  String command = String("AT+NRB\r");
  int len = command.length();
  
  //  send data
  for (int i=0; i<len; i++) {
    if ( gmxNB_interface == GMX_UART_INTERFACE )
    {
      Serial1.write(command.charAt(i));
    }
    delay(1);
  }
  
  delay(GMX_BOOT_DELAY);
 
}

void gmxCATM1_StringToHex(String string, char *data, int *len )
{
  char tmp[255];
  char temp[2];
  int j=0;
  
  string.toCharArray(tmp,255);

  for (int i = 0; i < strlen(tmp); i += 2)
  {
    strncpy(temp, &tmp[i], 2);

    temp[0] = toupper(temp[0]);    // Convert to upper case
    temp[1] = toupper(temp[1]);
    
    // Convert hex string to numeric:
    data[j] = (temp[0] <= '9') ? (temp[0] - '0') : (temp[0] - 'A' + 10);
    data[j] *= 16;
    data[j] += (temp[1] <= '9') ? (temp[1] - '0') : (temp[1] - 'A' +10);

    j++;
  }

    *len = j;
}

byte gmxCATM1_Led1(byte led_state) 
{
   digitalWrite(GMX_GPIO1,led_state);
   
}
byte gmxCATM1_Led2(byte led_state)
{
   digitalWrite(GMX_GPIO2,led_state);

}
byte gmxCATM1_Led3(byte led_state)
{
   digitalWrite(GMX_GPIO3,led_state);
}

