#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Имя и пароль вашей сети WiFi
const char* ssid = "Explabs";
const char* password = "Huttka12";
IPAddress ip(192,168,31,31);
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
char temperatureCString[6];

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
 
 // Запуск сервера
 server.begin();
 
 // Вывод полученного IP адреса
 Serial.println(WiFi.localIP());
}

void getTemperature() {
  float tempC;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void loop() {
 
 // Проверка подключения
 WiFiClient client = server.available();
 if (!client) {
 return;
 }
 else {
 
 // Ожидание данных
 Serial.println("new client");
 int count;
 while (!client.available()) {
 if (count >= 3){break;}
 delay(100);
 count++;
 }
 
 // Чтение первой строки запроса
 String req = client.readStringUntil('\r');
 Serial.println(req);
 client.flush();
 
  int temp;
 
 if(req.indexOf("status") != -1){
    getTemperature();
    Serial.println(temperatureCString);
 }
 // Работа с GPIO
 if (req.indexOf("start") != -1){
   if(req.indexOf("?") != -1){
    int len = req.length();
    int temp1 = req.indexOf("=");
    String temp2 = "";
    while(temp1 != len){
      temp1 += 1;
      if(req[temp1] == ' '){
       break;
      }
      temp2 += req[temp1];
    }
    
    temp = temp2.toInt();
    if(temp > 100){temp=100;}
    delay(200);
  }
  else {
    temp = 100;
  }
  Serial.print("OK ");
  Serial.println(temp);
  digitalWrite(4, 1);
 }
 if (req.indexOf("stop") != -1){
  Serial.println("STOP");
  digitalWrite(4, 0);
 }
 else
 // Если неверный запрос написать об ошибке
 {
 Serial.println("invalid request");
 client.stop();
 return;
 }
 }
}
