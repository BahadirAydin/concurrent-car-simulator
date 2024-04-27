#ifndef SIMULATION_H
#define SIMULATION_H

#include "Car.h"
#include "Crossroad.h"
#include "Ferry.h"
#include "NarrowBridge.h"
#include "WriteOutput.h"
#include <iostream>
#include <vector>

struct Simulation {
    std::vector<Car> cars;
    std::vector<NarrowBridge> narrowBridges;
    std::vector<Ferry> ferries;
    std::vector<Crossroad> crossroads;
    void readSimulationInput();
    void createCarThreads();
    static auto simulateCar(void *arg) -> void *;
    void enterNarrowBridge(Car &car, NarrowBridge &narrowBridge, int direction);
};
struct CarSimulationArgs {
    Car *car;
    Simulation *simulation;
};

#endif
