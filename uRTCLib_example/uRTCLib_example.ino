#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  rtc.begin();
}

void loop() {  
	Serial.print("RTC DateTime: ");

	Serial.print(rtc.now().year());
	Serial.print('/');
	Serial.print(rtc.now().month());
	Serial.print('/');
	Serial.print(rtc.now().day());

	Serial.print(' ');

	Serial.print(rtc.now().hour());
	Serial.print(':');
	Serial.print(rtc.now().minute());
	Serial.print(':');
	Serial.print(rtc.now().second());

	// Serial.print(" DOW: ");
	// Serial.print(rtc.dayOfWeek());

	// Serial.print(" - Temp: ");
	// Serial.print((float) rtc.temp() / 100);

  Serial.println();

  delay(1000);

}
