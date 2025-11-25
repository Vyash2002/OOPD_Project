#include "SimulationScenario.h"

SimulationScenario::SimulationScenario(const ProtocolConfig& cfg,
                                       const CellularCore& coreTemplate)
    : protocol(cfg),
      core(coreTemplate),
      tower(),
      users(0),
      userCount(0),
      maxUsersByCore(0),
      maxUsersBySpectrum(0),
      finalSupportedUsers(0),
      coresRequired(0) {}

SimulationScenario::~SimulationScenario() {
    if (users != 0) {
        delete[] users;
        users = 0;
    }
}

void SimulationScenario::run() {
    double totalMHz = protocol.totalBandwidthMHz;
    double chKHz = protocol.channelWidthKHz;
    int baseUsers = protocol.baseUsersPerChannel;
    int antennas = protocol.numAntennas;
    int msgPerUser = protocol.messagesPerUser;

    tower.configure(totalMHz, chKHz, baseUsers, antennas);

    maxUsersBySpectrum = tower.getMaxUsersBySpectrum();
    maxUsersByCore = core.computeMaxUsers(msgPerUser);

    if (maxUsersBySpectrum <= 0 || maxUsersByCore <= 0) {
        finalSupportedUsers = 0;
        coresRequired = 0;
        if (users != 0) {
            delete[] users;
            users = 0;
        }
        userCount = 0;
        return;
    }

    // final users = min(core limit, spectrum limit)
    if (maxUsersByCore < maxUsersBySpectrum) {
        finalSupportedUsers = maxUsersByCore;
    } else {
        finalSupportedUsers = maxUsersBySpectrum;
    }

    // integer ceil: (a + b - 1) / b
    if (maxUsersByCore > 0) {
        coresRequired = (maxUsersBySpectrum + maxUsersByCore - 1) / maxUsersByCore;
    } else {
        coresRequired = 0;
    }

    if (users != 0) {
        delete[] users;
    }
    users = new UserDevice[finalSupportedUsers];
    userCount = finalSupportedUsers;

    int i;
    for (i = 0; i < userCount; ++i) {
        users[i] = UserDevice(i + 1, msgPerUser);
    }

    tower.assignUsers(users, userCount);
}

void SimulationScenario::printSummary(IO& io) const {
    io.println("----------------------------------------");
    io.print(" Simulation Summary (");
    io.print(protocol.name);
    io.println(")");
    io.println("----------------------------------------");

    // We only have integer printing; cast doubles to int for display
    int bwMHzInt = (int)(protocol.totalBandwidthMHz);
    int chKHzInt = (int)(protocol.channelWidthKHz);

    io.print("Total bandwidth (MHz)        : ");
    io.printInt("", bwMHzInt);

    io.print("Channel width (kHz)          : ");
    io.printInt("", chKHzInt);

    io.printInt("Base users per channel       : ", protocol.baseUsersPerChannel);
    io.printInt("Number of antennas           : ", protocol.numAntennas);
    io.printInt("Messages per user            : ", protocol.messagesPerUser);

    io.printInt("Max users by spectrum        : ", maxUsersBySpectrum);
    io.printInt("Max users by core            : ", maxUsersByCore);
    io.printInt("=> Users supported           : ", finalSupportedUsers);
    io.printInt("Number of channels           : ", tower.getNumberOfChannels());
    io.printInt("Effective users per channel  : ", tower.getEffectiveUsersPerChannel());
    io.printInt("Cores needed for full spectrum: ", coresRequired);

    io.println("");
    io.println("Channel summary:");
    int ch;
    for (ch = 0; ch < tower.getNumberOfChannels(); ++ch) {
        int cnt = tower.getChannelUserCount(ch);
        io.print("  Channel ");
        io.printInt("", ch);
        io.print("  users: ");
        io.printInt("", cnt);
    }
}

void SimulationScenario::printChannelUsers(int channelIndex, IO& io) const {
    int count = tower.getChannelUserCount(channelIndex);
    if (count <= 0) {
        io.println("No users or invalid channel.");
        return;
    }

    io.print("Users on channel ");
    io.printInt("", channelIndex);
    io.println(":");

    int i;
    for (i = 0; i < count; ++i) {
        int id = tower.getChannelUserId(channelIndex, i);
        if (id > 0) {
            io.printInt("  User ID: ", id);
        }
    }
}
