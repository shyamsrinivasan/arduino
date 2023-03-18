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
#define sensorPin A0  // sensor pin for moisture sensor
// #define SDcsPIN 10  // for data logging use digital pin 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // initialize OLED display
DHT dht(DHTPIN, DHTTYPE);  // initialize DHT sensor
RTClib rtc; // initialize clock

const int dryValue = 636; // moisture sensor dry value
const int wetValue = 321; // moisture sensor wet value
int intervals = (dryValue - wetValue)/3;  // intervals very wet, wet, dry

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
  display.setCursor(60, 0);
  display.print(now.hour());
  display.print(":");
  display.print(now.minute());
  display.display();

  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.print("  ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.println(now.minute());

  float cT = dht.readTemperature();
  float humidity = dht.readHumidity();
  float fT = dht.readTemperature(true);

  display.setCursor(0, 10);
  display.print("Air: ");  
  if (isnan(cT) || isnan(fT) || isnan(humidity)){
    display.print("DHT Failure");   
    Serial.println("DHT Failure"); 
    display.display();
  }
  else{
    // display.print("Temperature:");
    display.print(cT, 1);
    // display.cp437(true);
    // display.write(167);
    display.print("C");
    display.setCursor(70, 10);
    // display.print("Humidity:");
    display.print(humidity, 1);
    display.print("%");
    display.display(); 

    Serial.print(cT, 1);
    Serial.println("C ");
    Serial.print("Humidity: ");
    Serial.print(humidity, 1);
    Serial.println("%");  
    //data.T = cT;
    //data.humidity = humidity;
  }

  // soil moisture sensor
  int moisture = readSensor();  
  display.setCursor(0, 20);
  display.print("Soil: ");
  display.setCursor(30, 20);
  
  // check read failure of Soil moisture sensor
  if (isnan(moisture)){    
    display.print(F("SM Err"));
    display.display();

    Serial.println("SM Err");
  }
  float moisture_percent = map(moisture, 634, 321, 0, 100);
  String moisture_category;
  int moisture_level;

  if(moisture >= wetValue && moisture < (wetValue + intervals))
  {
    moisture_category = "Very Wet";
    moisture_level = 3;
  }
  else if(moisture > (wetValue + intervals) && moisture < (dryValue - intervals))
  {
    moisture_category = "Wet";
    moisture_level = 2;
  }
  else if(moisture <= dryValue && moisture > (dryValue - intervals))
  {
    moisture_category = "Dry";
    moisture_level = 1;
  }

  // print moisture levels    
  display.print(moisture_percent, 0);
  display.print("%");
  display.setCursor(55, 20);
  display.print(moisture_category);  
  display.display();

  Serial.print("Soil Moisture: ");
  Serial.print(moisture);
  Serial.print(" ");
  Serial.print(moisture_category);
  Serial.print(" ");
  Serial.print(moisture_percent, 1);
  Serial.println("%");

  delay(1000);  
}

int readSensor() {  
  // read analog value from sensor
  int val = analogRead(sensorPin);
  return val;
}
