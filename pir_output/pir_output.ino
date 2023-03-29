// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SH110X.h>

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels
// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define pirSensPIN 3  // PIR Sensor pin
#define LEDPIN 13 // LED output pin

int pirState = LOW;   // initial pIR state
int val = 0;   

// Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // initialize OLED display

void setup() {

  pinMode(pirSensPIN, INPUT);   // set pin mode for PIR sensor
  pinMode(LEDPIN, OUTPUT);  // pin mode for output LED
  Serial.begin(9600);

  // display.begin(SCREEN_ADDRESS, true);
  // display.display();
  // delay(1000);

  // display.clearDisplay(); // Clear the buffer    
  // display.setTextColor(SH110X_WHITE);  // set text color
}

void loop() {
  
  val = digitalRead(pirSensPIN);

  // display.clearDisplay(); // Clear the buffer    
  // display.setTextSize(1); // set display font size  
  // display.setCursor(0, 0);

  if (val == HIGH)  {    
    digitalWrite(LEDPIN, HIGH);
    if (pirState == LOW)  {
      pirState = HIGH;
      // display.println(F("Motion Detected"));
      // display.display();
      Serial.println(F("Motion Detected"));
    }
  }
  else  {
    digitalWrite(LEDPIN, LOW);
    if (pirState == HIGH) {      
      pirState = LOW;
      // display.println(F("Motion Stopped"));
      // display.display();
      Serial.println(F("Motion Stopped"));
    }
  }

}

