#include "Protocol.h"

// 2G implementation
double Protocol2G::getTotalBandwidthMHz() const {
    // From problem: assume 1 MHz allocated
    return 1.0;
}

double Protocol2G::getChannelWidthKHz() const {
    // 200 kHz channels
    return 200.0;
}

int Protocol2G::getBaseUsersPerChannel() const {
    // TDMA: 16 users per 200 kHz
    return 16;
}

int Protocol2G::getNumAntennas() const {
    return 1;
}

int Protocol2G::getMessagesPerUser() const {
    // 5 data + 15 voice = 20 messages per user
    return 20;
}

std::string Protocol2G::getName() const {
    return "2G";
}

// 3G implementation
double Protocol3G::getTotalBandwidthMHz() const {
    // Assume same 1 MHz total for simplicity
    return 1.0;
}

double Protocol3G::getChannelWidthKHz() const {
    // 200 kHz chunks
    return 200.0;
}

int Protocol3G::getBaseUsersPerChannel() const {
    // 32 users per 200 kHz
    return 32;
}

int Protocol3G::getNumAntennas() const {
    return 1;
}

int Protocol3G::getMessagesPerUser() const {
    // given: 10 messages per user
    return 10;
}

std::string Protocol3G::getName() const {
    return "3G";
}

// 4G implementation
double Protocol4G::getTotalBandwidthMHz() const {
    // assume 1 MHz band for this problem
    return 1.0;
}

double Protocol4G::getChannelWidthKHz() const {
    // given: 10 kHz sub-channels
    return 10.0;
}

int Protocol4G::getBaseUsersPerChannel() const {
    // 30 users per 10 kHz
    return 30;
}

int Protocol4G::getNumAntennas() const {
    // up to 4 antennas (MIMO)
    return 4;
}

int Protocol4G::getMessagesPerUser() const {
    // given: 10 messages per user
    return 10;
}

std::string Protocol4G::getName() const {
    return "4G";
}

// 5G implementation
double Protocol5G::getTotalBandwidthMHz() const {
    // given: 10 MHz at 1800 MHz
    return 10.0;
}

double Protocol5G::getChannelWidthKHz() const {
    // 1 MHz blocks
    return 1000.0;
}

int Protocol5G::getBaseUsersPerChannel() const {
    // given: 30 users per 1 MHz
    return 30;
}

int Protocol5G::getNumAntennas() const {
    // massive MIMO: up to 16 antennas
    return 16;
}

int Protocol5G::getMessagesPerUser() const {
    // assume similar to 4G (10 messages)
    return 10;
}

std::string Protocol5G::getName() const {
    return "5G";
}

// Custom implementation
ProtocolCustom::ProtocolCustom(double totalBandwidthMHz,
                               double channelWidthKHz,
                               int baseUsersPerChannel,
                               int numAntennas,
                               int messagesPerUser,
                               const std::string& name)
    : totalBandwidthMHz(totalBandwidthMHz),
      channelWidthKHz(channelWidthKHz),
      baseUsersPerChannel(baseUsersPerChannel),
      numAntennas(numAntennas),
      messagesPerUser(messagesPerUser),
      name(name) {}

double ProtocolCustom::getTotalBandwidthMHz() const {
    return totalBandwidthMHz;
}

double ProtocolCustom::getChannelWidthKHz() const {
    return channelWidthKHz;
}

int ProtocolCustom::getBaseUsersPerChannel() const {
    return baseUsersPerChannel;
}

int ProtocolCustom::getNumAntennas() const {
    return numAntennas;
}

int ProtocolCustom::getMessagesPerUser() const {
    return messagesPerUser;
}

std::string ProtocolCustom::getName() const {
    return name;
}
