#define MAX_BUFFER_SIZE 63

char recv_buffer[MAX_BUFFER_SIZE + 1];
int n_recv_buffer = 0;
boolean is_newline = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()){
    if(n_recv_buffer < MAX_BUFFER_SIZE){
      char c = Serial.read();
      if(c==0x7f){
        if(n_recv_buffer > 0){
          Serial.print(c);
          recv_buffer[--n_recv_buffer] = '\0';
        }
      }
      else if(0x20 <= c && c <= 0x7e){
        Serial.print(c);
        recv_buffer[n_recv_buffer++] = c;
      }
      else if(c==0x0d){
        is_newline = true;
        recv_buffer[n_recv_buffer] = 0x00;
        Serial.println();
        break;
      }
    }
    else{
      Serial.println("Exceeded maximum length ... force new line");
      n_recv_buffer--;
      is_newline = true;
      recv_buffer[n_recv_buffer] = 0x00;
      break;
    }
  }

  if(is_newline){
    int len = strlen(recv_buffer);
    Serial.print("len=");
    Serial.print(len);
    Serial.print(",\"");
    Serial.print(recv_buffer);
    Serial.println("\"");
    is_newline = false;
    n_recv_buffer = 0;
  }
}
