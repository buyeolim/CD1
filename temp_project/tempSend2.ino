/***
    'NodeMCU'보드와 'DS18B20'센서를 이용한 온도 측정 및
    웹('ThingSpeak' & 'Amazon EC2')상 데이터 전송
***/

#include <OneWire.h>            // 'DS18B20온도센서'제어 라이브러리
#include <DallasTemperature.h>  // 'DS18B20온도센서'제어 라이브러리
#include <ESP8266WiFi.h>        // 'NodeMCU'보드에서 와이파이 접근을 위한 라이브러리
#include <time.h>               // 날짜, 시간 함수 사용을 위한 라이브러리

#define ONE_WIRE_BUS 2  // 'DS18B20온도센서'의 중간핀(Data wire)은 '아두이노 디지털 2번핀'에 연결
#define LED_BUILTIN 2   // GPIO2 (D4 in NodeMCU)

OneWire oneWire(ONE_WIRE_BUS);          // one-wire 디바이스와 통신하기 위한 준비
DallasTemperature sensors(&oneWire);    // OneWire 참조를 Dallas Temperature에게 보냄

const char* serverTs = "api.thingspeak.com";    // ThingSpeak API 서버
String apiKey = "VC73R15OOJP4W0A8";             // ThingSpeak의 Write API key

const char* serverEc2 = "54.180.101.233";       // EC2 서버
const char* serverNtp = "kr.pool.ntp.org";      // NTP 서버

int gmt = 9; // 한국시간: 그리니치 평균시(Greenwich Mean Time) + 9
int dst = 0; // 서머타임(Daylight Saving Time): 0

const char* ssid = "myWifi";              // 와이파이 SSID
const char* password = "myPassword";      // 와이파이 비밀번호
WiFiClient client;                        // 와이파이 클라이언트 변수

/* 
 * 최초 1회 setup 함수 작동 (리셋 or 전원연결시)
 */
void setup()
{
    Serial.begin(9600);  // start serial port
    sensors.begin();      // start up the library
    connectWifi(); 
    configTime(gmt * 3600, dst * 3600, serverNtp); // NTP 서버의 시간을 한국시간에 맞춰 가져옴

    return;
}

/* 
 *  loop 함수는 반복 작동
 */
void loop()
{
    float temp; // 온도(Temperature), 섭씨
    
    sensors.requestTemperatures();     // 온도센서 사용 요청
    temp = sensors.getTempCByIndex(0); // 온도 측정(섭씨)
    
    Serial.print("Current Temperature(Celsius): ");
    Serial.println(temp);

    sendToThingspeak(temp);
    sendToEc2(temp);
    Serial.println("");
    
    return;
}

/*
 * 와이파이 연결
 */
void connectWifi()
{
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);

    // 와이파이 연결상태 확인(1초 간격)
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("");

    return;
}

/*
 * 'ThingSpeak'로 전송(온도)
 */
 void sendToThingspeak(float temp)
 {
    if (client.connect(serverTs, 80)) {  
    // "184.106.153.149" or api.thingspeak.com 서버에 연결, 해당 API key에 전송
        String postStr = apiKey + "&field1=" + String(temp) + "\r\n\r\n";

        Serial.println("Sending data to ThingSpeak");
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        digitalWrite(LED_BUILTIN, HIGH);  // LED on 
        delay(1000);                      // 1초 딜레이
        digitalWrite(LED_BUILTIN, LOW);   // LED off
        delay(58000);                     // 58초 딜레이(1분 간격 측정을 위해)
    }
    client.stop();
    
    return;
 }

/*
 * 'Amazon EC2'로 전송(온도, 날짜, 시간)
 */
  void sendToEc2(float temp)
 {  
    time_t now = time(nullptr);
    struct tm * timeinfo;       // 타임 구조 테이블
    timeinfo = localtime(&now); // time_t를 시간 형태의 구조체로 변환
    
    unsigned int year   = 1900 + timeinfo->tm_year;
    unsigned int month  = 1 + timeinfo->tm_mon;
    unsigned int date   = timeinfo->tm_mday;
    unsigned int hour   = timeinfo->tm_hour;
    unsigned int minute = timeinfo->tm_min;
 
    if (client.connect(serverEc2, 4885)) {
    // Amazon EC2 서버에 연결, JSON data 전송
        String logStr = "GET /log";
        logStr += "?year="+ String(year) + "&month=" + String(month)  + "&date=" + String(date);
        logStr += "&hour=" + String(hour) + "&minute=" + String(minute) + "&temp=" + String(temp);
          
        Serial.println("Sending data to EC2 / ");
        
        client.print(logStr);                   
        client.print("\n\n");                     
   
        digitalWrite(LED_BUILTIN, HIGH);  
        delay(1000);                      
        digitalWrite(LED_BUILTIN, LOW);   
        delay(58000);                      
    }
    client.stop();
    
    return;
 }
