#include <SoftwareSerial.h>
#include <avr/sleep.h>

const String ssid = "test";         //change lines to your network credentials and specify receiving server and file
const String password = "123456";
const String host = "example.net";
const String file = "myexample.php";

const byte interruptPin = 2;

bool sent = false;

SoftwareSerial esp(3, 4);// RX, TX

void setup() 
{
  Serial.begin(9600);
  esp.begin(9600);
  reset();
  
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(interruptPin,INPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  attachInterrupt(digitalPinToInterrupt(interruptPin),WakeUp,HIGH);
  connectWifi();
}

void loop()
{
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
  esp.println("AT+CIPSTART=\"TCP\",\"+host+"\",80");
  delay(500);
  if( esp.find("OK")) {
    Serial.println("TCP connection ready");
  }
  else Serial.println("TCP connection failed");
  String getRequest = "GET +"+file+"?active="+reed+" HTTP/1.1\r\nHost: "+host+"\r\n\r\n";
  String sendCmd = "AT+CIPSEND=";
  esp.print(sendCmd);
  esp.println(getRequest.length() );
  if(esp.find(">")) { 
    Serial.println("Sending.."); 
    esp.print(getRequest);
    if( esp.find("SEND OK")) { 
      delay(100);
      Serial.println("Packet sent");
      while (esp.available()) {
        String tmpResponse = esp.readString();
        Serial.println(tmpResponse); 
      }
      esp.println("AT+CIPCLOSE");
      Serial.println("connection closed");
    }
    else Serial.println("Package Loss");
  }
  else Serial.println("Sending failed");
}

void connectWifi() {
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  esp.println(cmd);
  delay(1000);
  if(esp.find("OK")) {
    Serial.println("Connected!");
  }
  else {
    Serial.println("Cannot connect to wifi"); 
  }
}

void GoToSleep(){
  Serial.println("Sleeping..");
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(LED_BUILTIN,LOW);
  sleep_bod_disable();
  sleep_cpu();
  Serial.println("Woke Up");
  digitalWrite(LED_BUILTIN,HIGH); 
}

void WakeUp(){
  Serial.println("Interrupt fired");
  sleep_disable();
}
