#include "ProtocolConfig.h"

ProtocolConfig::ProtocolConfig()
    : name(0),
      totalBandwidthMHz(0.0),
      channelWidthKHz(0.0),
      baseUsersPerChannel(0),
      numAntennas(1),
      messagesPerUser(0) {}

// 2G: TDMA, 16 users / 200 kHz, 1 MHz total,
//     5 data + 15 voice = 20 messages per user
ProtocolConfig ProtocolConfig::make2G() {
    ProtocolConfig c;
    c.name = "2G";
    c.totalBandwidthMHz = 1.0;
    c.channelWidthKHz = 200.0;
    c.baseUsersPerChannel = 16;
    c.numAntennas = 1;
    c.messagesPerUser = 20;
    return c;
}

// 3G: 32 users / 200 kHz, 10 messages per user
ProtocolConfig ProtocolConfig::make3G() {
    ProtocolConfig c;
    c.name = "3G";
    c.totalBandwidthMHz = 1.0;
    c.channelWidthKHz = 200.0;
    c.baseUsersPerChannel = 32;
    c.numAntennas = 1;
    c.messagesPerUser = 10;
    return c;
}

// 4G: OFDM, 30 users / 10 kHz, up to 4 antennas, 10 messages per user
ProtocolConfig ProtocolConfig::make4G() {
    ProtocolConfig c;
    c.name = "4G";
    c.totalBandwidthMHz = 1.0;
    c.channelWidthKHz = 10.0;
    c.baseUsersPerChannel = 30;
    c.numAntennas = 4;
    c.messagesPerUser = 10;
    return c;
}

// 5G: 10 MHz at 1800 MHz, 30 users / 1 MHz, 16 antennas, 10 messages per user
ProtocolConfig ProtocolConfig::make5G() {
    ProtocolConfig c;
    c.name = "5G";
    c.totalBandwidthMHz = 10.0;
    c.channelWidthKHz = 1000.0;   // 1 MHz blocks
    c.baseUsersPerChannel = 30;
    c.numAntennas = 16;
    c.messagesPerUser = 10;
    return c;
}
