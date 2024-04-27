#include "Simulation.h"
#include "WriteOutput.h"

auto main() -> int {
    InitWriteOutput();
    Simulation sim;
    sim.readSimulationInput();
    sim.createCarThreads();
    return 0;
}
