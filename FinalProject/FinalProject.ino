#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TimeLib.h>
#include "AccessControl.hpp"

#define START_TIMESTAMP 1764568300

// Piezo
#define BUZ_PIN 2
// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// MFRC
#define SS_PIN 10
#define RST_PIN 9

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
    
    char timeStr[32];

    sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d",
            year(now),
            month(now),
            day(now),
            hour(now),
            minute(now),
            second(now)
    );

    // Single-line print for each swipe (logs)
    printf("[Attendance Event] %s | CardID: %s | ReaderID: %d | Time: %s | Status: %s | Attendance: LISTED\n",
           name,
           log.cardId,
           log.readerId,
           "timeStr",
           status == AttendanceStatus::ON_TIME ? "ON_TIME" :
           status == AttendanceStatus::LATE ? "LATE" : "ABSENT");
}

// Simple log printer
void onLog(const Log& log) {
    // Serial.println("[Log Event] CardID: %s, ReaderID: %d, Action: %s, Status: %s\n",
    //        log.cardId,
    //        log.readerId,
    //        log.action == Action::IN ? "IN" : "OUT",
    //        log.status == Status::AUTHORIZED ? "AUTHORIZED" :
    //        log.status == Status::UNAUTHORIZED ? "UNAUTHORIZED" :
    //        log.status == Status::EXPIRED ? "EXPIRED" : "ID_NOT_FOUND");

    Serial.print("[Log Event] CardID: ");
    Serial.print(log.cardId); // %s - prints the string/char array
    Serial.print(", ReaderID: ");
    Serial.print(log.readerId); // %d - prints the integer
    Serial.print(", Action: ");
    // Ternary operator for log.action
    Serial.print(log.action == Action::IN ? "IN" : "OUT"); // %s
    Serial.print(", Status: ");
    // Nested ternary operators for log.status
    Serial.print(log.status == Status::AUTHORIZED ? "AUTHORIZED" : 
                (log.status == Status::UNAUTHORIZED ? "UNAUTHORIZED" : 
                  (log.status == Status::EXPIRED ? "EXPIRED" : "ID_NOT_FOUND"))); // %s
    Serial.println(); // Prints the final newline character (\n)
}

void populateData() {
  usersRepo.add(User(1, "Computer Studies", START_TIMESTAMP + 3 * 60, "1D4784A", 12506346, "Andrei John", "Paleracio", "Sumilang", 0b1111));
  usersRepo.add(User(2, "Computer Studies", START_TIMESTAMP + 3 * 60, "2FD9EE7", 12537675, "Bernardo III", "Romero", "Mapolon", 0b0000));
  usersRepo.add(User(3, "Computer Studies", START_TIMESTAMP, "6FB9DC7", 12537616, "Zion", "Aemar", "Tagle", 0b1111));

  readersRepo.add(Reader(101, 201, "EKR"));
  readersRepo.add(Reader(102, 203, "UH"));
  readersRepo.add(Reader(103, 202, "MRR 101"));

  int readerId = 101;

  attendanceRepo.add(Attendance(301, 102, 60, START_TIMESTAMP, START_TIMESTAMP + 360));
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

  setTime(1764568300);
  
  populateData();

  logBus.subscribe(onLog);
  logBus.subscribe(attendanceListener);
}

void loop() {
  // put your main code here, to run repeatedly:

  while(readID()) {
    char readCard[8];

    tagID.toCharArray(readCard, 8);

    readCard[7] = '\0';

    swipe_card(readCard, 101);

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

  return true;
}