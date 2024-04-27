#ifndef NARROWBRIDGE_H
#define NARROWBRIDGE_H
#include "monitor.h"
#include <pthread.h>
#include <queue>

class NarrowBridge : public Monitor {
  public:
    int travel_time{};
    int max_wait_time{};
    std::queue<int> northQueue{};
    std::queue<int> southQueue{};
    Condition direction{this};
    int currentDirection{0};

    void enterBridge(int carID, int direction);
    void leaveBridge(int carID, int direction);
    void changeDirection();
};
#endif
