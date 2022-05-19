#include <DHT.h>         // Definição da biblioteca DHT
#include <TFT_eSPI.h>    // Definição da biblioteca referente ao display
#include <SPI.h>         // Definição da biblioteca referente ao display
#include <Preferences.h> // Definição da biblioteca para armazenamento dos dados

Preferences preferences; // Criação da váriavel com o tipo referecia

// Definição de pino do DHT 11 e tipo de sensor
#define DHTPIN 17
#define DHTTYPE DHT11

// Definição dos botões utilizados
#define BUTTON1PIN 35
#define BUTTON2PIN 0

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

// uso no código
int variavel;

bool useFahrenheit = true;    // Default to Fahrenheit
bool showTemp = true;         // Default to Temp / Humidity
long lastDebounceButton1 = 0; // Holds Button1 last debounce
long lastDebounceButton2 = 0; // Holds Button2 last debounce
long debounceDelay = 200;     // 200ms between re-polling

// INTRPT Function to execute when Button 1 is Pushed
void IRAM_ATTR toggleButton1()
{
  if ((millis() - lastDebounceButton1) > debounceDelay)
  {
    if (useFahrenheit)
    {
      useFahrenheit = false;
    }
    else
    {
      useFahrenheit = true;
    }
    lastDebounceButton1 = millis();
  }
}

// INTRPT Function to execute when Button 2 is Pushed
void IRAM_ATTR toggleButton2()
{
  if ((millis() - lastDebounceButton2) > debounceDelay)
  {
    if (showTemp)
    {
      showTemp = false;
    }
    else
    {
      showTemp = true;
    }
    lastDebounceButton2 = millis();
  }
};

// What to display if showTemp = true
void showScrn1()
{
  float t = dht.readTemperature(useFahrenheit);
  float h = dht.readHumidity();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(0, 30);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.println("Temp     Humidity");
  tft.drawLine(0, 35, 250, 35, TFT_BLUE);
  tft.setCursor(0, 60);
  tft.print(t);
  if (useFahrenheit)
  {
    tft.print(F("f"));
  }
  else
  {
    tft.print(F("c"));
  }
  tft.setCursor(130, 60);
  tft.print(h);
  tft.print(F("%"));
};

// What to display if showTemp = false
void showScrn2()
{
  float t = dht.readTemperature(useFahrenheit);
  float h = dht.readHumidity();
  float hi = dht.computeHeatIndex(t, h, useFahrenheit);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(50, 30);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.println("Show Temp");
  tft.drawLine(0, 35, 250, 35, TFT_BLUE);
  tft.setFreeFont(&Orbitron_Light_32);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(60, 100);
  tft.print(preferences.getFloat("maxtemp"));
  if (useFahrenheit)
  {
    tft.print(F("f"));
  }
  else
  {
    tft.print(F("c"));
  }
};

void MudaPreference(int variavel, float NovoValor)
{
  switch (variavel)
  {
  case 0:
    preferences.putUInt("maxtemp", NovoValor);
    break;
  case 1:
    preferences.putUInt("mintemp", NovoValor);
    break;

  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON1PIN, INPUT);
  pinMode(BUTTON2PIN, INPUT);
  attachInterrupt(BUTTON1PIN, toggleButton1, FALLING);
  attachInterrupt(BUTTON2PIN, toggleButton2, FALLING);
  preferences.begin("mypreference", false);
  dht.begin();
  tft.begin();
  tft.setRotation(1); // Landscape

  float maxtemp = preferences.getFloat("maxtemp", 50.00);
  float mintemp = preferences.getFloat("mintemp", 10.00);
  Serial.printf("%.1f", maxtemp);
  Serial.printf("%.1f", mintemp);
  preferences.end();
};

void loop()
{
  // Serial.printf("Valor maxima: %.2f\n", preferences.getFloat("maxtemp", 0));
  // Serial.printf("Valor minima: %.2f\n", preferences.getFloat("mintemp", 0));
  delay(1000); // Required by this device to function properly
  if (showTemp)
  {
    showScrn1();
  } // Temp Humidity
  else
  {
    showScrn2();
  } // Heat Index
};