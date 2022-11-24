/*
 * exp32-typewriter
 * 
 * Turn an Apple ImageWriter II into a typewriter.
 * 
*/

#include <PS2KeyAdvanced.h>

// Define GPIO pins to use for printer serial comms.
#define RXD2 16
#define TXD2 17
#define KEYBOARD_DATA 32
#define KEYBOARD_CLK 14

// track the current horizontal position of the print head. 
int horizontalPosition = 0;

PS2KeyAdvanced keyboard;
uint16_t inChar;

void setup() {
  
  
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // set the imagewriter software switches:
  //    
  //   To enable switches:  ESC Z <sw1> <sw2>
  //   To disable switches: ESC D <sw1> <sw2>
  //   Noe: <sw1> and <sw2> are 8-bit values where the '1' bits select which switches are operated on.

  sleep(1);
  Serial.println("Setting software switches");
  Serial2.write(char(27));            //ESC
  Serial2.write(char(90));            // 'Z'
  Serial2.write(char(247)); // bit positions 5, 6, 7, 8
  Serial2.write(char(0));             // no bit positions

  // fetch the printer
  sleep(1);
  Serial.println("Fetching ID");
  Serial2.write(char(27));  // ESC
  Serial2.write(char(63));  // Send ID command
  while (!Serial2.available()) {
    sleep(1);
  }
  while ( Serial2.available() )  {
    Serial.print(Serial2.read());
  }
  Serial.print("\n");
  sleep(1);
  
  Serial.println("Set up unidirectional printing");
  // set the printer to use unidirectional printing
  Serial2.write(byte(27));  // ESC
  Serial2.write(byte(62));  // Turn on unidirectional printing 

  //sleep(1);
  //Serial.println("Sent printer select");
  //Serial2.write(byte(17));  // printer select (deselect is 19)

  sleep(1);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  
  Serial.println("starting keyboard handler");
  keyboard.begin(KEYBOARD_DATA, KEYBOARD_CLK);


}

void loop() {
  char buff[6];
  
  while (keyboard.available()) {
    
    inChar = keyboard.read();
    if ( inChar > 0 ) {
      
      if (inChar >= 32 && inChar < 127) {

        if (horizontalPosition > 9999) horizontalPosition = 9999;
        sprintf(buff, "F%04d", horizontalPosition);
        
        Serial2.print(char(27));  // ESC
        Serial2.print(buff);
        Serial2.print(char(inChar));
        Serial2.print(char(13));
        
        horizontalPosition = horizontalPosition + 8;
        Serial.print(char(inChar));
      }
      else if(inChar == 287) {

        if (horizontalPosition > 9999) horizontalPosition = 9999;
        sprintf(buff, "F%04d", horizontalPosition);
        
        Serial2.print(char(27));  // ESC
        Serial2.print(buff);
        Serial2.print(' ');
        Serial2.print(char(13));
        
        horizontalPosition = horizontalPosition + 8;          
        Serial.print(' ');
      }
      else if (inChar == 286) {
        horizontalPosition = 0;
        Serial2.print(char(13));
        Serial2.print(char(10));
        Serial.print('\n');
      }
    }
  }
}
