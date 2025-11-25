#ifndef SIMULATIONSCENARIO_H
#define SIMULATIONSCENARIO_H

#include "ProtocolConfig.h"
#include "CellularCore.h"
#include "CellTower.h"
#include "UserDevice.h"
#include "IO.h"

class SimulationScenario {
private:
    ProtocolConfig protocol;
    CellularCore core;
    CellTower tower;

    UserDevice* users;   // dynamic array
    int userCount;

    int maxUsersByCore;
    int maxUsersBySpectrum;
    int finalSupportedUsers;
    int coresRequired;

public:
    SimulationScenario(const ProtocolConfig& cfg, const CellularCore& coreTemplate);
    ~SimulationScenario();

    void run();

    void printSummary(IO& io) const;
    void printChannelUsers(int channelIndex, IO& io) const;
};

#endif // SIMULATIONSCENARIO_H
