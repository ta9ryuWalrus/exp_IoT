#include <ESP8266WiFi.h>
#include <IPAddress.h>

const char* ssid = "bld2-guest";
const char* password = "MatsumotoWay";
const char* host = "www.hongo.wide.ad.jp";

void setup(){
    Serial.begin(115200);
    delay(10);

    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(". ");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

int value = 0;

void loop(){
    delay(5000);
    ++value;

    Serial.print("connecting to ");
    Serial.println(host);

    //Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if(!client.connect(host, httpPort)){
        Serial.println("connection failed");
        return;
    }

    //We now create a URI for the request
    String url = "/";
    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + "HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while(client.available() == 0){
        if(millis() - timeout > 5000){
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    while(client.available()){
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");

    delay(55000);
}