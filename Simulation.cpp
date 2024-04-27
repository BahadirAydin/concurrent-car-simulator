#include "Simulation.h"

void Simulation::readSimulationInput() {
    int numNarrowBridges = 0;
    int numFerries = 0;
    int numCrossroads = 0;
    int numCars = 0;

    std::cin >> numNarrowBridges;
    narrowBridges.resize(numNarrowBridges);
    for (int i = 0; i < numNarrowBridges; ++i) {
        std::cin >> narrowBridges[i].travel_time >>
            narrowBridges[i].max_wait_time;
    }

    std::cin >> numFerries;
    ferries.resize(numFerries);
    for (int i = 0; i < numFerries; ++i) {
        std::cin >> ferries[i].travel_time >> ferries[i].max_wait_time >>
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
        cars[i].id = i;
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

void Simulation::createCarThreads() {
    std::vector<pthread_t> threads(cars.size());
    for (size_t i = 0; i < cars.size(); ++i) {
        auto *args = new CarSimulationArgs{&cars[i], this};
        pthread_create(&threads[i], NULL, simulateCar, args);
    }

    for (auto &thread : threads) {
        pthread_join(thread, NULL);
    }
}

auto Simulation::simulateCar(void *arg) -> void * {
    CarSimulationArgs *args = static_cast<CarSimulationArgs *>(arg);
    Car *car = args->car;
    Simulation *simulation = args->simulation;

    car->simulate(simulation->narrowBridges, simulation->ferries,
                  simulation->crossroads);
    return nullptr;
}
