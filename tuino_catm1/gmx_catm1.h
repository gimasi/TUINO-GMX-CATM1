/* _____  _____  __  __             _____  _____ 
  / ____||_   _||  \/  |    /\     / ____||_   _|
 | |  __   | |  | \  / |   /  \   | (___    | |  
 | | |_ |  | |  | |\/| |  / /\ \   \___ \   | |  
 | |__| | _| |_ | |  | | / ____ \  ____) | _| |_ 
  \_____||_____||_|  |_|/_/    \_\|_____/ |_____|
  (c) 2017 GIMASI SA                                               

 * gmx_catm1.h
 *
 *  Created on: September 27, 2017
 *      Author: Massimo Santoli
 *      Co-Author: Dario Mader
 *      Brief: Tuino 1 driver for GMX-CATM1 module family
 *      Version: 1.0
 *
 *      License: it's free - do whatever you want! ( provided you leave the credits)
 *
 */


#define GMXCATM1_KO                      -1 

#define GMXCATM1_OK                      0 

#define GMXCATM1_NOT_PRESENT             10
#define GMXCATM1_NO_SERIAL               20

#define GMXCATM1_AT_ERROR                30
#define GMXCATM1_AT_GENERIC_ERROR        99

#define GMXCATM1_LED_ON                  1
#define GMXCATM1_LED_OFF                 0

#define GMX_UART_INTERFACE            1
#define GMX_SPI_INTERFACE             2

#define GMX_UART_SPEED                115200
#define GMX_UART_TIMEOUT              10000

#define GMX_BOOT_DELAY                4000


#define CATM1_NETWORK_JOINED             1
#define CATM1_NETWORK_NOT_JOINED         0
 

byte gmxCATM1_init(void (*function)());
byte gmxCATM1_getVersion(String& version);
byte gmxCATM1_getIMEI(String& imei);

byte gmxCATM1_radioON(String& param);
byte gmxCATM1_setAPN(String APN);
byte gmxCATM1_isNetworkJoined(void);

byte gmxCATM1_getCSQ(String& csq);


void gmxCATM1_startSwisscom();


// TX & RX Data
byte gmxNB_TXData(String data);
byte gmxNB_RXData(String& data);



/*
 * Utilities
 */
 
byte gmxNB_Led1(byte led_state);
byte gmxNB_Led2(byte led_state);
byte gmxNB_Led3(byte led_state);

void gmxNB_Reset(void);
void gmxNB_StringToHex(String string, char *data, int *len );



