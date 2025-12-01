#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>


// Piezo
#define BUZ_PIN 2
// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// MFRC
#define SS_PIN 10
#define RST_PIN 9

byte readCard[4];
String tagID = "";
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
  // put your setup code here, to run once:

  // Setup SPI & MFRC
  Serial.begin(115200);

  // Setup Buzzer
  pinMode(BUZ_PIN, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  // Setup LCD
  lcd.init();
  lcd.backlight();
}

void loop() {
  // put your main code here, to run repeatedly:

  while(readID()) {
    Serial.println(tagID);
    tone(BUZ_PIN, 2500, 150);
    lcd.setCursor(3,0); 
    lcd.print(tagID);
  }
}


//Read new tag if available
boolean readID()
{
  //Check if a new tag is detected or not. If not return.
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }
  //Check if a new tag is readable or not. If not return.
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }
  tagID = "";
  // Read the 4 byte UID
  for ( uint8_t i = 0; i < 4; i++)
  {
    //readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Convert the UID to a single String
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading


  Serial.println("Test 2 True");

  return true;
}