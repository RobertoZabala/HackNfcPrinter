// Take Money when NFC_seeedstudio_sensor detect a URL of a mobile.


#include "SPI.h"
#include "PN532_SPI.h"
#include "snep.h"
#include "NdefMessage.h"

PN532_SPI pn532spi(SPI, 10);
SNEP nfc(pn532spi);
uint8_t ndefBuf[128];

int redPin = 11;  //Pins RGB LED
int greenPin = 12;
int bluePin = 13;

int pwrButton1 =51 ; //boton explusion manual // dont work
int button1 =53;

int sensorPin = A2; // Detector Sensor 
int valorSensor; 
int PWMA = 7; //Speed motor 
int dirMotorA1 = 6; // Drive motor Connector 1
int dirMotorA2= 8; // Drive motor Connector 2
int velocidad = 230; //Speed (170-254)
int tiempoExpulsion = 2100;  //para que no caiga billete. Depende de velocidad de motores

unsigned long tiempo; // Vars to delays
unsigned long t_act;

void setup()
{TCCR4B = TCCR4B & 0b000 | 0x01; // High frec Timer5 Pin 5, PWM High frecuency, silent mottors ;)

//Pin Button
pinMode(pwrButton1, OUTPUT);
pinMode(button1, INPUT);
digitalWrite(pwrButton1, HIGH);

//Pins RGB Led
pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT); 

//Pins Output Motor 
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);
paro();


Serial.begin(9600);
Serial.println("NFC Peer to Peer Example - Receive Message");

}




void loop(){
  if ( digitalRead(button1)== HIGH) {  //boton Expulsion  --> Emergency button if nfc dont work . still dont work :(( 
       take();  //Give me  Money!!
       paro();
      }
      
  Serial.println("Waiting for message from Peer");
    int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf));
    if (msgSize > 0) {
        NdefMessage msg  = NdefMessage(ndefBuf, msgSize);
        msg.print();
        take();  //Give me  Money!!
        paro();
        Serial.println("\nSuccess");
    } else {
        Serial.println("Failed");
    }
    delay(3000);
 
 
 
  if ( digitalRead(button1)== HIGH) {  //boton Expulsion  --> Emergency button if nfc dont work ;) 
       take();  //Give me  Money!!
       paro();
      }
}
  

//FUNCTIONS

void adelante(){
digitalWrite (dirMotorA1,HIGH);// Turn forward Motor
digitalWrite (dirMotorA2,LOW);
analogWrite (PWMA, velocidad);
}

void atras(){
digitalWrite (dirMotorA1,LOW);// Turn backward Motor
digitalWrite (dirMotorA2,HIGH);
analogWrite (PWMA, velocidad);
}

void paro(){
digitalWrite (dirMotorA1,HIGH);// Stop Motor
digitalWrite (dirMotorA2,HIGH);
analogWrite (PWMA, 0);
}


void take(){  //TAKE A MONEY
 setColor(0,0,254); //led Blue;
 delay(500);
 setColor(254,254,254); //led White;
 valorSensor = analogRead(A2);
 tiempo= millis();
 t_act=millis();
 boolean haveMoney = true;
 
  while (valorSensor > 40 && haveMoney){  //load money;  
   valorSensor = analogRead(A2);
   t_act=millis();
   if (t_act-tiempo>5000){ haveMoney= false;}  //empty Cash protection 5 seg
   //Serial.println(haveMoney);

   atras();
   };
   
 delay(500);   //load little more 
 while (valorSensor > 40 && haveMoney){  //check again
   valorSensor = analogRead(A2);
 // Serial.println(valorSensor);
   atras();
   };
 adelante(); //Eject money!
 delay(tiempoExpulsion);
 paro();
 if (haveMoney){setColor(0,254,0);} //led green;
  else {setColor (254,0,0);}
 delay(3000);
 setColor(0,0,0); //led OFF;
}


void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
