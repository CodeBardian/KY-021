#include <SoftwareSerial.h>

#include <avr/sleep.h>
//AT+CWJAP="DESKTOP-HH6F8E3 2702","b8>2049U"
const String ssid = "DESKTOP-HH6F8E3 2702";
const String password = "b8>2049U";
const byte interruptPin = 2;
const byte espInterrupt = 5;

bool sent = false;

SoftwareSerial esp(3, 4);// RX, TX

void setup() 
{
  Serial.begin(9600);
  Serial.println("Hello");
  esp.begin(9600);
  esp.println("AT");
  if(esp.find("OK"))
    Serial.println("Module starting!");
  else Serial.println("Module not responding!");
  //reset();
  //esp.println("AT+WAKEUPGPIO=1,2,0");
  //if(esp.find("OK"))
    //Serial.println("Wakeup pin set!");
  //else Serial.println("Wakeup pin error!");

  pinMode(espInterrupt,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(interruptPin,INPUT);
  Serial.print("PIN2:");
  Serial.println(digitalRead(interruptPin));
  digitalWrite(LED_BUILTIN,HIGH);
  //digitalWrite(interruptPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(interruptPin),WakeUp,HIGH);
  //connectWifi();
}

void loop()
{
  //Serial.print("PIN2:");
  //Serial.println(digitalRead(interruptPin));
  if (digitalRead(interruptPin) == LOW) // when the magnetic sensor detect a signal
  {
    httppost("0");
    sent = false;
    GoToSleep();
  }
  else{
    if (sent==false){
      httppost("1");
      sent = true;
    }
  }
}

void reset() {
  esp.println("AT+RST");  
  delay(1000);
  if (esp.find("OK") ) Serial.println("Module Reset");
  else Serial.println("Reset failed");
}

void httppost(String reed){
  digitalWrite (LED_BUILTIN, HIGH);  
  //delay (100);       // mS        
  //digitalWrite (LED_BUILTIN, LOW);    
  //delay (900);    
  esp.println("AT+CIPSTART=\"TCP\",\"bard.bplaced.net\",80");
  delay(500);
  if( esp.find("OK")) {
    Serial.println("TCP connection ready");
  }
  else Serial.println("TCP connection failed");
  //delay(1000);
  String getRequest = "GET /api/active.php?active="+reed+"&id=6 HTTP/1.1\r\nHost: bard.bplaced.net\r\n\r\n";
  String sendCmd = "AT+CIPSEND=";
  esp.print(sendCmd);
  esp.println(getRequest.length() );
  //delay(1000);
  if(esp.find(">")) { 
    Serial.println("Sending.."); 
    esp.print(getRequest);
    //delay(1000);
    if( esp.find("SEND OK")) { 
      delay(100);
      Serial.println("Packet sent");
      while (esp.available()) {
        //Serial.print(esp.read());
        String tmpResponse = esp.readString();
        Serial.println(tmpResponse); 
      }
      esp.println("AT+CIPCLOSE");
      Serial.println("connection closed");
    }
    else Serial.println("Package Loss");
  }
  else Serial.println("Sending failed");
  //digitalWrite (LED_BUILTIN, LOW); 
}

void connectWifi() {
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  esp.println(cmd);
  delay(1000);
  if(esp.find("OK")) {
    Serial.println("Connected!");
  }
  else {
    //connectWifi();
    Serial.println("Cannot connect to wifi"); 
  }
}

void GoToSleep(){
  //attachInterrupt(digitalPinToInterrupt(interruptPin),WakeUp,CHANGE);
  //esp.println("AT+SLEEP=1");
  Serial.println("Sleeping..");
  //esp.println("AT+GSLP=1");
  if(esp.find("OK")) {
    Serial.println("ESP sleeping..!");
  }
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(LED_BUILTIN,LOW);
  delay(500);
  sleep_bod_disable();
  sleep_cpu();
  digitalWrite(espInterrupt,LOW);
  delay(1000);
  digitalWrite(espInterrupt,HIGH);
  Serial.println("Woke Up");
  digitalWrite(LED_BUILTIN,HIGH); 
}

void WakeUp(){
  Serial.println("Interrupt fired");
  sleep_disable();
  //detachInterrupt(digitalPinToInterrupt(interruptPin)); 
}
