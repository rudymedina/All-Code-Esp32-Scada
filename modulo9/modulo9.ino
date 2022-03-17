#include <WiFi.h>
#include <Arduino.h>
#include "MHZ19.h"
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial or non-uno library compatable device
#include "DHTesp.h"
#include "time.h"
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "esp_system.h"
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
//--------------------------------------------|------|-----------|-------------------
String filenumberUpdate = "ESP32_Stas_Svetovaya_Dev_0v1.ino.esp32.bin"; //que significa esto?
//--------------------------------------------|------|-----------|-------------------
/////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------|------|--
String UpdateServer = "http://fynjy8.beget.tech/ESP32/DarMal/Stasovoy/Svetovaya/"; //que significa esto?
//-------------------------------------------------------------------------|------|--
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------|---------------------------------------------------
const String  MestoNazvanie = "Modul9" ;
const String UstroystvoNazvanie = "Dev_0";
const String Upd = UpdateServer + filenumberUpdate;
//-------------------------------|---------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////
String GeoNazvanie = "K17";
String loadChanel = String("DarMal/") + String(GeoNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie);// + String('/') + String(ZamerNazvanie);

const char *ssid_0 =  "Fresh";  // Имя вайфай точки доступа
const char *password_0 =  "OKM690wsx"; // Пароль от точки доступа

const char *ssid_1 =  "initial D"; // Имя вайфай точки доступа 
const char *password_1 =  "belgorod25"; // Пароль от точки доступа  //

const char* mqttServer = "scada.darym.ru";// "http://scada.darym.ru";  //"45.67.57.161";//"31.31.202.123";
const int mqttPort = 1883;
const char* mqttUser = "agromon";
const char* mqttPassword = "CsSrK5kL";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;
int delayTme = 2222;

const int wdtTimeout = 5 * 60 * 1000; //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}
#define ONE_WIRE_BUS 4 //4//
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress termometr1 = { 0x28, 0xD, 0x85, 0x67, 0xD, 0x0, 0x0, 0x4B };//15
DeviceAddress termometr2 = { 0x28, 0x3B, 0x3E, 0x67, 0xD, 0x0, 0x0, 0x2B }; //16
//DeviceAddress termometr1 = { 0x28, 0x8D, 0x81, 0x7, 0xD6, 0x1, 0x3C, 0x2F };//15
//DeviceAddress termometr2 = { 0x28, 0x6, 0x47, 0x79, 0x97, 0x15, 0x3, 0x8C }; //16
//DeviceAddress termometr3 = { 0x28, 0xAA, 0x98, 0xFE, 0x45, 0x14, 0x1, 0xED }; //28 AA 98 FE 45 14 1 ED termometer ID3
/*DeviceAddress termometr4;
  DeviceAddress termometr5;*/
//=================================================================================
#define BAUDRATE 9600
//=================================================================================
#define RX_PIN_0 23                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN_0 5                                          // Tx pin which the MHZ19 Rx pin is attached to
MHZ19 myMHZ19_0;                                             // Constructor for MH-Z19 class
SoftwareSerial mySerial_0(RX_PIN_0, TX_PIN_0);

