/* Reference: 
    www.psurobotics.org/wiki/index.php?title=Wheel_Encoders
    www.ni.com/white-paper/3921/en/
*/    
    
/************************************* 
Pin Connection
    
    Encoder 1-Linear Motion
      1. Gnd        
      2. Index  
      3. Channel A    interrupt 0 - pin 2, pin 5  
      4. +5V
      5. Channel B    interrupt 1 - pin 3, pin 4
      
    Encoder 2-Rotary Motion
      1. Gnd        
      2. Index  
      3. Channel A    interrupt 2 - pin 21, pin 7  
      4. +5V
      5. Channel B    interrupt 3 - pin 20, pin 6
    
**************************************/
    
    #include <avr/interrupt.h>
    
    // Encoder 1
    #define encoder1PinA  5            //Quadrature Track A
    #define encoder1PinB  4            //Quadrature Track B
    
    // Encoder 2
    #define encoder2PinA  7            //Quadrature Track A
    #define encoder2PinB  6            //Quadrature Track B
    
    //Encoder resolution
    int R=2048;
    
    //encoder 1 variables
    volatile long int nA1;
    volatile long int nB1;
    volatile long int encoder1PinALast;
    volatile long int encoder1PinBLast;
    volatile long int  encoder1Pos = 0;      //the encoder position variable.
    
    
     long int lasttime1 = 0;
     long int curtime1 = 0;
     double lastrpm1 = 0;
     double rpm1 = 0;
     double accel1 = 0;
     double posd1=0;
     double vel1=0;
     double angular_velocity1=0;
     long newposition1;
     long oldposition1 = 0;
     unsigned long newtime1;
     unsigned long oldtime1 = 0;
     
    //encoder 2 variables
    volatile long int nA2;
    volatile long int nB2;
    volatile long int encoder2PinALast;
    volatile long int encoder2PinBLast;
    volatile long int  encoder2Pos = 0;      //the encoder position variable.
    
     long int lasttime2 = 0;
     long int curtime2 = 0;
     double lastrpm2 = 0;
     double rpm2 = 0;
     double accel2 = 0;
     double posd2=0;
     double vel2=0;
     double angular_velocity2=0;
     long newposition2;
     long oldposition2 = 0;
     unsigned long newtime2;
     unsigned long oldtime2 = 0;
     double diffv1=0;
     double diffv2=0;
      
     char ch;
     double scale;
     double angle=0.0; 
     double posdold2=0;
     double  posdold1=0;
     int flag=0;
 int flag222=0;
 
void setup()
{ 
      Serial.begin(9600);                 
      //encoder 1 pinModes
      pinMode(encoder1PinA, INPUT); 
      digitalWrite(encoder1PinA, HIGH);       // turn on pullup resistor
      pinMode(encoder1PinB, INPUT); 
      digitalWrite(encoder1PinB, HIGH);       // turn on pullup resistor  
      attachInterrupt(0, doEncoder1A, CHANGE);  // encoder track A on interrupt 0 - pin 2
      attachInterrupt(1, doEncoder1B, CHANGE);  // encoder track B on interrupt 1 - pin 3
     
    //encoder 2 pinModes  
      pinMode(encoder2PinA, INPUT); 
      digitalWrite(encoder2PinA, HIGH);       // turn on pullup resistor
      pinMode(encoder2PinB, INPUT); 
      digitalWrite(encoder2PinB, HIGH);       // turn on pullup resistor  
      attachInterrupt(2, doEncoder2A, CHANGE);  // encoder track A on interrupt 2 - pin 21
      attachInterrupt(3, doEncoder2B, CHANGE);  // encoder track B on interrupt 3 - pin 20
      
 } 
    
 void loop()
 {
      newposition1 = encoder1Pos;
      newtime1 = millis();
      newposition2 = encoder2Pos;
      newtime2 = millis();
      
      if(newtime1!=oldtime1)
        {   diffv1=(double(newposition1-oldposition1))/(double(newtime1-oldtime1));      }
      if(newtime2!=oldtime2)
        {   diffv2=(double(newposition2-oldposition2))/(double(newtime2-oldtime2));      }
     // Serial.println('a'+String(diffv1*100.0));
      
     if(Serial.available())
     {   ch = Serial.read();
         // send values
         if(ch == 's')
         { Serial.println('a'+String(diffv1*100.0)+'b'+String(diffv2*100.0)+'c');        }
         
         // reset values
         if(ch == 'r')
         {
           flag=0;           
           lastrpm1 = 0;
           rpm1 = 0;
           accel1 = 0;
           posd1=0;
           vel1=0;
           angular_velocity1=0;
           newposition1=0;
           oldposition1 = 0;
           encoder1Pos=0;
           
           lastrpm2 = 0;
           rpm2 = 0;
           accel2 = 0;
           posd2=0;
           vel2=0;
           angular_velocity2=0;
           newposition2=0;
           oldposition2 = 0;
           encoder2Pos=0;
         }
     } 
      oldposition1 = newposition1;
      oldtime1 = newtime1;
      oldposition2 = newposition2;
      oldtime2 = newtime2;
      posdold1=posd1;
      posdold2=posd2;
      Serial.flush();  
}


//Encoder 1    
    void doEncoder1A(){                        //every time a change happens on encoder pin A doEncoder will run.
      nA1 = digitalRead(encoder1PinA);
      nB1 = digitalRead(encoder1PinB);
      if ((encoder1PinALast == LOW) && (nA1 == HIGH)) 
      {    if (nB1 == LOW) { encoder1Pos--;} 
           else { encoder1Pos++;}
       } 
      encoder1PinALast = nA1;
    }
    
    void doEncoder1B(){                        //every time a change happens on encoder pin A doEncoder will run.
      nA1 = digitalRead(encoder1PinA);
      nB1 = digitalRead(encoder1PinB);
      if ((encoder1PinBLast == LOW) && (nB1 == HIGH)) 
      {    if (nA1 == LOW) { encoder1Pos++;} 
           else { encoder1Pos--;}    
      } 
      encoder1PinBLast = nB1;
    }
    
// Encoder 2
    void doEncoder2A(){                        //every time a change happens on encoder pin A doEncoder will run.
      nA2 = digitalRead(encoder2PinA);
      nB2 = digitalRead(encoder2PinB);
      if ((encoder2PinALast == LOW) && (nA2 == HIGH)) 
      {    if (nB2 == LOW) { encoder2Pos--;} 
           else { encoder2Pos++;}
       } 
      encoder2PinALast = nA2;
    }
    
    void doEncoder2B(){                        //every time a change happens on encoder pin A doEncoder will run.
      nA2 = digitalRead(encoder2PinA);
      nB2 = digitalRead(encoder2PinB);
      if ((encoder2PinBLast == LOW) && (nB2 == HIGH)) 
      {    if (nA2 == LOW) { encoder2Pos++;} 
           else { encoder2Pos--;}    
      } 
      encoder2PinBLast = nB2;
    }
