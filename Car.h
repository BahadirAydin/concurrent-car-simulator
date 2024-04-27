#ifndef CAR_H
#define CAR_H

#include "WriteOutput.h"
#include "helper.h"
#include <vector>

enum class ConnectorType { CROSSROAD, NARROWBRIDGE, FERRY };

struct Path {
    ConnectorType type;
    int connectorID;
    int from;
    int to;
};

struct Car {
    int travel_time;
    std::vector<Path> path;
};
#endif
