#include <Blynk.h>



// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPxxx-xxx-xxx"
#define BLYNK_DEVICE_NAME "Smart IoT"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG


//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

//#include "BlynkEdgent.h"
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)

#define DHTTYPE DHT22     // DHT 22
#define DHTPIN D4 //DHT22 Pin D4(GPIO 2)


DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float voltage;
int bat_percentage;
int analogInPin  = A0;    // Analog input pin
int sensorValue;
float calibration = 0.40; // Check Battery voltage using multimeter & add/subtract the value



void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.clearDisplay();
  display.setTextColor(WHITE);

  delay(100);

}

void loop() {
  BlynkEdgent.run();
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  sensorValue = analogRead(analogInPin);
  voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor

  bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage

  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  
      //send data to blynk
    Blynk.virtualWrite(V1, t); //for Temperature
    Blynk.virtualWrite(V2, h); //for Humidity
    Blynk.virtualWrite(V3, voltage);  // for battery voltage
    Blynk.virtualWrite(V4, bat_percentage);  // for battery percentage
  
    //Print data on serial monitor
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  Serial.print("Analog Value = ");
  Serial.println(sensorValue);
  Serial.print("Output Voltage = ");
  Serial.println(voltage);
  Serial.print("Battery Percentage = ");
  Serial.println(bat_percentage);

  Serial.println();
  Serial.println("****************************");
  Serial.println();
  delay(1000);
 
       if (bat_percentage <=30)
    {
      Serial.println("Battery level below 30%, Charge battery on time");
      //send notification
      Blynk.logEvent("battery_low", "Battery is getting low.... Plugin to charge") ;
      delay(500);
    }

  // display temperature on OLED
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  // display humidity on OLED
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(" %");

  display.display();
  delay(1500);
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
