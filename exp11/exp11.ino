#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <SPI.h>
#include <Wire.h>

const char* ssid = "bld2-guest";
const char* password = "MatsumotoWay";
const char* host = "www.hongo.wide.ad.jp";
unsigned long last_sync_time = 0;

int prev_stat = LOW;

/*void setup(){//setBZ(true)
    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
    delay(5000);
}

void setup(){//setBZ(false)
    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH);
    delay(1000);
}

void setup(){//getPushSWStatus()
    Serial.begin(9600);
    pinMode(2, INPUT);
}

void setup(){ //detectPushSWON()
    Serial.begin(9600);
    pinMode(2, INPUT);
}

void setup(){//getDIPSWStatus()
    Serial.begin(9600);
    pinMode(12, INPUT);
    pinMode(13, INPUT);
}

void setup(){//getMDStatus()
    pinMode(16, INPUT);
    Serial.begin(9600);
}

void setup(){//getIlluminance()
    pinMode(A0, INPUT);
    Serial.begin(9600);
}*/

void setup(){//getNTPtime
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

/*void setup(){//syncNTPtime
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
    syncNTPtime();
}*/

/*void loop(){//setBZ(true)
    setBZ(true);
    while(1);
}

void loop(){//setBZ(false)
    setBZ(false);
    while(1);
}

void loop(){//getPushSWStatu()
    if(getPushSWStatus()){
        Serial.println("true");
    }else{
        Serial.println("false");
    }
    delay(1000);
}

void loop(){//detectPushSWON()
    if(detectPushSWON()){
        Serial.println("true");
    }else{
        Serial.println("false");
    }
    delay(1000);
}

void loop(){//getDIPSWStatus()
    Serial.println(getDIPSWStatus());
    delay(1000);
}

void loop(){//getMDStatus()
    if(getMDStatus()){
        Serial.println("true");
    }else{
        Serial.println("false");
    }
    delay(1000);
}

void loop(){//getIllminance()
    Serial.println(getIlluminance());
    delay(1000);
}*/

/*void loop(){
    const char* ntp_server ="ntp.nict.jp";
    const char* ntp_server2 = "www.gutp.jp";
    unsigned long unixtime = getNTPtime(ntp_server);
    Serial.println(unixtime);
    delay(1000);
}*/

void loop(){//syncNTPtime
const char* ntp_server = "ntp.nict.jp";
const char* ntp_server2 = "www.gutp.jp";
    unsigned long t = now();
    char str_time[30];
    sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d",
        year(t), month(t), day(t), hour(t), minute(t), second(t));

    Serial.print("Real time: ");
    Serial.println(str_time);
    Serial.print("Unix time: ");
    Serial.println(getNTPtime(ntp_server));
    if(syncNTPtime(ntp_server)){
        Serial.println("true");
    }else{
        Serial.println("false");
    }
    delay(15000);
}

void setBZ(boolean on){
    if(on==true){
        digitalWrite(14, HIGH);
    }
    else{
        digitalWrite(14, LOW);
    }
}

boolean getPushSWStatus(){
    int stat = digitalRead(2);
    if(stat == LOW)return true;
    else return false;
}

boolean detectPushSWON(){
    int stat = digitalRead(2);
    boolean res;
    if(stat == LOW && prev_stat == HIGH)res = true;
    else res = false;
    
    prev_stat = stat;
    return res;
}

int getDIPSWStatus(){
    int stat = 0;
    int bit1 = digitalRead(12);
    int bit0 = digitalRead(13);
    if(bit0==LOW){
        stat|=0x01;
    }
    if(bit1==LOW){
        stat|=0x02;
    }
    return stat;
}

boolean getMDStatus(){
    int sensor = digitalRead(16);
    if(sensor==LOW)return false;
    else return true;
}

int getIlluminance(){
    int tout = analogRead(A0);
    int syodo = 3210 * tout / 3 / 1024;
    return syodo;
}

boolean syncNTPtime(const char* ntp_server){
    unsigned long unix_time = getNTPtime(ntp_server);
    if(unix_time > 0){
        setTime(unix_time);
        return true;
    }
    return false;
}

unsigned long getNTPtime(const char* ntp_server){
    WiFiUDP udp;
    udp.begin(8888);
    unsigned long unix_time = 0UL;
    byte packet[48];
    memset(packet, 0, 48);
    packet[0] = 0b11100011;
    packet[1] = 0;
    packet[2] = 6;
    packet[3] = 0xEC;
    packet[12] = 49;
    packet[13] = 0x4E;
    packet[14] = 49;
    packet[15] = 52;

    udp.beginPacket(ntp_server, 123);
    udp.write(packet, 48);
    udp.endPacket();

    for(int i=0; i<10; i++){
        delay(500);
        if(udp.parsePacket()){
            udp.read(packet, 48);
            unsigned long highWord = word(packet[40], packet[41]);
            unsigned long lowWord = word(packet[42], packet[43]);
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            const unsigned long seventyYears = 2208988800UL;
            unix_time = secsSince1900 - seventyYears + 32400UL;
            break;
        }
    }
    udp.stop();
    return unix_time;
}