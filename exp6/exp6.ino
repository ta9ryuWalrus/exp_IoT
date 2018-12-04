void setup(){
    pinMode(A0, INPUT);
    Serial.begin(9600);
}

void loop(){
    int tout = analogRead(A0);
    double syodo = 3210.0 / 3.0 * tout / 1024.0;
    Serial.print("syodo is");
    Serial.println(syodo);
    delay(1000);
}