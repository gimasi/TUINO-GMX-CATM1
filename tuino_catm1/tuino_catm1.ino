/* _____  _____  __  __             _____  _____ 
  / ____||_   _||  \/  |    /\     / ____||_   _|
 | |  __   | |  | \  / |   /  \   | (___    | |  
 | | |_ |  | |  | |\/| |  / /\ \   \___ \   | |  
 | |__| | _| |_ | |  | | / ____ \  ____) | _| |_ 
  \_____||_____||_|  |_|/_/    \_\|_____/ |_____|
  (c) 2017 GIMASI SA                                               

 * tuino_catm1.ino
 *
 *  Created on: September 27, 2017
 *      Author: Massimo Santoli
 *      Co-Author: Dario Mader
 *      Brief: Example Sketch to use GMX-CATM1 Module
 *      Version: 1.0
 *
 *      License: it's free - do whatever you want! ( provided you leave the credits)
 *
 */
 
#include "gmx_catm1.h"

char devAddress[8] = {0x78,0xAF,0x58,0x02,0x00,0x00,0x00,0x02};
char SendBuffer[256];

long int timer_period_to_tx = 20000;
long int timer_millis_tx = 0;

void setup() {

  String version;
  byte join_status;
  int join_wait=0;

  
  Serial.begin(9600);
  Serial.println("Starting");

  // Init CATM1 IoT board - specify UDP endpoint
  gmxCATM1_init("5.79.89.3","9200",NULL);

  gmxCATM1_getVersion(version);
  Serial.print("GMX-CATM1 Version: "+version);

  gmxCATM1_getIMEI(version);
  Serial.println("GMX-CATM1 IMEI: "+version);

  gmxCATM1_startSwisscom(); 

 /* Need to understand how we know we are joined to the network */
 /* for now we simply delay for 5 seconds */

  while((join_status = gmxCATM1_isNetworkJoined()) != CATM1_NETWORK_JOINED) {
    gmxCATM1_Led2(GMXCATM1_LED_ON);
    delay(500);
    gmxCATM1_Led2(GMXCATM1_LED_OFF);
    Serial.print("Waiting to connect:");
    Serial.println(join_wait);
    join_wait++;
    
    delay(2500);  
  }  

 
  Serial.println("Connected!!!");
  gmxCATM1_Led2(GMXCATM1_LED_ON);
}

void loop() {

  long int delta_tx;


  delta_tx = millis() - timer_millis_tx;

  if ( delta_tx > timer_period_to_tx) {
    Serial.println("TX DATA");

    memcpy(SendBuffer,devAddress,8);
    SendBuffer[8] = 4; // len of payload 4 bytes
    SendBuffer[9] = 0x31; // payload byte 1
    SendBuffer[10] = 0x32; // payload byte 2
    SendBuffer[11] = 0x33; // payload byte 3
    SendBuffer[12] = 0x34; // payload byte 4
    SendBuffer[13] = 0x00; // End

    gmxCATM1_TXData(SendBuffer);
   
    timer_millis_tx = millis();

    // flash LED
    gmxCATM1_Led3(GMXCATM1_LED_ON);
    delay(200);
    gmxCATM1_Led3(GMXCATM1_LED_OFF);
    
   }

}
