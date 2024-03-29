#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <RTClib.h>
// #include <Adafruit_BMP280.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define DHTPIN 4  // DHT digital PIN
#define DHTTYPE DHT22 // DHT sensor type DHT11 or DHT22
#define sensorPin A0  // sensor pin for moisture sensor

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // initialize OLED display
DHT dht(DHTPIN, DHTTYPE);  // initialize DHT sensor
RTC_DS3231 rtc;   // DS3231 RTC initialization
// Adafruit_BMP280 bmp; //Initialize BMP180/BMP280 I2C

// buffer for DateTime.tostr
char buf[20];


void setup() {
  Serial.begin(9600);
  dht.begin();
  rtc.begin();
  // unsigned status;  
  // status = bmp.begin();  // bmp initialization
  // if(!status)  {
  //   Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
  //                     "try a different address!"));
  //   Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
  //   while(1);
  // }

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
  float val[5]; 
  
  // temperature sensor
  val[0] = dht.readTemperature();
  val[1] = dht.readHumidity();
  // float fT = dht.readTemperature(true);
  // BMP180/BMP280 in kPa (sensor provides data in Pa)
  // val[2] = bmp.readPressure()/1000;
  // val[3] = bmp.readTemperature();
  // val[4] = bmp.readAltitude(1008.1);

    // display.setCursor(0, 10);
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
    display.setCursor(70, 0);    
    display.print(val[1], 1);
    display.print(F("%"));
    display.display(); 

    Serial.print(val[0], 1);
    Serial.println(F("C "));
    Serial.print(F("Humidity: "));
    Serial.print(val[1], 1);
    Serial.println(F("%"));      
  }

  display.setCursor(0, 10);
  display.print(rtc.now().year()); 
  display.print(F("-"));
  display.print(rtc.now().month());
  display.print(F("-"));
  display.print(rtc.now().day());
  display.print(F(" "));
  display.print(rtc.now().hour());
  display.print(F(":"));
  display.print(rtc.now().minute());
  display.print(F(":"));
  display.print(rtc.now().second()); 

  Serial.print(rtc.now().year());
  Serial.print(F("-"));
  Serial.print(rtc.now().month());
  Serial.print(F("-"));
  Serial.print(rtc.now().day());
  Serial.print(F("  "));
  Serial.print(rtc.now().hour());
  Serial.print(F(":"));
  Serial.print(rtc.now().minute());
  Serial.print(F(":"));
  Serial.println(rtc.now().second()); 

  // if (isnan(val[2]) || isnan(val[3]) || isnan(val[4]))  {
  //   display.print(F("BMP Failure"));   
  //   Serial.println(F("BMP Failure")); 
  //   display.display();
  // }
  // else  {
  //   // pressure
  //   display.print(val[2], 0);    
  //   display.print(F("kPa"));
  //   // temperature
  //   display.setCursor(70, 10);    
  //   display.print(val[3], 1);    
  //   display.display(); 
  //   // altitude
  //   display.setCursor(90, 10);    
  //   display.print(val[4], 1); 
  //   display.print(F("m"));   
  //   display.display(); 

  //   Serial.print(F("Pressure: "));
  //   Serial.print(val[2], 1);
  //   Serial.println(F("kPa "));
  //   Serial.print(F("Temperature: "));
  //   Serial.print(val[3], 1);
  //   Serial.print(F("C"));      
  //   Serial.print(F("Altitude: "));
  //   Serial.print(val[4], 1);
  //   Serial.println(F("m"));  
  // }

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
  display.setCursor(57, 20);
  display.print(moisture_category);  
  display.display();

  Serial.print(F("Soil Moisture: "));
  Serial.print(moisture);
  Serial.print(F(" "));
  Serial.print(moisture_category);
  Serial.print(F(" "));
  Serial.print(moisture_percent, 1);
  Serial.println(F("%"));

  delay(5000);  
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
