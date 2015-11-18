// Take Money when NFC seeedstudio sensor detect a any type of NFC 


#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"

PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

int redPin = 11;  //Pins RGB LED
int greenPin = 12;
int bluePin = 13;

int pwrButton1 =51 ; //boton explusion manual
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

 Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
    
  Serial.println("Waiting for an ISO14443A card");

 


}


   
void loop(){
  if ( digitalRead(button1)== HIGH) {  //boton Expulsion  --> Emergency button if nfc dont work ;) 
       take();  //Give me  Money!!
       paro();
       Serial.println("boton");
      }
  
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
    }
    Serial.println("");
    take();  //Give me  Money!!
       paro();
    // Wait 1 second before continuing
    delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
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