int dhtPin_0 = 26;
int dhtPin_1 = 27;
DHTesp dht_0;
DHTesp dht_1;
String CO2_0, Tco2_0, CO2_1, Tco2_1, CO2_2, Tco2_2, Tdht22_0, Hdht22_0;
String CO2_3, Tco2_3, CO2_4, Tco2_4, CO2_5, Tco2_5, Tdht22_1, Hdht22_1;
WiFiClient espClient;
PubSubClient client(espClient);
WiFiMulti WiFiMulti;
#define Debug 1 // 1 - включён, 0 выключен. Отключение серийных портов для финальной прошивки
void setup() {
#if (Debug == 1)
  Serial.begin(115200);
  while (!Serial) {}
#endif
  //===== upd =====
  WiFi.mode(WIFI_STA);
  Serial.println("HTTP_UPDATE...");
  delay(5000);
  WiFiMulti.addAP(ssid_0, password_0);
  while ((WiFiMulti.run() == !WL_CONNECTED)) {
#if (Debug == 1)
    Serial.println("wi-fi...");
#endif
    delay(1000);
  }
  if ((WiFiMulti.run() == WL_CONNECTED)) {
#if (Debug == 1)
    Serial.println("wf-started");
#endif
    WiFiClient client;
    t_httpUpdate_return ret = httpUpdate.update(client, "http://fynjy8.beget.tech/ESP32/DarMal/Stas/Dev_0/ESP32.ino.esp32.bin");
    //t_httpUpdate_return ret = httpUpdate.update(client, UpdateServer);
#if (Debug == 1)
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
#endif
    ///////////////////////////////////////////
    WiFi.mode(WIFI_OFF);
    timer = timerBegin(0, 80, true);                  //timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);  //attach callback
    timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
    timerAlarmEnable(timer);                          //enable interrupt



    dht_0.setup(dhtPin_0, DHTesp::DHT11);
    dht_1.setup(dhtPin_1, DHTesp::DHT11);

    sensors.begin();
    sensors.setResolution(termometr1, TEMPERATURE_PRECISION);
    sensors.setResolution(termometr2, TEMPERATURE_PRECISION);
    //sensors.setResolution(termometr3, TEMPERATURE_PRECISION);
    /*sensors.setResolution(termometr4, TEMPERATURE_PRECISION);
      sensors.setResolution(termometr5, TEMPERATURE_PRECISION);*/

  }
}
void loop() {
  ////////////////////////////////////////////
  // запускаем прогрев датчика СО2 и делаем замер
  //Serial.println("1-1");
  //------------------------------------------------------------------------------------------
  //digitalWrite(powerPin, HIGH); //включаем датчик
  mySerial_0.begin(BAUDRATE);                               // Uno example: Begin Stream with MHZ19 baudrate
  myMHZ19_0.begin(mySerial_0);
  //Serial.println("0q");// *Important, Pass your Stream reference
  //------------------------------------------------------------------------------------------
  for ( byte i = 0; i <= 1; i++) { //гоняем датчик 4минуты замеров
    CO2_0 = String( myMHZ19_0.getCO2() );
    Tco2_0 = String( myMHZ19_0.getTemperature() );
    delay(delayTme);
    //Serial.println(Tco2_0);
    //Serial.println(CO2_0);
  }
  //====================================================================================
  //====================================================================================
  ////////////////////////////////////////////
  //замер DS18b20

  sensors.requestTemperatures();
  delay(11);
  float Tds18_0 = sensors.getTempC(termometr1);
  delay(11);
  float Tds18_1 = sensors.getTempC(termometr2);
  delay(11);
  //float Tds18_2 = sensors.getTempC(termometr3);
  //delay(11);
  /*float Tds18_3 = sensors.getTempC(termometr4);
    delay(11);
    float Tds18_4 = sensors.getTempC(termometr5);
    delay(11);*/
#if (Debug == 1)
  Serial.print("ds18b20-1: ");
  Serial.println(Tds18_0);
  Serial.print("ds18b20-2: ");
  Serial.println(Tds18_1);
  //Serial.print("ds18b20-3: ");
  //Serial.println(Tds18_2);
  /* Serial.print("ds18b20-4: ");
    Serial.println(Tds18_3);
    Serial.print("ds18b20-5: ");
    Serial.println(Tds18_4);*/
#endif
  ////////////////////////////////////////////
  //замер DHT22
  TempAndHumidity newValues = dht_0.getTempAndHumidity();
  Tdht22_0 = String(newValues.temperature);
  delay(1);
  Hdht22_0 = String(newValues.humidity);
  delay(1);
#if (Debug == 1)
  Serial.print("dhtPin_0 = "); Serial.println(dhtPin_0);
  Serial.println(Tdht22_0);
  Serial.println(Hdht22_0);
#endif
  //замер DHT22
  newValues = dht_1.getTempAndHumidity();
  delay(1);
  Tdht22_1 = String(newValues.temperature);
  delay(1);
  Hdht22_1 = String(newValues.humidity);
#if (Debug == 1)
  Serial.print("dhtPin_1 = "); Serial.println(dhtPin_1);
  Serial.println(Tdht22_1);
  Serial.println(Hdht22_1);
#endif
  //digitalWrite(powerPin, LOW); //вЫключаем датчик
  delay(3333);
  ////////////////////////////////////////////
  // Включаем вайфай, проверка обновлений прошивки, получаем время сети
  WIFI();
  struct tm timeinfo;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime(&timeinfo);
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  timeinfo.tm_year = timeinfo.tm_year - 100 + 2000;// timeinfo.tm_isdst
  //Serial.print("timeinfo.tm_year = "); Serial.println(timeinfo.tm_year);
  /////////////////////////////////////////////////
  // MQTT
  int loadChanel_len = loadChanel.length() + 1;
  // Prepare the character array (the buffer)
  char loadChanel_char_array[loadChanel_len];
  // Copy it over
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);

  client.setServer(mqttServer, mqttPort);
  //client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect(loadChanel_char_array, mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

  ////////////////////////////////////////////
  // формирование строк и MQTT отправка
  //<value,yyyy-mm-ddThh:mm:ss+0700>
  //: "60,2020-08-31T14:55:32+0700"
  //==============================================================================================
  String ZamerNazvanie = "CO2_0";
  String loadData = CO2_0 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
  String loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  int loadData_len = loadData.length() + 1;
  char loadData_char_array[loadData_len];
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  //Serial.println(timeinfo.tm_year);
  delay(5555);
  //===============================================================================================
  ZamerNazvanie = "Tdht22_0";
  //struct tm timeinfo;
  loadData = Tdht22_0 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
  Serial.println("loadData");

  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  // Prepare the character array (the buffer)
  loadData_char_array[loadData_len];
  // Copy it over
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  // Prepare the character array (the buffer)
  loadChanel_char_array[loadChanel_len];
  // Copy it over
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  delay(5555);
  //==============================================================================================
  ZamerNazvanie = "Hdht22_0";
  //struct tm timeinfo;
  loadData = Hdht22_0 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  // Prepare the character array (the buffer)
  loadData_char_array[loadData_len];
  // Copy it over
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  // Prepare the character array (the buffer)
  loadChanel_char_array[loadChanel_len];
  // Copy it over
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  delay(5555);
  //==============================================================================================
  //===============================================================================================
  ZamerNazvanie = "Tdht22_1";
  //struct tm timeinfo;
  loadData = Tdht22_1 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  // Prepare the character array (the buffer)
  loadData_char_array[loadData_len];
  // Copy it over
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  // Prepare the character array (the buffer)
  loadChanel_char_array[loadChanel_len];
  // Copy it over
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  delay(5555);
  //==============================================================================================
  ZamerNazvanie = "Hdht22_1";
  //struct tm timeinfo;
  loadData = Hdht22_1 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  // Prepare the character array (the buffer)
  loadData_char_array[loadData_len];
  // Copy it over
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  // Prepare the character array (the buffer)
  loadChanel_char_array[loadChanel_len];
  // Copy it over
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  delay(5555);
  //==============================================================================================
  //==============================================================================================
  if (Tds18_0 > (-55)) {
    ZamerNazvanie = "Tds18_0";
    //struct tm timeinfo;
    loadData = Tds18_0 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
    loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
    //---------------------------------------------------------------------------------------------
    loadData_len = loadData.length() + 1;
    // Prepare the character array (the buffer)
    loadData_char_array[loadData_len];
    // Copy it over
    loadData.toCharArray(loadData_char_array, loadData_len);
    //---------------------------------------------------------------------------------------------
    loadChanel_len = loadChanel.length() + 1;
    // Prepare the character array (the buffer)
    loadChanel_char_array[loadChanel_len];
    // Copy it over
    loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
    //---------------------------------------------------------------------------------------------
    client.publish(loadChanel_char_array, loadData_char_array);
    delay(5555);
  }
  //==============================================================================================
  if (Tds18_1 > (-55)) {
    ZamerNazvanie = "Tds18_1";
    //struct tm timeinfo;
    loadData = Tds18_1 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
    loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
    //---------------------------------------------------------------------------------------------
    loadData_len = loadData.length() + 1;
    // Prepare the character array (the buffer)
    loadData_char_array[loadData_len];
    // Copy it over
    loadData.toCharArray(loadData_char_array, loadData_len);
    //---------------------------------------------------------------------------------------------
    loadChanel_len = loadChanel.length() + 1;
    // Prepare the character array (the buffer)
    loadChanel_char_array[loadChanel_len];
    // Copy it over
    loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
    //---------------------------------------------------------------------------------------------
    client.publish(loadChanel_char_array, loadData_char_array);
    delay(5555);
  }
  //==============================================================================================
  /*if (Tds18_2 > (-55)) {
    ZamerNazvanie = "Tds18_2";
    //struct tm timeinfo;
    loadData = Tds18_2 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
    loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
    //---------------------------------------------------------------------------------------------
    loadData_len = loadData.length() + 1;
    // Prepare the character array (the buffer)
    loadData_char_array[loadData_len];
    // Copy it over
    loadData.toCharArray(loadData_char_array, loadData_len);
    //---------------------------------------------------------------------------------------------
    loadChanel_len = loadChanel.length() + 1;
    // Prepare the character array (the buffer)
    loadChanel_char_array[loadChanel_len];
    // Copy it over
    loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
    //---------------------------------------------------------------------------------------------
    client.publish(loadChanel_char_array, loadData_char_array);
    delay(5555);
  } */
  //==============================================================================================

  //struct tm timeinfo;
  ZamerNazvanie = "RSSi";
  loadData = String( WiFi.RSSI()) + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  // Prepare the character array (the buffer)
  loadData_char_array[loadData_len];
  // Copy it over
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  // Prepare the character array (the buffer)
  loadChanel_char_array[loadChanel_len];
  // Copy it over
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  delay(5555);
  /* 
    //==============================================================================================
    if (Tds18_4 > (-55)) {
    ZamerNazvanie = "Tds18_4";
    //struct tm timeinfo;
    loadData = Tds18_4 + String(',') + String(timeinfo.tm_year) + String('-') + String(timeinfo.tm_mon) + String('-') + String(timeinfo.tm_mday) + String('T') + String(timeinfo.tm_hour) + String(':') + String(timeinfo.tm_min) + String(':') + String(timeinfo.tm_sec) + String("+0700");
    loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
    //---------------------------------------------------------------------------------------------
    loadData_len = loadData.length() + 1;
    // Prepare the character array (the buffer)
    loadData_char_array[loadData_len];
    // Copy it over
    loadData.toCharArray(loadData_char_array, loadData_len);
    //---------------------------------------------------------------------------------------------
    loadChanel_len = loadChanel.length() + 1;
    // Prepare the character array (the buffer)
    loadChanel_char_array[loadChanel_len];
    // Copy it over
    loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);
    //---------------------------------------------------------------------------------------------
    client.publish(loadChanel_char_array, loadData_char_array);
    delay(5555);
    }*/
  //==============================================================================================
  ZamerNazvanie = "Upd";
  loadData = filenumberUpdate;
  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  loadData_char_array[loadData_len];
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  loadChanel_char_array[loadChanel_len];
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);

  //---------------------------------------------------------------------------------------------
  
  //==============================================================================================
  ZamerNazvanie = "Upd8888";
  loadData = 9;
  loadChanel = String("DarMal/") + String(GeoNazvanie) + String('/') + String(ZamerNazvanie);// + String(MestoNazvanie) + String('/') + String(UstroystvoNazvanie) + String('/') + String(ZamerNazvanie);
  //---------------------------------------------------------------------------------------------
  loadData_len = loadData.length() + 1;
  loadData_char_array[loadData_len];
  loadData.toCharArray(loadData_char_array, loadData_len);
  //---------------------------------------------------------------------------------------------
  loadChanel_len = loadChanel.length() + 1;
  loadChanel_char_array[loadChanel_len];
  loadChanel.toCharArray(loadChanel_char_array, loadChanel_len);

  //---------------------------------------------------------------------------------------------
  client.publish(loadChanel_char_array, loadData_char_array);
  delay(5555);
  ////////////////////////////////////////////
  // глубокий сон
  //Serial.println("Dsleep");
  esp_sleep_enable_timer_wakeup(1 * 1 * 1000000);
  esp_deep_sleep_start();
}


