#ifndef CELLTOWER_H
#define CELLTOWER_H

#include "UserDevice.h"

#define MAX_CHANNELS           128
#define MAX_USERS_PER_CHANNEL  512

class CellTower {
private:
    class Channel {
    private:
        int index;
        int maxUsers;
        int currentUsers;
        UserDevice* users[MAX_USERS_PER_CHANNEL];

    public:
        Channel();
        void init(int idx, int capacity);

        int getIndex() const;
        int getMaxUsers() const;
        int getCurrentUserCount() const;
        bool addUser(UserDevice* user);
        UserDevice* getUserAt(int pos) const;
    };

    double totalBandwidthMHz;
    double channelWidthKHz;
    int baseUsersPerChannel;
    int numAntennas;

    int numChannels;
    Channel channels[MAX_CHANNELS];

    void createChannels();

public:
    CellTower();
    void configure(double totalBandwidthMHz,
                   double channelWidthKHz,
                   int baseUsersPerChannel,
                   int numAntennas);

    int getNumberOfChannels() const;
    int getEffectiveUsersPerChannel() const; // baseUsersPerChannel * numAntennas
    int getMaxUsersBySpectrum() const;

    void assignUsers(UserDevice* users, int userCount);

    int getChannelUserCount(int channelIndex) const;
    int getChannelUserId(int channelIndex, int position) const;
};

#endif // CELLTOWER_H
