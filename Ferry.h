#ifndef FERRY_H
#define FERRY_H
#include "WriteOutput.h"
#include "helper.h"
#include <ctime>
#include <iostream>
#include <pthread.h>

class Ferry {
  public:
    int travel_time{};
    int max_wait_time{};
    int capacity{};
    int carsOnFerry_0{};
    int carsOnFerry_1{};
    bool canPass_0{};
    bool canPass_1{};
    int totalCarsCarried_0{};
    int totalCarsCarried_1{};
    int id{};

    void enterFerry(int carID, int direction);

  private:
    pthread_mutex_t mut;
    pthread_cond_t waitCond_0 = PTHREAD_COND_INITIALIZER;
    pthread_cond_t waitCond_1 = PTHREAD_COND_INITIALIZER;
    pthread_cond_t waitToBoard_0 = PTHREAD_COND_INITIALIZER;
    pthread_cond_t waitToBoard_1 = PTHREAD_COND_INITIALIZER;
};

#endif
