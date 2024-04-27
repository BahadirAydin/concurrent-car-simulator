#ifndef PARSER_H
#define PARSER_H

#include "Car.h"
#include "Crossroad.h"
#include "Ferry.h"
#include "NarrowBridge.h"
#include "WriteOutput.h"
#include <iostream>
#include <vector>

struct Simulation {
  public:
    std::vector<Car> cars;
    std::vector<NarrowBridge> narrowBridges;
    std::vector<Ferry> ferries;
    std::vector<Crossroad> crossroads;
    void readSimulationInput();
};

#endif
