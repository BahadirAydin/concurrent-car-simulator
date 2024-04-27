#ifndef NARROWBRIDGE_H
#define NARROWBRIDGE_H
#include "monitor.h"

class NarrowBridge : public Monitor {
  public:
    int travelTime{};
    int maxWaitTime{};
};
#endif
