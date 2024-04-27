#ifndef FERRY_H
#define FERRY_H
#include "monitor.h"

class Ferry : public Monitor {
  public:
    int travel_time{};
    int max_wait_time{};
    int capacity{};
};

#endif
