#ifndef FERRY_H
#define FERRY_H
#include "monitor.h"

class Ferry : public Monitor {
  public:
    int travelTime{};
    int maxWaitTime{};
    int capacity{};
};

#endif
