#include "Ferry.h"

void Ferry::enterFerry(int carID, int direction) {
    pthread_mutex_lock(&mut);
    pthread_cond_t &waitCond = direction == 0 ? waitCond_0 : waitCond_1;
    pthread_cond_t &onMoveCond = direction == 0 ? onMoveCond_0 : onMoveCond_1;
    bool &canPass = direction == 0 ? canPass_0 : canPass_1;
    int &carsOnFerry = direction == 0 ? carsOnFerry_0 : carsOnFerry_1;
    carsOnFerry++;
    if (carsOnFerry == capacity) { // OR max_wait_time is reached TODO
        canPass = true;
        carsOnFerry = 0;
        pthread_cond_broadcast(&waitCond);
    }
    while (!canPass) {
        pthread_cond_wait(&waitCond, &mut);
    }
    pthread_mutex_unlock(&mut);

    WriteOutput(carID, 'F', id, START_PASSING);
    sleep_milli(travel_time);
    WriteOutput(carID, 'F', id, FINISH_PASSING);
    pthread_mutex_lock(&mut);
    carsOnFerry--;
    if (carsOnFerry == 0) {
        canPass = false;
        pthread_cond_broadcast(&onMoveCond);
    }
    pthread_mutex_unlock(&mut);
}
