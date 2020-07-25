#include <ArduinoJson.h>

#include <DHT.h>;

#define DHTPIN1 2
#define DHTPIN2 3
#define DHTPIN3 4
#define DHTPIN4 5
#define DHTPIN5 6
#define DHTPIN6 7

DHT dht[] = {
  {DHTPIN1, DHT22},
  {DHTPIN2, DHT22},
  {DHTPIN3, DHT22},
  {DHTPIN4, DHT22},
  {DHTPIN5, DHT22},
  {DHTPIN6, DHT22},
};
String ssid ="Biasa";
String password="sandi12397";
//String server = "odi.sdnlada2.sch.id"; // www.example.com
String uri = "/index.php";// our example is /esppost.php
String txt = "/test.json";
float humidity[6];
float temperature[6];
bool m[7];
int pengganti[6];
int tora=0;
int trig_pin = 10;
int echo_pin = 11;
long echotime; 
float distance;
void(* ulang) (void) = 0;
void setup()
{
  Serial3.begin(115200);//AT+UART_DEF=9600,8,1,0,0
  Serial.begin(9600);
  for (auto& sensor : dht) {
    sensor.begin();
  }
  pinMode(30,OUTPUT);
  pinMode(31,OUTPUT);
  pinMode(32,OUTPUT);
  pinMode(33,OUTPUT);
  pinMode(34,OUTPUT);
  pinMode(35,OUTPUT);
  pinMode(36,OUTPUT);
  pinMode(37,OUTPUT);
  pinMode(trig_pin, OUTPUT); 
  pinMode(echo_pin, INPUT);
  digitalWrite(trig_pin, LOW);
  reset();
  connectWifi();
}

