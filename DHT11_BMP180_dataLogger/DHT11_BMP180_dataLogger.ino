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
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085); /*0x77*/

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
    dataFile.println("Year\tMonth\tDate\tHour\tMinute\tSecond\tTemperature\tHumidity\tHeat Index\tSea Level Pressure\tPressure\tAltitude");
    dataFile.close();
  }
}

//Write data to SD card
void Write_SDcard(TPdata *data)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);

  // if the file is available, write to it
  if (dataFile)
  {
    // Write date and time to file from data
    dataFile.print(data->Dyear, DEC);
    dataFile.print("\t");
    dataFile.print(data->Dmonth, DEC);
    dataFile.print("\t");
    dataFile.print(data->Dday, DEC);
    dataFile.print("\t");

    dataFile.print(data->Dhour, DEC);
    dataFile.print("\t");
    dataFile.print(data->Dminute, DEC);
    dataFile.print("\t");
    dataFile.print(data->Dsec, DEC);
    dataFile.print("\t");

    //Write Temperature and Pressure data to file
    dataFile.print(data->temp);
    dataFile.print("\t");
    dataFile.print(data->Rhumid);
    dataFile.print("\t");
    dataFile.print(data->heatindex);
    dataFile.print("\t");
    dataFile.print(data->Spressure);
    dataFile.print("\t");
    dataFile.print(data->pressure);
    dataFile.print("\t");
    dataFile.print(data->altitude);
    dataFile.println();

    dataFile.close();
    Serial.println("Write to File Successful");
  }
  else
  {
    Serial.println("Write to File Unsuccessful");

    Serial.print(data->Dyear, DEC);
    Serial.print("-");
    Serial.print(data->Dmonth, DEC);
    Serial.print("-");
    Serial.print(data->Dday, DEC);
    Serial.print("\t");

    Serial.print(data->Dhour, DEC);
    Serial.print(":");
    Serial.print(data->Dminute, DEC);
    Serial.print(":");
    Serial.print(data->Dsec, DEC);
    Serial.print("\t");

    Serial.print("Temperature: ");
    Serial.print(data->temp);
    Serial.print("C, ");
    Serial.print("RH: ");
    Serial.print(data->Rhumid);
    Serial.print("% ");
    Serial.print("Heat Index: ");
    Serial.print(data->heatindex);
    Serial.print("C, ");
    Serial.print("Sea level Pressure: ");
    Serial.print(data->Spressure);
    Serial.print("hPa, ");
    Serial.print("Current Pressure: ");
    Serial.print(data->pressure);
    Serial.print("hPa, ");
    Serial.print("Altitude: ");
    Serial.print(data->altitude);
    Serial.print("m");
    Serial.println();
  }
}

void setup()
{
  Serial.begin(9600);
  dht.begin();

  //Initialise the sensor
  if (!bmp.begin())
  {
    //There was a problem detecting the BMP085 ... check your connections
    Serial.print("Ooops, no BMP180 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  // Initialize SD card
  Initialize_SDcard();
  Wire.begin();
}

void loop() {

  // Collect sensor data in struct
  TPdata *dataptr = &Oldata;
  TPdata Finaldata = setTemperature(dataptr);
  TPdata *dataptr2 = &Finaldata;
  Finaldata = setPressure(dataptr2);

  // Write data to file
  Write_SDcard(dataptr2);  

  //delay(3000);
  delay(1800000); // 30 minutes between measurements
}
