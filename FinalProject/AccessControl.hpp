#ifndef ACCESS_CONTROL_HPP
#define ACCESS_CONTROL_HPP

#include "models.hpp"
#include "Repository.hpp"
#include "EventBus.hpp"
#include "PermissionUtility.hpp"

constexpr int MAX_USERS = 20;
constexpr int MAX_LOGS = 100;
constexpr int MAX_READERS = 20;
constexpr int MAX_ATTENDANCE = 20;

Repository<Attendance, MAX_ATTENDANCE> attendanceRepo;
Repository<User, MAX_USERS> usersRepo;
Repository<Log, MAX_LOGS> logsRepo;
Repository<Reader, MAX_READERS> readersRepo;

EventBus<Log, 5> logBus;

Action getNextAction(const char* cardId, int readerId) {
    for (int i = logsRepo.size() - 1; i >= 0; --i) {
        Log* l = logsRepo.get(i);
        if (!l) continue;
        if (strcmp(l->cardId, cardId) == 0 && l->readerId == readerId) {
            // Found the latest swipe at this reader
            return (l->action == Action::IN) ? Action::OUT : Action::IN;
        }
    }
    return Action::IN; // first swipe at this reader
}

// Custom swipe function for testing with manual timestamps
void swipe_card_at(const char* cardId, int readerId, unsigned long timestamp) {
    Reader* reader = readersRepo.find([readerId](const Reader& r){ return r.id == readerId; });
    if (!reader) return;

    User* user = usersRepo.find([cardId](const User& u){ return strcmp(u.cardId, cardId) == 0; });

    Log log;
    strncpy(log.cardId, cardId, sizeof(log.cardId));
    log.cardId[sizeof(log.cardId) - 1] = '\0';
    log.readerId = readerId;
    log.timestamp = timestamp;
    log.action = getNextAction(cardId, readerId);

    if (!user) {
        log.status = Status::ID_NOT_FOUND;
    } else if (timestamp > user->accessExpiry) {
        log.status = Status::EXPIRED;
    } else if (!PermissionUtility::hasPermission(user->permission, reader->id)) {
        log.status = Status::UNAUTHORIZED;
    } else {
        log.status = Status::AUTHORIZED;
    }

    logsRepo.add(log);
    logBus.publish(log);
}

void swipe_card(const char* cardId, int readerId) {
    Reader* reader = readersRepo.find([readerId](const Reader& r){ return r.id == readerId; });
    if (!reader) return;

    User* user = usersRepo.find([cardId](const User& u){ return strcmp(u.cardId, cardId) == 0; });

    Log log;
    strncpy(log.cardId, cardId, sizeof(log.cardId));
    log.cardId[sizeof(log.cardId) - 1] = '\0';
    log.readerId = readerId;
    log.timestamp = static_cast<unsigned long>(time(nullptr));
    log.action = getNextAction(cardId, readerId);

    if (!user) {
        log.status = Status::ID_NOT_FOUND;
    } else if (log.timestamp > user->accessExpiry) {
        log.status = Status::EXPIRED;
    } else if (!PermissionUtility::hasPermission(user->permission, reader->id)) {
        log.status = Status::UNAUTHORIZED;
    } else {
        log.status = Status::AUTHORIZED;
    }

    logsRepo.add(log);
    logBus.publish(log);
}

#endif
