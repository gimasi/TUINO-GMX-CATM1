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


long int timer_period_to_tx = 20000;
long int timer_millis_tx = 0;

void setup() {

  String version;
  byte join_status;
  int join_wait=0;

  
  Serial.begin(9600);
  Serial.println("Starting");

  // Init CATM1 IoT board - specify UDP endpoint
  gmxCATM1_init("1.1.1.1""9200",NULL);

  gmxCATM1_getVersion(version);
  Serial.println("GMX-CATM1 Version:"+version);

  gmxCATM1_getIMEI(version);
  Serial.println("GMX-CATM1 IMEI:"+version);

  gmxCATM1_startSwisscom(); 

  while((join_status = gmxCATM1_isNetworkJoined()) != CATM1_NETWORK_JOINED) {
    gmxNB_Led2(GMXCATM1_LED_ON);
    delay(500);
    gmxNB_Led2(GMXCATM1_LED_OFF);
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
    gmxCATM1_TXData("313233");
   
    timer_millis_tx = millis();

    // flash LED
    gmxCATM1_Led3(GMXCATM1_LED_ON);
    delay(200);
    gmxCATM1_Led3(GMXCATM1_LED_OFF);
    
   }

}
