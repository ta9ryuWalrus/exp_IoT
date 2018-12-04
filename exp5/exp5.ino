void setup(){
    pinMode(16, INPUT);
    Serial.begin(9600);
}

void loop(){
    int sensor=digitalRead(16);
    if(sensor==LOW){
        Serial.println("Not sensing");
    }
    else{
        Serial.println("Sensing");
    }
    delay(1000);
}