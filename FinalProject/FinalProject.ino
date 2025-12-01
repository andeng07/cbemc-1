#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "AccessControl.hpp"

#define START_TIMESTAMP 1231232

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

// Attendance listener (simplified)
void attendanceListener(const Log& log) {
    // Only process authorized swipes
    if (log.status != Status::AUTHORIZED) return;

    unsigned long now = log.timestamp;

    Attendance* session = attendanceRepo.find([&log](Attendance& a){
        return log.readerId == a.readerId;
    });
    if (!session) return;

    unsigned long entryGraceStart = session->timeStart - session->gracePeriod;

    if (now < entryGraceStart) return;

    AttendanceStatus status;
    if (now <= session->timeStart) {
        status = AttendanceStatus::ON_TIME;
    } else if (now <= session->timeEnd) {
        status = AttendanceStatus::LATE;
    } else {
        status = AttendanceStatus::ABSENT;
    }

    // Lookup user name
    User* user = usersRepo.find([&log](const User& u){ return strcmp(u.cardId, log.cardId) == 0; });
    const char* name = user ? user->firstName : "UNKNOWN";

    // Format time
    char timeStr[32];
    std::time_t t = static_cast<std::time_t>(now);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    // Single-line print for each swipe (logs)
    printf("[Attendance Event] %s | CardID: %s | ReaderID: %d | Time: %s | Status: %s | Attendance: LISTED\n",
           name,
           log.cardId,
           log.readerId,
           timeStr,
           status == AttendanceStatus::ON_TIME ? "ON_TIME" :
           status == AttendanceStatus::LATE ? "LATE" : "ABSENT");
}

// Simple log printer
void onLog(const Log& log) {
    printf("[Log Event] CardID: %s, ReaderID: %d, Action: %s, Status: %s\n",
           log.cardId,
           log.readerId,
           log.action == Action::IN ? "IN" : "OUT",
           log.status == Status::AUTHORIZED ? "AUTHORIZED" :
           log.status == Status::UNAUTHORIZED ? "UNAUTHORIZED" :
           log.status == Status::EXPIRED ? "EXPIRED" : "ID_NOT_FOUND");
}

void populateData() {
  usersRepo.add(User(1, "Computer Studies", ))
}

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

  logBus.subscribe(onLog);
  logBus.subscribe(attendanceListener);

}

void loop() {
  // put your main code here, to run repeatedly:

  while(readID()) {


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