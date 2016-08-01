 char ch;
 int flag=0;
 
void setup()
{ 
      Serial.begin(9600);                 
      // Vibrator motor
      pinMode(12, OUTPUT); 
      digitalWrite(12,LOW);
      pinMode(13, OUTPUT); 
      digitalWrite(13,LOW);
} 
    
 void loop()
 {
       if(Serial.available())
       {   ch = Serial.read();
           // send values
          // vibrator motor ON
          if(ch == 'm')
           { 
             digitalWrite(12,HIGH);
             digitalWrite(13,HIGH);
             flag=1;
             delay(175);
             Serial.println('a');
           }
             // vibrator motor OFF
             digitalWrite(12, LOW);
             digitalWrite(13, LOW);
             // reset values
             if(ch == 'r')
             {
               flag=0;
             }
       } 
      Serial.flush();  
}

