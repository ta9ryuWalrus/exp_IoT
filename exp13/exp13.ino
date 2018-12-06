#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "bld2-guest";
const char* password = "MatsumotoWay";
const char* host = "iot.hongo.wide.ad.jp";
const int port = 50420;
const char* ntp_server = "ntp.nict.jp";
unsigned long last_sync_time = 0;
int prev_stat = LOW;

#define OLED_RESET 2
Adafruit_SSD1306 display(OLED_RESET);

void setup(){
    Serial.begin(115200);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to ");
    display.println(ssid);
    display.display();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        display.print(".");
        display.display();
    }
    display.println();
    display.display();

    display.println("WiFi connected");
    display.println("IP address: ");
    display.println(WiFi.localIP());
    display.display();
    syncNTPtime(ntp_server);
    display.print("Sync to ");
    display.println(ntp_server);
    display.display();

    char str_time[30];
    unsigned long t = now();
    sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d",
        year(t), month(t), day(t), hour(t), minute(t), second(t));
    display.println(str_time);
    display.display();
}

void loop(){
    delay(3000);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.display();

    unsigned long t = now();
    if(t/30 != last_sync_time/30){

        //同期
        syncNTPtime(ntp_server);
        last_sync_time = t;

        //messageの作成
        int dip = getDIPSWStatus();
        int illumi = getIlluminance();
        int md = 0;
        if(getMDStatus()){
            md = 1;
        }
        char message[60];
        sprintf(message, "%d,%04d-%02d-%02dT%02d:%02d:%02d,%d,%d",
            dip, year(t), month(t), day(t), hour(t), minute(t), second(t), illumi, md);
        display.println(message);
        display.display();

        //サーバへの接続
        WiFiClient client;
        if(!client.connect(host, port)){
            display.println("...ERR");
            display.println();
            display.display();
            return;
        }
        client.print(message);
        char* recv = client.read();
        if(recv == "OK"){
            display.println("...OK");
            display.println();
        }else if(recv == "ERROR"){
            display.println("...NG");
            display.println();
        }else if(!client.connected()){
            display.println("...ERR");
            display.println();
        }
        client.stop();
    }
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