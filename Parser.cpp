#include "Parser.h"

void Simulation::readSimulationInput() {
    int numNarrowBridges = 0;
    int numFerries = 0;
    int numCrossroads = 0;
    int numCars = 0;

    std::cin >> numNarrowBridges;
    narrowBridges.resize(numNarrowBridges);
    for (int i = 0; i < numNarrowBridges; ++i) {
        std::cin >> narrowBridges[i].travelTime >> narrowBridges[i].maxWaitTime;
    }

    std::cin >> numFerries;
    ferries.resize(numFerries);
    for (int i = 0; i < numFerries; ++i) {
        std::cin >> ferries[i].travelTime >> ferries[i].maxWaitTime >>
            ferries[i].capacity;
    }

    std::cin >> numCrossroads;
    crossroads.resize(numCrossroads);
    for (int i = 0; i < numCrossroads; ++i) {
        std::cin >> crossroads[i].travelTime >> crossroads[i].maxWaitTime;
    }

    std::cin >> numCars;
    cars.resize(numCars);
    for (int i = 0; i < numCars; ++i) {
        int carPathCount = 0;
        int carTravelTime = 0;
        std::cin >> carTravelTime >> carPathCount;
        cars[i].travel_time = carTravelTime;
        for (int j = 0; j < carPathCount; j++) {
            std::string pathCode;
            std::cin >> pathCode;
            Path path{};
            if (pathCode[0] == 'N') {
                path.type = ConnectorType::NARROWBRIDGE;
            } else if (pathCode[0] == 'F') {
                path.type = ConnectorType::FERRY;
            } else if (pathCode[0] == 'C') {
                path.type = ConnectorType::CROSSROAD;
            }
            path.connectorID = std::stoi(pathCode.substr(1));
            std::cin >> path.from >> path.to;
            cars[i].path.push_back(path);
        }
    }
}
