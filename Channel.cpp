#include "Channel.h"
#include "UserDevice.h"

Channel::Channel(int indexValue, int maxUsersValue) {
    index = indexValue;
    maxUsers = maxUsersValue;
    userCount = 0;
}

int Channel::getIndex() const {
    return index;
}

int Channel::getMaxUsers() const {
    return maxUsers;
}

int Channel::getCurrentUserCount() const {
    return userCount;
}

bool Channel::addUser(UserDevice* user) {
    if (userCount >= maxUsers || userCount >= CHANNEL_MAX_USERS) {
        return false;
    }
    users[userCount++] = user;
    return true;
}

UserDevice** Channel::getUsers() {
    return users;
}

int Channel::getUserCount() const {
    return userCount;
}
