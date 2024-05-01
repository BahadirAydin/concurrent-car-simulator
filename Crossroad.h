#ifndef CROSSROAD_H
#define CROSSROAD_H
#include "WriteOutput.h"
#include "helper.h"
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <vector>

class Crossroad {
  public:
    int travel_time{};
    int max_wait_time{};
    int id{};
    void enterCrossroad(int carID, int direction);
    void leaveCrossroad(int carID, int direction);
    void addToQueue(int carID, int direction);
    void removeFromQueue(int carID, int direction);

  private:
    std::vector<std::queue<int>> queues{4};
    std::vector<pthread_cond_t> conds{4};
    int currentDirection{0};
    int onBridge{0};
    bool timeout{false};
    bool dirChanged{false};
    pthread_mutex_t mut;
    pthread_mutex_t queueMut;
    bool neutral{true};
    int lastCarID{};
};

#endif
