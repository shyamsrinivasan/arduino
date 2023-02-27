#define sensorPin A0


const int dryValue = 634;
const int wetValue = 321;
int intervals = (dryValue - wetValue)/3;

void setup() {
  Serial.begin(9600);
}

void loop() {
  float moisture_percent;
  int moisture = readSensor();

  

  moisture_percent = map(moisture, 634, 321, 0, 100);
  Serial.print("Moisture Analog: ");
  Serial.println(moisture);  
  Serial.print("Moisture category: ");
  
  if(moisture > wetValue && moisture < (wetValue + intervals))
  {
    Serial.print("very wet\n");
  }
  else if(moisture > (wetValue + intervals) && moisture < (dryValue - intervals))
  {
    Serial.print("Wet\n");
  }
  else if(moisture < dryValue && moisture > (dryValue - intervals))
  {
    Serial.print("Dry\n");
  }
  Serial.print("Moisture Percentage: ");
  Serial.print(moisture_percent);
  Serial.println("%\n\n");
  delay(1000);

}

int readSensor() {
  delay(10);
  // read analog value from sensor
  int val = analogRead(sensorPin);
  return val;
}
