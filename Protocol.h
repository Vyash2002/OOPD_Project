#ifndef PROTOCOL_H
#define PROTOCOL_H

// YOU CANNOT USE std::string, so we use fixed-size char arrays
#define PROTOCOL_NAME_LEN 32

class Protocol {
public:
    virtual ~Protocol() {}

    virtual double getTotalBandwidthMHz() const = 0;
    virtual double getChannelWidthKHz() const = 0;
    virtual int getBaseUsersPerChannel() const = 0;
    virtual int getNumAntennas() const = 0;
    virtual int getMessagesPerUser() const = 0;

    // Return C-style string instead of std::string
    virtual const char* getName() const = 0;
};

// ======================= 2G =======================
class Protocol2G : public Protocol {
private:
    char name[PROTOCOL_NAME_LEN];
public:
    Protocol2G();
    virtual double getTotalBandwidthMHz() const;
    virtual double getChannelWidthKHz() const;
    virtual int getBaseUsersPerChannel() const;
    virtual int getNumAntennas() const;
    virtual int getMessagesPerUser() const;
    virtual const char* getName() const;
};

// ======================= 3G =======================
class Protocol3G : public Protocol {
private:
    char name[PROTOCOL_NAME_LEN];
public:
    Protocol3G();
    virtual double getTotalBandwidthMHz() const;
    virtual double getChannelWidthKHz() const;
    virtual int getBaseUsersPerChannel() const;
    virtual int getNumAntennas() const;
    virtual int getMessagesPerUser() const;
    virtual const char* getName() const;
};

// ======================= 4G =======================
class Protocol4G : public Protocol {
private:
    char name[PROTOCOL_NAME_LEN];
public:
    Protocol4G();
    virtual double getTotalBandwidthMHz() const;
    virtual double getChannelWidthKHz() const;
    virtual int getBaseUsersPerChannel() const;
    virtual int getNumAntennas() const;
    virtual int getMessagesPerUser() const;
    virtual const char* getName() const;
};

// ======================= 5G =======================
class Protocol5G : public Protocol {
private:
    char name[PROTOCOL_NAME_LEN];
public:
    Protocol5G();
    virtual double getTotalBandwidthMHz() const;
    virtual double getChannelWidthKHz() const;
    virtual int getBaseUsersPerChannel() const;
    virtual int getNumAntennas() const;
    virtual int getMessagesPerUser() const;
    virtual const char* getName() const;
};

// ======================= CUSTOM =======================
class ProtocolCustom : public Protocol {
private:
    double totalBandwidthMHz;
    double channelWidthKHz;
    int baseUsersPerChannel;
    int numAntennas;
    int messagesPerUser;
    char name[PROTOCOL_NAME_LEN];

public:
    ProtocolCustom(double totalBandwidthMHz,
                   double channelWidthKHz,
                   int baseUsersPerChannel,
                   int numAntennas,
                   int messagesPerUser,
                   const char* nameInput);

    virtual double getTotalBandwidthMHz() const;
    virtual double getChannelWidthKHz() const;
    virtual int getBaseUsersPerChannel() const;
    virtual int getNumAntennas() const;
    virtual int getMessagesPerUser() const;
    virtual const char* getName() const;
};

#endif // PROTOCOL_H
