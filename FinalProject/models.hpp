#ifndef MODELS_HPP
#define MODELS_HPP

#include <string.h>
#include <stdint.h>

enum class Action : uint8_t {
    IN,
    OUT,
    UNKNOWN
};

enum class Status : uint8_t {
    AUTHORIZED,
    UNAUTHORIZED,
    ID_NOT_FOUND,
    EXPIRED
};

struct Location {
    int id;
    char buildingName[30];
    char roomName[15];

    Location(int id_ = 0, const char* buildingName_ = "", const char* roomName_ = "")
        : id(id_) {
        strncpy(buildingName, buildingName_, sizeof(buildingName));
        buildingName[sizeof(buildingName) - 1] = '\0';
        strncpy(roomName, roomName_, sizeof(roomName));
        roomName[sizeof(roomName) - 1] = '\0';
    }
};

struct User {
    int id;
    char college[25];
    unsigned long accessExpiry;
    int studentId;
    char cardId[8];
    char firstName[15];
    char middleName[10];
    char lastName[15];
    uint8_t permission;

    User(int id_ = 0, const char* college_ = "", unsigned long accessExpiry_ = 0,
         const char* cardId_ = "", int studentId_ = 0, const char* firstName_ = "",
         const char* middleName_ = "", const char* lastName_ = "", uint8_t permission_ = 0)
        : id(id_), accessExpiry(accessExpiry_), studentId(studentId_), permission(permission_) {
        strncpy(college, college_, sizeof(college));
        college[sizeof(college) - 1] = '\0';
        strncpy(cardId, cardId_, sizeof(cardId));
        cardId[sizeof(cardId) - 1] = '\0';
        strncpy(firstName, firstName_, sizeof(firstName));
        firstName[sizeof(firstName) - 1] = '\0';
        strncpy(middleName, middleName_, sizeof(middleName));
        middleName[sizeof(middleName) - 1] = '\0';
        strncpy(lastName, lastName_, sizeof(lastName));
        lastName[sizeof(lastName) - 1] = '\0';
    }
};

struct Reader {
    int id;
    int locationId;
    char name[20];

    Reader(int id_ = 0, int locationId_ = 0, const char* name_ = "")
        : id(id_), locationId(locationId_) {
        strncpy(name, name_, sizeof(name));
        name[sizeof(name) - 1] = '\0';
    }
};

struct Log {
    int id;
    char cardId[8];
    int readerId;
    unsigned long timestamp;
    Action action;
    Status status;

    Log(int id_ = 0, const char* cardId_ = "", int readerId_ = 0, unsigned long timestamp_ = 0,
        Action action_ = Action::UNKNOWN, Status status_ = Status::UNAUTHORIZED)
        : id(id_), readerId(readerId_), timestamp(timestamp_),
          action(action_), status(status_) {
        strncpy(cardId, cardId_, sizeof(cardId));
        cardId[sizeof(cardId) - 1] = '\0';
    }
};

enum class AttendanceStatus : uint8_t {
    ON_TIME,
    LATE,
    ABSENT
};

struct Attendance {
    int id;
    int readerId;
    unsigned long gracePeriod;
    unsigned long timeStart;
    unsigned long timeEnd;
    const char* attendees[20];
    int attendeeCount;

    Attendance(int id_ = 0, int readerId_ = 0, unsigned long grace_ = 0,
            unsigned long start = 0, unsigned long end = 0)
        : id(id_), readerId(readerId_), gracePeriod(grace_),
        timeStart(start), timeEnd(end), attendeeCount(0) {
        for (int i = 0; i < 20; ++i) {
            attendees[i] = nullptr;
        }
}
};

#endif
