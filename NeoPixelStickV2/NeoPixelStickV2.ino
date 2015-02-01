// **INCOMING COMMUNICATION**
// All messages must start with '255', if not, they are discarded and the next byte is read, until a '255' is found
// the first byte after '255' is the Mode byte
// after the Mode byte comes the DataLength byte which states how many bytes the rest of the message is. So the total message = DataLength + 3 NOT USED AT THE MOMENT - all messages are 10 Bytes
// **OUTGOING COMMUNCIATION**
// Feedback from Arduino: integer, prepended by ']' (ascii character 48)is the number of integers in the buffer. 
// Every iteration of the main loop sends out how many byter are in the buffer
// when the buffer is empty ( "]0") this means the arduino is ready to receive a new command
// Any other comments from the Arduino should be prepended with "[" and will show up as text in Max



#include <Adafruit_NeoPixel.h>
#define PIN 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

int inByte = 0; //inByte is constantly read and evaluated for value '255' If its not 255, it is discarded and the next byte is read
int Mode = 0; //Mode is the first byte read after any '255' and determines what happens next.
int DataLength = 0; //The amount of Bytes this mode uses. this is the amount AFTER '255' and 'Mode' and 'ModeBytes' 
int BytesInBuffer = 0;
int colorByte = 0;
int Instruction[100] = {
  0,0,0,0,0,0,0,0,0,0};
int Diagnostic = 0 ; //SWITCH when Diagnostic = 1 more diagnostic info is sent back. Making it 0 switches it off


void setup() {
  // initialize serial communication:
  Serial.begin(9600); 
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Hello.");
}

void loop() 
{ // main section
  Serial.print("]");
  Serial.println(Serial.available());

  if (Serial.available() > 9) // for now, all commands are 10 bytes. Here we wait to make sure all 10 are in
  {
    // only start caring once 3 bytes have arrived ('start', 'mode' and 'DataLength'. Then, check that the first is '255' if not, discard
    inByte=Serial.read();
    if (inByte==255) 
    {// First, determine which mode, and how many Bytes this mode requires
      Mode=Serial.read();
      DataLength=Serial.read();
      //Read in full array IF there are Bytes in the buffer
      for (int i = 0; i <=10; i++)
      {
        Instruction[i]=Serial.read();
        BytesInBuffer = Serial.available();
        //Diagnostic
        if (Diagnostic)
        { //Diag start
          Serial.print("[ Array Reading step = ");
          Serial.println(i);
          Serial.print("[ Bytes in buffer, left to read  = ");
          Serial.println(BytesInBuffer);
        } //Diag end

      } //end loop that runs 10 times

      ///array printer
      if (Diagnostic) {
        Serial.print(" [ Read array at end of reading cycle: ");      
        for (int i = 0; i <=(DataLength-1); i++)
        {
          Serial.print(i);
          Serial.print(":");
          Serial.print(Instruction[i]);
          Serial.print(", ");
        }
        Serial.println("*end*");
      } // end array printer

    } // end section 'if inbyte = 255
    else  // if the inByte is NOT 255, print an error
    {
      // Send some error messages if the first byte is NOT the start byte (255)
      Serial.print("[ ERROR inByte is NOT 255. Instead it is : ");
      Serial.println(inByte);
      Serial.print("[ There are ");
      Serial.print(Serial.available());
      Serial.println(" Bytes left unread in the buffer");
      // End non-255 startbyte error cycle
    }
  }


  // ON to the action

    switch (Mode) 

  { //Start MODES Section

  case 0: //All Off
    {
     if (Diagnostic)
        { //Diag start
         Serial.println("[ Mode: All Off");
        } //Diag end
      
      for(int i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
      }
      strip.show();              // Refresh LED states
      break;

    }

  case 1: //Single LED
    {
      if (Diagnostic)
        { //Diag start
         Serial.println("[ Mode: Single LED");
        } //Diag end
      
      int pix = Instruction[0] ;
      int r = Instruction[1];
      int g = Instruction[2];
      int b = Instruction[3];
      colorByte = strip.Color(r, g, b);
      strip.setPixelColor(pix, strip.Color(r, g, b)); // Set new pixel 'on'
      strip.show();              // Refresh LED states
      break;
    }
  case 2: //All LED
    {
      if (Diagnostic)
        { //Diag start
         Serial.println("[ Mode: All LEDs");
        } //Diag end
      
      int r = Instruction[1];
      int g = Instruction[2];
      int b = Instruction[3];
      for(int i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(r, g, b) ); // Erase pixel, but don't refresh!
      }
      strip.show();              // Refresh LED states
      break;
    }
  case 3: //color chase
    {
      if (Diagnostic)
        { //Diag start
         Serial.println("[ Mode: Chase");
        } //Diag end
      
      int r = Instruction[1];
      int g = Instruction[2];
      int b = Instruction[3];
      int time = (2 * Instruction[0]) ;
      colorChase(strip.Color(r, g, b), time);
      break;
    }
  case 4: //color wipe
    {
      if (Diagnostic)
        { //Diag start
         Serial.println("[ Mode: Wipe");
        } //Diag end
      
      int r = Instruction[1];
      int g = Instruction[2];
      int b = Instruction[3];
      int time = (2 * Instruction[0]) ;

      colorWipe(strip.Color(r, g, b), time);
      break;
    }

  default:
    { 
      Serial.println("[ No Known mode engaged");
      break;
    };
  } //end MODES section

}// end Main section




// Fill the dots progressively along the strip.
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Chase one dot down the full strip.
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}




















