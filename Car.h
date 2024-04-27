#ifndef CAR_H
#define CAR_H

#include "Crossroad.h"
#include "Ferry.h"
#include "NarrowBridge.h"
#include "WriteOutput.h"
#include "helper.h"
#include <vector>

enum class ConnectorType { CROSSROAD, NARROWBRIDGE, FERRY };

union Connector {
    NarrowBridge *narrowBridge;
    Ferry *ferry;
    Crossroad *crossroad;
};

struct Path {
    ConnectorType type;
    int connectorID;
    int from;
    int to;
};

class Car {
  public:
    int id;
    int travel_time;
    std::vector<Path> path;

    void simulate(std::vector<NarrowBridge> &narrowBridges,
                  std::vector<Ferry> &ferries,
                  std::vector<Crossroad> &crossroads);
    void passConnector(ConnectorType type, Connector connector, int from,
                       int to);
};

#endif // CAR_H
