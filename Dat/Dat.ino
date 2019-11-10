#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Имя и пароль вашей сети WiFi
const char* ssid = "Explabs_Robotics";
const char* password = "Huttka18";

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
char temperatureCString[6];
char temperatureFString[6];

// Создаем сервер и порт для прослушки 80
 
WiFiServer server(80);
 
void setup() {
 Serial.begin(115200);
 delay(10);

 DS18B20.begin();
 
 // Подготовка GPIO
 pinMode(4, OUTPUT);
 digitalWrite(4, 0);
 
 // Подключаемся к сети WiFi
 Serial.println();
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 
 // Ожидание подключения
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 
 // Запуск сервера
 server.begin();
 Serial.println("Server started");
 
 // Вывод полученного IP адреса
 Serial.println(WiFi.localIP());
}

void getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    tempF = DS18B20.getTempFByIndex(0);
    dtostrf(tempF, 3, 2, temperatureFString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void loop() {
 
 // Проверка подключения
 WiFiClient client = server.available();
 if (!client) {
 return;
 }
 
 // Ожидание данных
 Serial.println("new client");
 while (!client.available()) {
 delay(1);
 }
 
 boolean blank_line = true;
 
 // Чтение первой строки запроса
 String req = client.readStringUntil('\r');
 Serial.println(req);
 client.flush(); 
  if(req.indexOf("status") != -1){
    getTemperature();
    Serial.println(temperatureCString);
 }
 // Работа с GPIO
 if (req.indexOf("start") != -1){
  digitalWrite(4, 1);
 }
 if (req.indexOf("stop") != -1){
  digitalWrite(4, 0);
 }
 else if (req.indexOf("stop") != -1) {
  digitalWrite(4, 0);
 client.stop();
 return;
 }
 else
 // Если неверный запрос написать об ошибке
 {
 Serial.println("invalid request");
 client.stop();
 return;
 }
}
