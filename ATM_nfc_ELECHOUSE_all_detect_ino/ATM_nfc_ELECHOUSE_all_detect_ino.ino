// take money when ELECHOUSE sensor detect any nfc (active or passive)

#include <PN532_HSU.h>  //libraries and initial.... for nfc UART
#include <PN532.h>
 PN532_HSU pn532hsu(Serial1);
 PN532 nfc(pn532hsu);

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
int tiempoExpulsion = 2000;  //para que no caiga billete. Depende de velocidad de motores

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
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
}




void loop(){
  if ( digitalRead(button1)== HIGH) {  //boton Expulsion  --> Emergency button if nfc dont work ;) 
       take();  //Give me  Money!!
       paro();
      }
 
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    take();  //Give me  Money!!
    paro();
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
	  
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	  
	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
	  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
		
        // If you want to write something to block 4 to test with, uncomment
		// the following line and this text should be read back in a minute
        // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
		
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
		  
          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
	  
      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");
		
        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
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
   Serial.println(haveMoney);

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
  setColor (254,0,0);
 delay(3000);
 setColor(0,0,0); //led OFF;
}


void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
