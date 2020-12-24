#include <ArduinoJson.h>
#include <DHT.h>;
#include <LiquidCrystal_I2C.h>

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
LiquidCrystal_I2C lcd(0x27,20,4);
String ssid ="Wi_Virus4";
String password="Taku2020";
//String server = "isi dengan domain yang mengarah ke server-side anda"; // www.example.com
String uri = "/index.php";// our example is /esppost.php
String txt = "/test.json";
float humidity[6];
float temperature[6];
bool m[7];
int pengganti[6];
int tank=0;
int tora=0,tora2=0;
int trig_pin = 10;
int echo_pin = 11;
long echotime; 
float distance;
String mair;
void reset() {
  Serial3.println("AT+RST");
  delay(1000);
  if(Serial3.find("OK") ){
    Serial.println("Module Reset");
    lcd.clear();
    isi(3,0,"MODUL RESET");
  }else{
    Serial.println("Not REset");
    lcd.clear();
    isi(3,0,"GAGAL RESET");
    delay(1000);
    lcd.clear();
    isi(3,0,"MENCOBA LAGI");
    reset();
  }
}
void connectWifi() {
  
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\""; 
  Serial3.println(cmd);
  delay(4000);
  if(Serial3.find("OK")) {
    Serial.println("Client Connected!");
    lcd.clear();
    isi(3,2,"WIFI TERHUBUNG");
  }else {
    lcd.clear();
    isi(0,0,"WIFI TIDAK TERHUBUNG");
    delay(1000);
    lcd.clear();
    isi(3,0,"MENCOBA LAGI");
    connectWifi();
    Serial.println("Cannot connect to wifi"); 
  }
}
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
  for(int t=0; t<8; t++){
    digitalWrite(30+t, HIGH);
  }
  pinMode(trig_pin, OUTPUT); 
  pinMode(echo_pin, INPUT);
  digitalWrite(trig_pin, LOW);
  lcd.init();                   
  lcd.backlight();
  isi(0,0,"0  0 0000 0 000 0000");
  isi(0,1,"0  0 0  0 0 0   0  0");
  isi(0,2,"0  0 0  0 0 0   0000");
  isi(0,3,"0000 0  0 0 0   0  0");
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
  tank=cekair();
  if(tank>70){
    digitalWrite(36, LOW);
    Serial.println("Pompa AC Nyala");
//    if(!m[6]){
//      mair="MANUAL";
//    }else{
//      mair="OTOMATIS";
//    }
  }else if(tank<15){
    digitalWrite(36, HIGH);
//    if(m[6]){
//      tank=100;
//      mair="MANUAL";
//    }else{
//      mair="OTOMATIS";
//    }
  }
  if(tank!=100){
    isi(0,3,"TANGKI:"+String(100-tank)+"%"+mair);
  }else{
    isi(0,3,"TANGKI:"+String(tank)+"%"+mair);
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
    isi(0,0,"S"+String(i+1)+":"+temperature[i]+"C L"+String(i+1)+":"+humidity[i]+"%");
    delay(500);
    if(digitalRead(0)!=LOW){
      isi(2,1,"MODE : OTOMATIS");
      if(!isnan(temperature[i]) && !isnan(humidity[i])){
        if(temperature[i]>29 || humidity[i]<80 && !m[i]){
          digitalWrite(30+i,LOW);
          digitalWrite(37,LOW);
          Serial.println(" Aktif dalam keadaan NORMAL");
        }else{//kondisi ketika tombol otomatis dan suhu dalam keadaan normal
          if(m[i]){//manual dari tombol app walau sensor aktif
            digitalWrite(30+i,LOW);
            digitalWrite(37,LOW);
            isi(0,1,"MENYIRAM DARI APLIKASI");
            Serial.println(" Aktif dari app kondisi normal");
          }else{
            digitalWrite(30+i, HIGH);
//            digitalWrite(37,HIGH);
            Serial.println(" OFF Kondisi Normal ke- "+String(i));
            tora2=0;
            for(int j=0; j<6; j++){
              if(temperature[j]>29 || humidity[j]<80 && !m[j]){
                tora2++;
              }
            }
            if(tora2==0){
              digitalWrite(37,HIGH);
            }
          }
        }
         pengganti[i]=0;
      }else{
        if(m[i]){//manual dari tombol app jika bernilai NaN
          digitalWrite(30+i,LOW);
          digitalWrite(37,LOW);
          Serial.println(" Aktif dari tombol app kondisi Nan ke- "+String(i));
          isi(0,1,"MENYIRAM DARI APLIKASI");
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
                tora2=0;
                for(int j=0; j<6; j++){
                  if(temperature[j]>29 || humidity[j]<80 && !m[j]){
                    tora2++;
                  }
                }
                if(tora2==0){
                  digitalWrite(37,HIGH);
                }
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
      isi(2,1,"MODE : MANUAL");
      Serial.println("manual dari tombol box");
      for (int i2 = 0; i2 < 6; i2++) {
        digitalWrite(30+i2, LOW);
      }
      digitalWrite(37,LOW);
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
Serial3.println("AT+CIPSTART=\"TCP\",\"isi dengan domain yang mengarah ke server-side anda\",80");//start a TCP connection.
  if( Serial3.find("OK")) {
    Serial.println("TCP connection ready");
  }
  delay(1000);
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

int cekair(){
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  echotime= pulseIn(echo_pin, HIGH);
  distance= 0.0001*((float)echotime*340.0)/2.0;
  return distance;
}
void isi(int strt, int cu, String izi){
   lcd.setCursor(strt,cu);
   //for(int i=0;i<izi.length();i++){
    lcd.print(izi);
    delay(200);
   //}
}
