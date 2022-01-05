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

struct TPdata{  
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

void setup() 
{
  Serial.begin(9600);
  dht.begin();
  Wire.begin();   

  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP180 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }  
}

void loop() {  
  
  TPdata *dataptr = &Oldata;
  TPdata Finaldata = setTemperature(dataptr); 
  TPdata *dataptr2 = &Finaldata;
  Finaldata = setPressure(dataptr2);  

  Serial.print(Finaldata.Dyear, DEC);
  Serial.print("-");
  Serial.print(Finaldata.Dmonth, DEC); 
  Serial.print("-");
  Serial.print(Finaldata.Dday, DEC); 
  Serial.print("\t");

  Serial.print(Finaldata.Dhour, DEC);
  Serial.print(":");
  Serial.print(Finaldata.Dminute, DEC);
  Serial.print(":");
  Serial.print(Finaldata.Dsec, DEC);
  Serial.print("\t");
   
  Serial.print("Temperature: ");
  Serial.print(Finaldata.temp);  
  Serial.print("C, ");
  Serial.print("RH: ");
  Serial.print(Finaldata.Rhumid); 
  Serial.print("% ");
  Serial.print("Heat Index: ");
  Serial.print(Finaldata.heatindex);
  Serial.print("C, ");
  Serial.print("Sea level Pressure: ");
  Serial.print(Finaldata.Spressure);
  Serial.print("hPa, ");
  Serial.print("Current Pressure: ");
  Serial.print(Finaldata.pressure);
  Serial.print("hPa, ");
  Serial.print("Altitude: ");
  Serial.print(Finaldata.altitude);
  Serial.print("m");
  Serial.println(); 

  delay(3000);
  //delay(1800000); // 30 minutes between measurements
}

/*// Read DHT sensor data
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
}*/