void WIFI(void) {
  Serial.println("Wifi_Start_0");
  WiFi.begin(ssid_0, password_0);
  int i = millis();
  int it = millis();
  Serial.println(WiFi.status());
  while ((WiFi.status() != WL_CONNECTED) && ((it - i) < 120000) ) {//    <<-----------------------------------------------<<
    delay(500);
    Serial.print(".");
    Serial.println((millis() - i));
    it = millis();
    Serial.println("Wifi_0");
  }
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.status());
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi_Start_1");
    WiFi.begin(ssid_1, password_1);
    int is = millis();
    i = millis();
    while ((WiFi.status() != WL_CONNECTED) && ((i - is) < 120000)) {//    <<-----------------------------------------------<<
      delay(500);
      Serial.print(".");
      Serial.println("Wifi_1");
      i = millis();
    }
    Serial.println(WiFi.localIP());
  }
  if (WiFi.status() == WL_CONNECTED) {}
  else {
    Serial.println("deepReset");
    esp_sleep_enable_timer_wakeup(5 * 1000000);
    esp_deep_sleep_start();
  };
}


////////////////////// MQTT слушатель /////////////////////////
void callback(char* topic, byte * message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // изменение состояния светодиода на первом приборе
  if (String(topic) == "house/device1/led") {
    Serial.print("Changing house/device1/led to ");
    if (messageTemp == "on") {
      Serial.println("on");

    }
    else if (messageTemp == "off") {
      Serial.println("off");

    }
  }
}
