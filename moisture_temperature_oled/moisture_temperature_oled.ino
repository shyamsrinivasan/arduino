#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DS3231.h>
// #include <SD.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define DHTPIN 2  // DHT digital PIN
#define DHTTYPE DHT22 // DHT sensor type DHT11 or DHT22
// #define SDcsPIN 10  // for data logging use digital pin 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // initialize OLED display
DHT dht(DHTPIN, DHTTYPE);  // initialize DHT sensor
RTClib rtc; // initialize clock

void setup() {
  Serial.begin(9600);
  dht.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(5000); // Pause for 5 seconds

  display.clearDisplay(); // Clear the buffer    
  display.setTextColor(SSD1306_WHITE);  // set text color
}

void loop() {
  display.clearDisplay(); // Clear the buffer    
  display.setTextSize(1); // set display font size  
  display.setCursor(0, 0);

  // temperature sensor
  delay(3000);
  DateTime now = rtc.now();               // get current date/time

  display.print(now.year());
  display.print("-");
  display.print(now.month());
  display.print("-");
  display.print(now.day());
  display.display();

  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.println(now.day());

  float cT = dht.readTemperature();
  float humidity = dht.readHumidity();
  float fT = dht.readTemperature(true);

  display.setCursor(0, 8);
  if (isnan(cT) || isnan(fT) || isnan(humidity)){
    display.print("DHT Failure");   
    Serial.println("DHT Failure"); 
    display.display();
  }
  else{
    display.print("Temperature:");
    display.print(cT, 1);
    // display.cp437(true);
    // display.write(167);
    display.print("C");
    display.setCursor(0, 16);
    display.print("Humidity:");
    display.print(humidity, 1);
    display.print("%");

    Serial.print(cT, 1);
    Serial.println("C ");
    Serial.print("Humidity: ");
    Serial.print(humidity, 1);
    Serial.println("%");

    display.display(); 
    delay(1000);  

    //data.T = cT;
    //data.humidity = humidity;
  }

}
