#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(5000); // Pause for 5 seconds

  // Clear the buffer
  display.clearDisplay();

  // set display font size
  display.setTextSize(1);

  // set text color
  display.setTextColor(SSD1306_WHITE);

  // display.setFont(&FreeSans9pt7b);

  display.setCursor(0,0);

  display.println("Hello, World");

  //display.setCursor(5, 10);

  display.println("Your World");

  display.display();
  delay(100);

}

void loop() {
  display.startscrollright(0x00, 0x0F);
  delay(5000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(5000);
  display.stopscroll();
  delay(1000);  
  display.startscrolldiagright(0x00, 0x07);
  delay(5000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(5000);
  display.stopscroll();
  delay(1000);

}
