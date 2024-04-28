#ifndef NARROWBRIDGE_H
#define NARROWBRIDGE_H
#include "monitor.h"
#include <pthread.h>
#include <queue>

class NarrowBridge : public Monitor {
  private:
    std::queue<int> northQueue{};
    std::queue<int> southQueue{};
    Condition direction{this};
    int currentDirection{0};
    bool bridge_empty{true};
    pthread_mutex_t mut;

  public:
    int travel_time{};
    int max_wait_time{};
    void enterBridge(int carID, int direction);
    void leaveBridge(int carID, int direction);
    void addToQueue(int carID, int direction);
    void removeFromQueue(int carID, int direction);
    void changeDirection();
};
#endif
