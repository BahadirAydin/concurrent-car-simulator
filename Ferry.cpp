#include "Ferry.h"

void Ferry::enterFerry(int carID, int direction) {
    pthread_mutex_lock(&mut);
    pthread_cond_t &waitCond = direction == 0 ? waitCond_0 : waitCond_1;
    pthread_cond_t &waitToBoard =
        direction == 0 ? waitToBoard_0 : waitToBoard_1;
    bool &canPass = direction == 0 ? canPass_0 : canPass_1;
    int &carsOnFerry = direction == 0 ? carsOnFerry_0 : carsOnFerry_1;
    int &totalCarsCarried =
        direction == 0 ? totalCarsCarried_0 : totalCarsCarried_1;

    while (totalCarsCarried > 0) {
        pthread_cond_wait(&waitToBoard, &mut);
    }
    carsOnFerry++;
    if (carsOnFerry == capacity) {
        canPass = true;
        totalCarsCarried = capacity;
        pthread_cond_broadcast(&waitCond);
        carsOnFerry = 0;
    }
    while (!canPass) {
        struct timespec ts {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_wait_time / 1000;
        ts.tv_nsec += (max_wait_time % 1000) * 1000000;
        int rc = pthread_cond_timedwait(&waitCond, &mut, &ts);
        if (rc == ETIMEDOUT) {
            canPass = true;
            totalCarsCarried = carsOnFerry;
            pthread_cond_broadcast(&waitCond);
            carsOnFerry = 0;
        }
    }
    totalCarsCarried--;
    if (totalCarsCarried == 0) {
        canPass = false;
        pthread_cond_broadcast(&waitToBoard);
    }
    pthread_mutex_unlock(&mut);
    WriteOutput(carID, 'F', id, START_PASSING);
    sleep_milli(travel_time);
    WriteOutput(carID, 'F', id, FINISH_PASSING);
}
