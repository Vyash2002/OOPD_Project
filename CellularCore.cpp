#include "CellularCore.h"

CellularCore::CellularCore(int maxMessagesCapacity,
                           int overheadPer100Messages)
    : maxMessagesCapacity(maxMessagesCapacity),
      overheadPer100Messages(overheadPer100Messages) {}

int CellularCore::getMaxMessagesCapacity() const {
    return maxMessagesCapacity;
}

int CellularCore::getOverheadPer100Messages() const {
    return overheadPer100Messages;
}

int CellularCore::computeMaxUsers(int messagesPerUser) const {
    if (messagesPerUser <= 0 || maxMessagesCapacity <= 0) {
        return 0;
    }

    // Effective capacity considering overhead:
    // for every 100 user messages, overheadPer100Messages extra messages.
    // So user_messages = total * 100 / (100 + overhead)
    double total = static_cast<double>(maxMessagesCapacity);
    double effectiveUserMessages =
        total * 100.0 / (100.0 + static_cast<double>(overheadPer100Messages));

    if (effectiveUserMessages < 0.0) {
        effectiveUserMessages = 0.0;
    }

    int maxUsers = static_cast<int>(effectiveUserMessages / messagesPerUser);
    if (maxUsers < 0) {
        maxUsers = 0;
    }
    return maxUsers;
}
