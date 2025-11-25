CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET   = simulator

SRCS = main.cpp \
       UserDevice.cpp \
       CellularCore.cpp \
       CellTower.cpp \
       ProtocolConfig.cpp \
       SimulationScenario.cpp \
       basicIO.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