String data="";
void loop()
{
  for (int i = 0; i < 6; i++) {
    temperature[i] = dht[i].readTemperature();
    humidity[i] = dht[i].readHumidity();
  }
  int tank=cekair();
  if(tank>80 || m[6]){
    digitalWrite(36, LOW);
    Serial.println("Pompa AC Nyala");
  }else if(tank<10 || !m[6]){
    digitalWrite(36, HIGH);
    if(m[6]){
      tank=100;
    }
  }
  for (int i = 0; i < 6; i++) {
    Serial.print("T");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(temperature[i]);
    Serial.print(" || H");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(humidity[i]);
    if(digitalRead(0)!=LOW){
      if(!isnan(temperature[i]) && !isnan(humidity[i])){
        if(temperature[i]>29 || humidity[i]<80 && !m[i]){
          digitalWrite(30+i,LOW);
          Serial.println(" Aktif dalam keadaan NORMAL");
        }else{//kondisi ketika tombol otomatis dan suhu dalam keadaan normal
          if(m[i]){//manual dari tombol app walau sensor aktif
            digitalWrite(30+i,LOW);
            Serial.println(" Aktif dari app kondisi normal");
          }else{
            digitalWrite(30+i, HIGH);
            Serial.println(" OFF Kondisi Normal ke- "+String(i));
          }
        }
         pengganti[i]=0;
      }else{
        if(m[i]){//manual dari tombol app jika bernilai NaN
          digitalWrite(30+i,LOW);
          Serial.println(" Aktif dari tombol app kondisi Nan ke- "+String(i));
        }else{
          if(pengganti[i]!=0){
            Serial.print(" Pengganti di temukan di- "+String(pengganti[i]));
            if(!isnan(temperature[pengganti[i]]) && !isnan(humidity[pengganti[i]])){
              if(temperature[pengganti[i]]>29 || humidity[pengganti[i]]<80){
                digitalWrite(30+pengganti[i], LOW);
                digitalWrite(30+i, LOW);
                Serial.println(" Aktif dari pengganti");
              }else{
                Serial.println(" Nonaktif dari pengganti");
                digitalWrite(30+pengganti[i], HIGH);
                digitalWrite(30+i, HIGH);
              }
            }else{
              pengganti[i]=0;
            }
          }else{
            Serial.println(" Mencari pengganti sensor ke- "+String(i));
            for (int s=0; s<6; s++){//perulangan mencari yang akan ditunjuk pengganti sensor yang mati/error
              if(!isnan(temperature[s]) && !isnan(humidity[s])){
                if(temperature[s]>29 || humidity[s]<80){
                  pengganti[i]=s;
                  digitalWrite(30+s, LOW);
                  digitalWrite(30+i, LOW);
                }
              }
            }
          }//cek pengganti
        }
      }
      //otomatis dari tombol box
    }else{
      //manual dari tombol box
      Serial.println("manual dari tombol box");
      for (int i2 = 0; i2 < 6; i2++) {
        digitalWrite(30+i2, LOW);
      }
    }
    if(i!=5){
      data+="tp"+String(i)+"="+temperature[i]+"&hm"+String(i)+"="+humidity[i]+"&";
    }else{
      data+="tp"+String(i)+"="+temperature[i]+"&hm"+String(i)+"="+humidity[i]+"&tank="+tank;
    }
  }
  Serial.println(tank);
  Serial.println(data);
  httppost (uri);
  httppost (txt);
  data="";
//  delay(3000);
}
void httppost (String urr) {
  DynamicJsonDocument doc(1024);
Serial3.println("AT+CIPSTART=\"TCP\",\"odi.sdnlada2.sch.id\",80");//start a TCP connection.
  if( Serial3.find("OK")) {
    Serial.println("TCP connection ready");
  }
//  delay(1000);
  String postRequest =
  "POST " + urr + " HTTP/1.0\r\n" +
  "Host: odi.sdnlada2.sch.id\r\n" +
  "Accept: *" + "/" + "*\r\n" +
  "Content-Length: " + data.length() + "\r\n" +
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  "\r\n" + data;
  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  Serial3.print(sendCmd);
  Serial3.println(postRequest.length() );
  delay(500);
  String tmpResp="",l;
  if(Serial3.find(">")) { 
    Serial.println("Sending.."); 
    Serial3.println(postRequest);
    if( Serial3.find("SEND OK")) {
      Serial.println("Packet sent");
      if(tora!=0){
        if (Serial3.available()) {
//        tmpResp += Serial3.readString();
//        Serial.println(tmpResp);
          Serial3.find("Agent\r\n");
          l=Serial3.readStringUntil("\r\n\r\n");
          String k=l.substring(1,l.length()-8);
          DeserializationError error=deserializeJson(doc, k);
          if(error){
            Serial.println("deserialize eror");
            Serial.print(error.c_str());
//            ulang();
          }
          m[0]=doc["m0"].as<bool>();
          m[1]=doc["m1"].as<bool>();
          m[2]=doc["m2"].as<bool>();
          m[3]=doc["m3"].as<bool>();
          m[4]=doc["m4"].as<bool>();
          m[5]=doc["m5"].as<bool>();
          m[6]=doc["tkk"].as<bool>();
          Serial.println(k);
        }
        tora=0;
      }else{
        tora=1;
      }
      Serial3.println("AT+CIPCLOSE");
      while(Serial3.find("CLOSED")) {
        Serial3.println("AT+CIPCLOSE");  
      }
    }
  }
}
void reset() {
  Serial3.println("AT+RST");
  delay(1000);
  if(Serial3.find("OK") ){
    Serial.println("Module Reset");
  }else{
    Serial.println("Not REset");
    reset();
  }
}
void connectWifi() {
  
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\""; 
  Serial3.println(cmd);
  delay(4000);
  if(Serial3.find("OK")) {
    Serial.println("Client Connected!");
  }else {
    connectWifi();
    Serial.println("Cannot connect to wifi"); 
  }
}
int cekair(){
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  echotime= pulseIn(echo_pin, HIGH);
  distance= 0.0001*((float)echotime*340.0)/2.0;
  return distance;
}
