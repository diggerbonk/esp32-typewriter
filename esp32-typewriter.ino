/*
 * exp32-typewriter
 * 
 * Turn an Apple ImageWriter II into a typewriter.
 * 
*/

// Define GPIO pins to use for printer serial comms.
#define RXD2 16
#define TXD2 17

// track the current horizontal position of the print head. 
int horizontalPosition = 0;

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

  sleep(1);
  Serial.println("Sent printer select");
  if (Serial2.availableForWrite() > 0) Serial2.write(byte(17));  // printer select (deselect is 19)

  sleep(1);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
}

void loop() {
  int incomingByte = -1;
  char buff[6];
  while (Serial.available()) {
    incomingByte = Serial.read();
    if (incomingByte == -1) break;
    else if (incomingByte == 13 || incomingByte == 10) { // intercept returns and newlines.
      Serial.println("gobbling up carriage return");
      //Serial2.print(char(13));
      //Serial2.print(char(10));
    }
    else if (incomingByte == 38) {
      horizontalPosition = 0;
      Serial2.print(char(13));
      Serial2.print(char(10));
    }
    else {
      // convert horizontal position to a 4 padded 4 character string decimal.
      if (horizontalPosition > 9999) horizontalPosition = 9999;
      sprintf(buff, "F%04d", horizontalPosition);
      
      Serial2.print(char(27));  // ESC
      Serial2.print(buff);
      Serial2.print(char(incomingByte));
      Serial2.print(char(13));
      
      horizontalPosition = horizontalPosition + 8;
      
    }
  }
}
