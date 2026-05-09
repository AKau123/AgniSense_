#include "BluetoothSerial.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

BluetoothSerial SerialBT;

/* -------- TFT PINS -------- */
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/* -------- LED PINS -------- */
#define LED_GREEN  14
#define LED_YELLOW 12
#define LED_RED    13

/* -------- BUZZER -------- */
#define BUZZER 27

String receivedData = "";

/* -------- Parsed values -------- */
String id="", mode="", msg="", state="";
String status="";

/* -------- CLEAN FUNCTION -------- */
String clean(String s)
{
  s.trim();
  return s;
}

/* -------- FUNCTION TO EXTRACT DATA -------- */
String getValue(String data, String key)
{
  int start = data.indexOf(key);
  if(start == -1) return "";

  start += key.length();
  int end = data.indexOf(';', start);

  if(end == -1) end = data.length();

  return data.substring(start, end);
}

/* -------- BUZZER FUNCTIONS -------- */
void slowBeep()
{
  for(int i = 0; i < 2; i++)
  {
    tone(BUZZER, 1500);
    delay(120);
    noTone(BUZZER);
    delay(120);
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  /* -------- TFT INIT -------- */
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);   // better UI

  tft.setCursor(10, 50);
  tft.println("Starting...");

  /* -------- BLUETOOTH -------- */
  SerialBT.begin("ESP32_MASTER", true);
  SerialBT.setPin("1234", 4);

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 50);
  tft.println("Connecting...");

  if (SerialBT.connect("HC-05"))
  {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 50);
    tft.println("Connected!");
    delay(1000);
    tft.fillScreen(ST77XX_BLACK);
  }
  else
  {
    tft.setCursor(10, 50);
    tft.println("BT Failed!");
    while (1);
  }
}

void loop()
{
  while (SerialBT.available())
  {
    char c = SerialBT.read();

    if (c == '\n')
    {
      /* -------- PARSE DATA -------- */
      id = clean(getValue(receivedData, "ID:"));
      mode = clean(getValue(receivedData, "MODE:"));
      msg = clean(getValue(receivedData, "MSG:"));
      state = clean(getValue(receivedData, "STATE:"));

      mode.toUpperCase();
      msg.toUpperCase();
      state.toUpperCase();

      /* -------- DISPLAY -------- */
      tft.fillScreen(ST77XX_BLACK);

      tft.setCursor(0, 10);
      tft.println("ID: " + id);
      tft.println("MODE: " + mode);

      if(mode == "EMERGENCY")
      {
        tft.println("ALERT:");
        tft.println(msg);
      }
      else
      {
        tft.println("STATE: " + state);
      }

      /* -------- LED + BUZZER LOGIC -------- */
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);

      if(mode == "NORMAL")
      {
        if(state.indexOf("CLEAR") != -1)
        {
          digitalWrite(LED_GREEN, HIGH);
          noTone(BUZZER);
        }
        else if(state.indexOf("TRAFFIC") != -1)
        {
          digitalWrite(LED_YELLOW, HIGH);
          slowBeep();
        }
        else if(state.indexOf("OBSTACLE") != -1)
        {
          digitalWrite(LED_RED, HIGH);

          for(int i=0;i<3;i++)
          {
            tone(BUZZER, 1800);
            delay(80);
            noTone(BUZZER);
            delay(80);
          }
        }
      }
      else if(mode == "EMERGENCY")
      {
        // 🔴 CLEAR THE WAY
        if(msg.indexOf("CLEAR") != -1)
        {
          digitalWrite(LED_RED, HIGH);
          tone(BUZZER, 2500);
        }

        // 🟡 APPROACHING
        else if(msg.indexOf("APPROACH") != -1 || msg.indexOf("ALERT") != -1)
        {
          digitalWrite(LED_YELLOW, HIGH);

          tone(BUZZER, 1500);
          delay(200);
          noTone(BUZZER);
          delay(300);
        }

        // 🟢 FAR AWAY
        else
        {
          digitalWrite(LED_GREEN, HIGH);
          noTone(BUZZER);
        }
      }

      /* -------- DEBUG -------- */
      Serial.println("MODE=[" + mode + "] MSG=[" + msg + "] STATE=[" + state + "]");

      receivedData = "";
    }
    else
    {
      receivedData += c;
    }
  }
}
