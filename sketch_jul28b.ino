#include <MQ135.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* MQ135 + DHT Temp Sensor

   Combination of the MQ135 air quality sensor and a DHT11/22 temperature sensor to accurately measure ppm values through the library correction.
   Uses the Adafruit DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library

   Written by: https://github.com/NuclearPhoenixx/MQ135
*/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   21
#define OLED_CLK   22
#define OLED_DC    14
#define OLED_CS    12
#define OLED_RESET 13
#define PIN_MQ135 35   // MQ135 Analog Input Pin
#define DHTPIN 4       // DHT Digital Input Pin
#define DHTTYPE DHT11  // DHT11 or DHT22, depends on your sensor

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
MQ135 mq135_sensor(PIN_MQ135);
DHT dht(DHTPIN, DHTTYPE);

float temperature, humidity;  // Temp and Humid floats, will be measured by the DHT


void wait_mq135 ()
{
  int cont=19;
  do
  {
    display.setCursor(0,0);
    display.clearDisplay();
    display.println("Avvio...");
    display.println(cont);
    display.display();
    cont--;
    delay(1000);
  }while (cont!=0);
}



void setup() {
  Serial.begin(115200);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setTextSize(2); 
  wait_mq135();       
  dht.begin();
  analogReadResolution(10);
}

void loop() {
  
  display.clearDisplay();
  display.setCursor(0,0);
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
  float Rs_R0 = resistance / correctedRZero;

  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Analog Read MQ135: ");
  Serial.print(analogRead(35));
  Serial.print("\t Rs/R0: ");
  Serial.print(Rs_R0);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("ppm");
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  display.println("Rs/R0: ");
  display.println(Rs_R0);
  display.println("PPm: ");
  display.println(correctedPPM);
  display.display();
  delay(300);
}