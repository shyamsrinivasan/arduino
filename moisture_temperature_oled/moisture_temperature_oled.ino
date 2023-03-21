#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DS3231.h>
#include <SD.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define DHTPIN 4  // DHT digital PIN
#define DHTTYPE DHT22 // DHT sensor type DHT11 or DHT22
#define sensorPin A0  // sensor pin for moisture sensor

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
  
  delay(3000);
  int x[5];
  float val[3];

  // get current date/time 
  x[0] = rtc.now().year();
  x[1] = rtc.now().month();
  x[2] = rtc.now().day();
  x[3] = rtc.now().hour();
  x[4] = rtc.now().minute();

  display.print(x[0]);
  display.print(F("-"));
  display.print(x[1]);
  display.print(F("-"));
  display.print(x[2]);
  display.setCursor(60, 0);
  display.print(x[3]);
  display.print(F(":"));
  display.print(x[4]);
  display.display();

  Serial.print(x[0]);
  Serial.print(F("-"));
  Serial.print(x[1]);
  Serial.print(F("-"));
  Serial.print(x[2]);
  Serial.print(F("  "));
  Serial.print(x[3]);
  Serial.print(F(":"));
  Serial.println(x[4]);

  // temperature sensor
  val[0] = dht.readTemperature();
  val[1] = dht.readHumidity();
  // float fT = dht.readTemperature(true);

  display.setCursor(0, 10);
  display.print(F("Air: "));  
  if (isnan(val[0]) || isnan(val[1])){
    display.print(F("DHT Failure"));   
    Serial.println(F("DHT Failure")); 
    display.display();
  }
  else{
    // temperature
    display.print(val[0], 1);    
    display.print(F("C"));
    // humidity
    display.setCursor(70, 10);    
    display.print(val[1], 1);
    display.print(F("%"));
    display.display(); 

    Serial.print(val[0], 1);
    Serial.println(F("C "));
    Serial.print(F("Humidity: "));
    Serial.print(val[1], 1);
    Serial.println(F("%"));      
  }

  // soil moisture sensor
  int moisture = readSensor();  
  display.setCursor(0, 20);
  display.print(F("Soil: "));
  display.setCursor(30, 20);
  
  // check read failure of Soil moisture sensor
  if (isnan(moisture)){    
    display.print(F("SM Err"));
    display.display();

    Serial.println(F("SM Err"));
  }
  float moisture_percent = map(moisture, 634, 321, 0, 100);
  int moisture_level = check_moisture(moisture);
  String moisture_category = return_moisture_category(moisture_level);    

  // print moisture levels    
  display.print(moisture_percent, 0);
  display.print(F("%"));
  display.setCursor(55, 20);
  display.print(moisture_category);  
  display.display();

  Serial.print(F("Soil Moisture: "));
  Serial.print(moisture);
  Serial.print(F(" "));
  Serial.print(moisture_category);
  Serial.print(F(" "));
  Serial.print(moisture_percent, 1);
  Serial.println(F("%"));

  delay(1000);  
}

int readSensor() {  
  // read analog value from sensor
  int val = analogRead(sensorPin);
  return val;
}

int check_moisture(int sensor_value)  {
  // check and return moisture level category
  int dryValue = 636; // moisture sensor dry value
  int wetValue = 321; // moisture sensor wet value
  int intervals = (dryValue - wetValue)/3;  // intervals very wet, wet, dry

  int moisture_level;

  if(sensor_value >= wetValue && sensor_value < (wetValue + intervals))
  {   
    moisture_level = 3;
  }
  else if(sensor_value > (wetValue + intervals) && sensor_value < (dryValue - intervals))
  {    
    moisture_level = 2;
  }
  else if(sensor_value <= dryValue && sensor_value > (dryValue - intervals))
  {
    moisture_level = 1;
  }

  return moisture_level;
}

String return_moisture_category(int moisture_level) {
  // return moisture category string based on moisture value
  String moisture_category;  

  if(moisture_level == 3)
  {
    moisture_category = "Very Wet";
  }
  else if(moisture_level == 2)
  {
    moisture_category = "Wet";
  }
  else if(moisture_level == 1)
  {    
    moisture_category = "Dry";
  }
  return moisture_category;
}
