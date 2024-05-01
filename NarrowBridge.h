#ifndef NARROWBRIDGE_H
#define NARROWBRIDGE_H
#include "WriteOutput.h"
#include "helper.h"
#include "monitor.h"
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <queue>

// Direction is either 0 or 1

class NarrowBridge {
  private:
    std::queue<int> oneQueue{};
    std::queue<int> zeroQueue{};
    pthread_cond_t zeroCond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t oneCond = PTHREAD_COND_INITIALIZER;
    int currentDirection{0};
    int onBridge{0};
    bool timeout{false};
    bool dirChanged{false};
    pthread_mutex_t mut;
    pthread_mutex_t queueMut;
    bool neutral{true};
    int lastCarID{};

  public:
    int id{};
    int travel_time{};
    int max_wait_time{};
    void enterBridge(int carID, int direction);
    void leaveBridge(int carID, int direction);
    void addToQueue(int carID, int direction);
    void removeFromQueue(int carID, int direction);
    void changeDirection();
};
#endif
