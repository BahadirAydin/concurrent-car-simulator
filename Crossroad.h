#ifndef CROSSROAD_H
#define CROSSROAD_H
#include "monitor.h"

class Crossroad : public Monitor {
  public:
    int travelTime{};
    int maxWaitTime{};
};

#endif
