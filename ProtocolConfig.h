#ifndef PROTOCOLCONFIG_H
#define PROTOCOLCONFIG_H

class ProtocolConfig {
public:
    const char* name;
    double totalBandwidthMHz;
    double channelWidthKHz;
    int baseUsersPerChannel;
    int numAntennas;
    int messagesPerUser;

    ProtocolConfig();

    static ProtocolConfig make2G();
    static ProtocolConfig make3G();
    static ProtocolConfig make4G();
    static ProtocolConfig make5G();
};

#endif // PROTOCOLCONFIG_H
