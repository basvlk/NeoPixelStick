
#include <Adafruit_NeoPixel.h>
#define PIN 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

int inByte = 0; //inByte is constantly read and evaluated for value '255' If its not 255, it is discarded and the next byte is read
int Mode = 0; //Mode is the first byte read after any '255' and determines what happens next. It 
int ModeBytes = 0; //The amount of Bytes this mode uses. this is the amount ADFTER '255' and 'Mode'
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
  Serial.print("bytes Avail: ");
  Serial.println(Serial.available());
  Serial.print("]");
  Serial.println(Serial.available());

  if (Serial.available() > 9) 
  {
    // only start caring once 10 bytes have arrived. Then, check that the first is '255' if not, discard
    inByte=Serial.read();
    Serial.println(inByte);
    if (inByte==255) 
    {// First, determine which mode, and how many Bytes this mode requires
      Mode=Serial.read();
      ModeBytes=Serial.read();
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
        Serial.print("Read array at end of reading cycle: ");      
        for (int i = 0; i <=(ModeBytes-1); i++)
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
      for(int i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
      }
      strip.show();              // Refresh LED states
      break;

    }

  case 1: //Single LED
    {
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
      int r = Instruction[1];
      int g = Instruction[2];
      int b = Instruction[3];
      int time = (2 * Instruction[0]) ;
      colorChase(strip.Color(r, g, b), time);
      break;
    }
  case 4: //color wipe
    {
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




















