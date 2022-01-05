#include <Wire.h>
#include <TimeLib.h>
#include <DS3231.h>
#include <DS1307RTC.h>
#include <DHT.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#define DHTPIN 2
#define DHTTYPE DHT22

//Initialize DS1307 clock
RTClib rtc;

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

/*To assign a unique ID, simply
   provide an appropriate value in the constructor below */
Adafruit_BMP085_Unified bmp; // = Adafruit_BMP085_Unified(10085); /*0x77*/

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

struct TPdata {
  int Dyear;
  int Dmonth;
  int Dday;
  int Dhour;
  int Dminute;
  int Dsec;
  float temp;
  float Rhumid;
  float Ftemp;
  float heatindex;
  float Spressure;
  float pressure;
  float altitude;
};

// base line pressure @ sea level for BMP180
//float seaLevelPressure = 1009;

// Initial global struct of TPdata type
TPdata Oldata = {2022, 01, 05, 00, 00, 00, 0.0, 0.0, 0.0, 0.0, 1012, 0.0, 0.0};

TPdata setPressure(TPdata *data)
{
  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  //Serial.println(data->Spressure);

  if (event.pressure)
  {
    data->pressure = event.pressure;
    data->altitude = bmp.pressureToAltitude(data->Spressure, event.pressure);
    return *data;
  }
  else
  {
    Serial.println("BMP180 Pressure Sensor read unsuccessful");
    return *data;
  }
}

TPdata setTemperature(TPdata *data)
{
  // read current date and time
  DateTime now = rtc.now();

  data->Dyear = now.year();
  data->Dmonth = now.month();
  data->Dday = now.day();
  data->Dhour = now.hour();
  data->Dminute = now.minute();
  data->Dsec = now.second();

  // read DHT22/DHT11 data
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float f = dht.readTemperature(true);
  float hi = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return *data;
  }
  data->temp = t;
  data->Rhumid = h;
  data->Ftemp = f;
  data->heatindex = hi;
  return *data;
}

// Initialize SD card module
void Initialize_SDcard()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
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
    dataFile.println("Year\tMonth\tDate\tHour\tMinute\tSecond\tTemperature\tHumidity\tHeat Index");
    dataFile.close();
  }
}

//Write data to SD card
void Write_SDcard()
{
    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);

  // read current date and time
  DateTime now = rtc.now();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  // if the file is available, write to it:
  if (dataFile) {
     //Store year on SD card     
    // ardprintf("%d-%d-%d", now.year(), now.month(), now.day());
    dataFile.print(now.year(), DEC);
    dataFile.print("\t"); //Move to next column using a ","

    //Store month on SD card
    dataFile.print(now.month(), DEC); 
    dataFile.print("\t"); 

    //Store date on SD card
    dataFile.print(now.day(), DEC); 
    dataFile.print("\t");

    //store hour on SD card
    //ardprintf("%d:%d:%d", now.hour(), now.minute(), now.second());
    dataFile.print(now.hour(), DEC); 
    dataFile.print("\t");
    //store minute on SD card
    dataFile.print(now.minute(), DEC);   
    dataFile.print("\t");
    //store second on SD card
    dataFile.print(now.second(), DEC);   
    dataFile.print("\t");    

    dataFile.print(t); //Store date on SD card
    dataFile.print("\t"); //Move to next column using a ","

    dataFile.print(h); //Store date on SD card
    dataFile.print("\t"); //Move to next column using a ","

    // Compute heat index in Celsius (isFahreheit = false)
    dataFile.print(dht.computeHeatIndex(t, h, false));  //Store date on SD card

    dataFile.println(); //End of Row move to next row
    dataFile.close(); //Close the file

    Serial.println("Data Write successful");
  }
  else {    
    Serial.println("OOPS!! SD card writing failed");
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(", ");    
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(",");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(",");
    Serial.print("RH: ");
    Serial.print(h);
    Serial.print("Sea level pressure:   ");
    Serial.print(seaLevelPressure);
    Serial.print(" hPa\n");
    Serial.print("Pressure:    ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");   
    Serial.print("Altitude:    "); 
    Serial.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure)); 
    Serial.println(" m");
    Serial.println();    
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  Initialize_SDcard();
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP180 detected ... Check your wiring or I2C ADDR!");
    /*lcd.print("Ooops! No BMP180");*/
    while(1);
  }   
  Wire.begin();
  // while (!Serial) ; // wait for serial
}

void loop() {  
  readDHT22();
  
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);  

  if (event.pressure)
  {
    float temperature;
    bmp.getTemperature(&temperature);
    float seaLevelPressure = 1009;    
  }
  
  /* Write data to SD cardd */
  Write_SDcard();
  delay(900000);    
}


// Read DHT sensor data
void readDHT22(){
  // Wait a few seconds between measurements.
  // delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  Serial.println("Sensor read successful");

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}


// Read BMP180 data
