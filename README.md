# MyTa2
Code Dibawah ini merupakan logika awal saya.
```
if(temperature[i]>29 || humidity[i]<80){
      if(temperature[i]!=0 && humidity[i]!=0){
        digitalWrite(30+i, LOW);
        Serial.println("high di not nol");
      }else{
        if(i!=5 || i!=4){
          digitalWrite(30+1+i, LOW);
        }else{
          if(temperature[i]!=0 && humidity[i]!=0){
            digitalWrite(30+i-1, LOW);
          }
        }
      }
    }else{
      if(i!=5){
        if(temperature[i+1]!=0 && humidity[i+1]!=0){
          digitalWrite(30+i, HIGH);
        }else{
          if(!temperature[i+1]>29 || !humidity[i+1]<80){
            digitalWrite(30+i, HIGH);
          }
        }
      }else{
        digitalWrite(30+i, HIGH);
      }
    }
```
