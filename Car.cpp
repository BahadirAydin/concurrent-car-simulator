#include "Car.h"
#include <iostream>

void Car::simulate(std::vector<NarrowBridge> &narrowBridges,
                   std::vector<Ferry> &ferries,
                   std::vector<Crossroad> &crossroads) {
    for (const auto &p : path) {
        char connector_type;
        Connector connector;
        switch (p.type) {
        case ConnectorType::NARROWBRIDGE:
            connector.narrowBridge = &narrowBridges[p.connectorID];
            connector_type = 'N';
            break;
        case ConnectorType::FERRY:
            connector.ferry = &ferries[p.connectorID];
            connector_type = 'F';
            break;
        case ConnectorType::CROSSROAD:
            connector.crossroad = &crossroads[p.connectorID];
            connector_type = 'C';
            break;
        }

        WriteOutput(id, connector_type, p.connectorID, TRAVEL);
        sleep_milli(this->travel_time);
        WriteOutput(id, connector_type, p.connectorID, ARRIVE);

        passConnector(p.type, connector, p.from, p.to);
    }
}

void Car::passConnector(ConnectorType type, Connector connector, int from,
                        int to) {
    switch (type) {
    case ConnectorType::NARROWBRIDGE:
        connector.narrowBridge->addToQueue(this->id, to);
        connector.narrowBridge->enterBridge(this->id, to);
        sleep_milli(connector.narrowBridge->travel_time);
        connector.narrowBridge->leaveBridge(this->id, to);
        break;
    case ConnectorType::FERRY:
        connector.ferry->enterFerry(this->id, to);
        break;
    case ConnectorType::CROSSROAD:
        break;
    }
}
