#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_SHT4x.h>

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

//SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

double tempC = 0;
double humi = 0;
double battV = 0;

unsigned long previousMillis = 0; 
const long interval = 1000; //set how often you want to read and display sensor readings (in milliseconds)

void setup() 
{
  //Register the Particle variables that you can access over the cloud
  Particle.variable("temp", tempC); //variable to hold temperature measurement
  Particle.variable("humi", humi);  //variable to hold the humidity measurement
  Particle.variable("batt", battV); //variable to hold the battery voltage measurement

  pinMode(S5, INPUT_PULLUP);
  pinMode(D7, OUTPUT);
  Serial.begin(115200);

  // Call function to initialize the sensor
  initSHT40();

  // Initialize the OLED screen
  display.begin(0x3C, true); // Address 0x3C default
  display.clearDisplay();
  display.display();
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE, SH110X_BLACK);
  display.setCursor(0, 0);

  // Call function to draw the console on the OLED screen
  drawConsole();
}

void loop() 
{
  unsigned long currentMillis = millis();

  //update the console once per set interval
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    updateConsole();
  }
}

//Draw the console on the OLED screen
void drawConsole(void)
{
  //Draw the console table
  display.drawLine(42,0,42,44,SH110X_WHITE);
  display.drawLine(84,0,84,44,SH110X_WHITE);
  display.drawLine(0,14,128,14,SH110X_WHITE);
  display.drawLine(0,44,128,44,SH110X_WHITE);
    
  display.setCursor(10,4); display.print("Temp");
  display.setCursor(54,4); display.print("Humi");
  display.setCursor(96,4); display.print("Batt ");
  display.display();

}

//Update the console with latest values
void updateConsole(void)
{
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);

  tempC = temp.temperature;
  humi = humidity.relative_humidity;
  //LiPo positive terminal is connected to the A6 pin aka VBAT_MEAS
  //This is a special 5V tolerant pin
  //The ADC measurement is referenced to 5V instead of 3.3V
  battV = (analogRead(A6)*5.0)/4095;//LiPo positive terminal is connected to the A6 pin

  //Display a message on the bottom
  //You can choose to use this space to display any text or information you want
  display.setCursor(0,50); display.print("  Particle Photon 2");

  //Read and display the temperature
  display.setCursor(6,20); display.print(tempC);
  display.setCursor(20,32); display.print("C");

  //Read and display the relative humidity
  display.setCursor(50,20); display.print(humi);
  display.setCursor(56,32); display.print("%rH");

  //Read and display the battery voltage
  display.setCursor(92,20); display.print(battV);
  //if (digitalRead(S5)) display.print(" "); else display.print("*");
  display.setCursor(102,32); display.print("V");
  display.display();

}

//Function to initialize the sensor
void initSHT40(void)
{
  Serial.println("Adafruit SHT4x test");
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater()) {
     case SHT4X_NO_HEATER: 
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
  }
}