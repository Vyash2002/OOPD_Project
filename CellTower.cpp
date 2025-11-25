#include "CellTower.h"

CellTower::Channel::Channel()
    : index(0), maxUsers(0), currentUsers(0) {}

void CellTower::Channel::init(int idx, int capacity) {
    int i;
    index = idx;
    maxUsers = capacity;
    currentUsers = 0;
    if (maxUsers > MAX_USERS_PER_CHANNEL) {
        maxUsers = MAX_USERS_PER_CHANNEL;
    }
    for (i = 0; i < MAX_USERS_PER_CHANNEL; ++i) {
        users[i] = 0;
    }
}

int CellTower::Channel::getIndex() const {
    return index;
}

int CellTower::Channel::getMaxUsers() const {
    return maxUsers;
}

int CellTower::Channel::getCurrentUserCount() const {
    return currentUsers;
}

bool CellTower::Channel::addUser(UserDevice* user) {
    if (currentUsers >= maxUsers) {
        return false;
    }
    users[currentUsers] = user;
    currentUsers++;
    return true;
}

UserDevice* CellTower::Channel::getUserAt(int pos) const {
    if (pos < 0 || pos >= currentUsers) {
        return 0;
    }
    return users[pos];
}

// -------- CellTower --------

CellTower::CellTower()
    : totalBandwidthMHz(0.0),
      channelWidthKHz(0.0),
      baseUsersPerChannel(0),
      numAntennas(1),
      numChannels(0) {}

void CellTower::configure(double totalBandwidthMHz,
                          double channelWidthKHz,
                          int baseUsersPerChannel,
                          int numAntennas) {
    this->totalBandwidthMHz = totalBandwidthMHz;
    this->channelWidthKHz = channelWidthKHz;
    this->baseUsersPerChannel = baseUsersPerChannel;
    this->numAntennas = numAntennas;
    createChannels();
}

void CellTower::createChannels() {
    int i;
    numChannels = 0;

    if (channelWidthKHz <= 0.0 || totalBandwidthMHz <= 0.0) {
        return;
    }

    double totalKHz = totalBandwidthMHz * 1000.0;
    int possibleChannels = (int)(totalKHz / channelWidthKHz + 1e-9);

    if (possibleChannels > MAX_CHANNELS) {
        possibleChannels = MAX_CHANNELS;
    }
    if (possibleChannels < 0) {
        possibleChannels = 0;
    }

    int effectiveUsers = getEffectiveUsersPerChannel();
    for (i = 0; i < possibleChannels; ++i) {
        channels[i].init(i, effectiveUsers);
    }
    numChannels = possibleChannels;
}

int CellTower::getNumberOfChannels() const {
    return numChannels;
}

int CellTower::getEffectiveUsersPerChannel() const {
    return baseUsersPerChannel * numAntennas;
}

int CellTower::getMaxUsersBySpectrum() const {
    return getNumberOfChannels() * getEffectiveUsersPerChannel();
}

void CellTower::assignUsers(UserDevice* users, int userCount) {
    int channelIndex = 0;
    int userIndex = 0;

    while (userIndex < userCount && channelIndex < numChannels) {
        Channel* ch = &channels[channelIndex];
        if (!ch->addUser(&users[userIndex])) {
            // channel full, move to next
            channelIndex++;
        } else {
            userIndex++;
        }
    }
}

int CellTower::getChannelUserCount(int channelIndex) const {
    if (channelIndex < 0 || channelIndex >= numChannels) {
        return 0;
    }
    return channels[channelIndex].getCurrentUserCount();
}

int CellTower::getChannelUserId(int channelIndex, int position) const {
    if (channelIndex < 0 || channelIndex >= numChannels) {
        return -1;
    }
    const Channel* ch = &channels[channelIndex];
    UserDevice* u = ch->getUserAt(position);
    if (u == 0) {
        return -1;
    }
    return u->getId();
}
