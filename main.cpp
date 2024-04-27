#include "Parser.h"
#include "WriteOutput.h"

auto main() -> int {
    InitWriteOutput();
    Simulation sim;
    sim.readSimulationInput();
    return 0;
}
