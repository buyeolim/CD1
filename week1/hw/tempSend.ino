/***
    'NodeMCU'보드와 'DS18B20'센서를 이용한 온도 측정 및
    웹('ThingSpeak')상 데이터 업로드
***/

#include <OneWire.h>            // 'DS18B20온도센서'제어 라이브러리
#include <DallasTemperature.h>  // 'DS18B20온도센서'제어 라이브러리
#include <ESP8266WiFi.h>        // 'NodeMCU'보드에서 와이파이 접근을 위한 라이브러리

#define ONE_WIRE_BUS 2  // 'DS18B20온도센서'의 중간핀(Data wire)은 '아두이노 디지털 2번핀'에 연결
#define LED_BUILTIN 2   // GPIO2 (D4 in NodeMCU)

OneWire oneWire(ONE_WIRE_BUS);          // one-wire 디바이스와 통신하기 위한 준비
DallasTemperature sensors(&oneWire);    // OneWire 참조를 Dallas Temperature에게 보낸다.

const char* server = "api.thingspeak.com";  // ThingSpeak API 서버
String apiKey = "VC73R15OOJP4W0A8";         // ThingSpeak의 Write API key
const char* ssid = "myWifi";                // 와이파이 SSID
const char* password = "myPassword";        // 와이파이 비밀번호
WiFiClient client;                          // 와이파이 클라이언트 변수

/* 
 * 최초 1회 setup 함수 작동 (리셋 or 전원연결시)
 */
void setup() 
{
    Serial.begin(9600);  // start serial port
    sensors.begin();     // start up the library
    connectWifi();     
    
    return;
}

/* 
 *  loop 함수는 반복 작동
 */
void loop()
{
    float t; // 온도(Temperature), 섭씨
    
    sensors.requestTemperatures();  // 온도센서 사용 요청
    t = sensors.getTempCByIndex(0); // 온도 측정(섭씨)
    
    Serial.print("Current Temperature(Celsius): ");
    Serial.println(t);
    
    if (client.connect(server,80)) {  
    // "184.106.153.149" or api.thingspeak.com 서버에 연결, 해당 API key에 업로드
        String postStr = apiKey + "&field1=" + String(t) + "\r\n\r\n";

        Serial.println("Sending data to ThingSpeak");
        Serial.println("");
        
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
        delay(56000);                     // 56초 딜레이(1분 간격 측정을 위해)
    }
    client.stop();
  
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
