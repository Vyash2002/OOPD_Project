#include "UserDevice.h"

UserDevice::UserDevice()
    : id(0), messagesRequired(0) {}

UserDevice::UserDevice(int id, int messagesRequired)
    : id(id), messagesRequired(messagesRequired) {}

int UserDevice::getId() const {
    return id;
}

int UserDevice::getMessagesRequired() const {
    return messagesRequired;
}
