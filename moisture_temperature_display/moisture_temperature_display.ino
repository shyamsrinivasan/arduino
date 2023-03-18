#include <Wire.h>
#include <LCD_I2C.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <DS3231.h>
#include <SD.h>

#define sensorPin A0
#define DHTPIN 2
#define DHTTYPE DHT22
#define SLevelPressure 1008;  // sea level pressure in hPa
#define SDcsPIN 10  // for the data logging shield, we use digital pin 10 for the SD cs line


LCD_I2C lcd(0x27);  // default LCD I2C address
DHT dht(DHTPIN, DHTTYPE);  // initialize DHT sensor
Adafruit_BMP085 bmp; //Initialize BMP180
RTClib rtc; // initialize DS1307 clock

const int dryValue = 636; // moisture sensor dry value
const int wetValue = 321; // moisture sensor wet value
int intervals = (dryValue - wetValue)/3;  // intervals very wet, wet, dry
 
// data structure
struct DataStruct {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  float T;
  float humidity;
  float SeaLevelPressure;
  float P;
  float moisture_percent;
  String moisture;
};

DataStruct oldData = {2023, 01, 01, 00, 00, 00, 0.0, 0.0, 100.8, 100.8, 0.0, "Dry"};

// initialize SD card module
void Initialize_SDcard()  {
  // see if the card is present and can be initialized:
  if (!SD.begin(SDcsPIN)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    // dataFile.println("Year,Month,Date,Hour,Minute,Second,Temperature,Humidity,Heat Index"); //Write the first row of the excel file
    dataFile.println("Year\tMonth\tDate\tHour\tMinute\tSecond\tTemperature\tHumidity\tSea_Level_Pressure\tPressure\tSoil_Moisture_Percentage\tSoil_Moisture");
    dataFile.close();
  }
}

void Write2SDcard(DataStruct *data) {
  // open file
  File data_file = SD.open("LoggerCD.txt", FILE_WRITE);

  if (data_file)  {
    // wwrite data to file
    // time data
    data_file.print(data->year, DEC);
    data_file.print("\t");
    data_file.print(data->month, DEC);
    data_file.print("\t");
    data_file.print(data->day, DEC);
    data_file.print("\t");
    data_file.print(data->hour, DEC);
    data_file.print("\t");
    data_file.print(data->minute, DEC);
    data_file.print("\t");
    data_file.print(data->second, DEC);
    data_file.print("\t");
    // temperature and humidity
    data_file.print(data->T);
    data_file.print("/t");
    data_file.print(data->humidity);
    data_file.print("/t");
    // pressure
    data_file.print(data->SeaLevelPressure);
    data_file.print("/t");
    data_file.print(data->P);
    data_file.print("/t");
    // soil moisture
    data_file.print(data->moisture_percent);
    data_file.print("/t");
    data_file.println(data->moisture);

    data_file.close();
    Serial.println("Data written to file successfully");
  }
  else  {
    Serial.println("Write to File Unsuccessful");
  }

}

void setup() {
  Serial.begin(9600);
  dht.begin();
  bmp.begin();
  lcd.begin();
  lcd.backlight();
  Initialize_SDcard(); // initialize SD card
}

void loop() {  
  
  delay(3000);  // delay between measurements
  
  DataStruct data;  // intialize data struct  
  DateTime now = rtc.now(); // get current date/time

  // store time to data struct
  data.year = now.year();
  data.month = now.month();
  data.day = now.day();
  data.hour = now.hour();
  data.minute = now.minute();
  data.second = now.second();

  Serial.print("Date: ");
  Serial.print(data.year);
  Serial.print("-");
  Serial.print(data.month);
  Serial.print("-");
  Serial.print(data.day);
  Serial.print(" ");
  Serial.print(data.hour);
  Serial.print(":");
  Serial.print(data.minute);
  Serial.print(":");
  Serial.println(data.second);

  // temperature sensor
  Serial.print("Tempertature: ");
  float cT = dht.readTemperature();
  float humidity = dht.readHumidity();
  float fT = dht.readTemperature(true);
  // check read failure of DHT sensor
  if (isnan(cT) || isnan(fT) || isnan(humidity)){
    lcd.print(F("DHT Failure"));   
    Serial.println(F("DHT Failure")); 
  }
  else{
    lcd.print(cT, 1);
    lcd.print("C ");
    lcd.print(humidity, 1);
    lcd.print("%");

    Serial.print(cT, 1);
    Serial.println("C ");
    Serial.print("Humidity: ");
    Serial.print(humidity, 1);
    Serial.println("%");

    data.T = cT;
    data.humidity = humidity;
  }
  
  lcd.setCursor(0, 1);
  // pressure sensor - sensor reads in Pa
  float cP = bmp.readPressure()/1000;
  lcd.print(cP, 1);
  lcd.print("kPa ");  
  Serial.print("Pressure: ");
  Serial.print(cP);
  Serial.println("kPa");

  data.SeaLevelPressure = SLevelPressure;
  data.P = cP;
  
  // soil moisture sensor
  int moisture = readSensor();
  // check read failure of Soil moisture sensor
  if (isnan(moisture)){    
    lcd.print(F("SM Err"));
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
  lcd.print(moisture_category);  
  Serial.print("Soil Moisture: ");
  Serial.print(moisture);
  Serial.print(" ");
  Serial.print(moisture_category);
  Serial.print(" ");
  Serial.print(moisture_percent, 1);
  Serial.println("%");

  data.moisture_percent = moisture_percent;
  data.moisture = moisture_category;

  Write2SDcard(&data);
  
  delay(3000);
  lcd.clear();
}

int readSensor() {  
  // read analog value from sensor
  int val = analogRead(sensorPin);
  return val;
}
