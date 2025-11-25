# Project Overview

This project is a cellular network simulator designed to demonstrate key Object-Oriented Programming (OOP) concepts including:

Inheritance

Polymorphism

Data abstraction

Data hiding

The simulator models interaction between:

1.User Devices

2.Cell Towers

3.Cellular Core(s)

4.Protocols (2G, 3G, 4G, 5G)

5.Simulation Scenarios


# Project Structure
```
├── main.cpp
├── Makefile
├── UserDevice.h / UserDevice.cpp
├── CellularCore.h / CellularCore.cpp
├── CellTower.h / CellTower.cpp
├── Channel.h / Channel.cpp
├── Protocol.h / Protocol.cpp
├── ProtocolConfig.h / ProtocolConfig.cpp
├── SimulationScenario.h / SimulationScenario.cpp
├── csv_allocate.cpp / IO.h
└── basicIO.cpp / basicIO.h

```
# File Purpose(Short Description)

main.cpp — Runs the overall program, handles user interaction, and triggers simulation scenarios.

UserDevice.h / UserDevice.cpp — Defines a user device with an ID and message requirement.

CellularCore.h / CellularCore.cpp — Calculates core capacity, message limits, and number of cores needed.

CellTower.h / CellTower.cpp — Manages channels, spectrum allocation, and assigns users to channels.

Channel.h / Channel.cpp — Represents a single channel and stores user IDs assigned to that channel.

Protocol.h / Protocol.cpp — Abstract protocol class and protocol-specific implementations (2G, 3G, 4G, 5G, Custom).

ProtocolConfig.h / ProtocolConfig.cpp — Lightweight configuration objects to quickly create protocol settings.

SimulationScenario.h / SimulationScenario.cpp — Runs a complete simulation using a protocol, tower, and core; produces final results.

basicIO.h / basicIO.cpp — Simplified I/O utilities for printing integers, strings, and formatted output.

csv_allocate.cpp — Reads a CSV list of users from stdin, filters only 2G users, and redistributes them across fixed-capacity channels (5 channels × 16 users). Prints channel loads and assigned channels.

Makefile — Builds the entire simulator, compiling all source files into an executable.


# Running the Program 

make

./simulator

# Requirements

g++ with C++17 support

Standard Linux/Windows terminal

No STL strings allowed in protocol classes (as per project restrictions)

# Authors

Team Members : 
1. Yash Verma
2. Videh Sharma
3. Singh Tharun

