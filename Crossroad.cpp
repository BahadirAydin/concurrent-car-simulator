#include "Crossroad.h"

void Crossroad::addToQueue(int carID, int direction) {
    std::queue<int> &queue = queues[direction];
    pthread_mutex_lock(&queueMut);
    queue.push(carID);
    pthread_mutex_unlock(&queueMut);
}
void Crossroad::removeFromQueue(int carID, int direction) {
    std::queue<int> &queue = queues[direction];
    pthread_mutex_lock(&queueMut);
    if (!queue.empty() && queue.front() == carID) {
        queue.pop();
    }
    pthread_mutex_unlock(&queueMut);
}

void Crossroad::enterCrossroad(int carID, int direction) {
    std::queue<int> &currentQueue = queues[direction];
    pthread_cond_t &currentCond = conds[direction];
    pthread_mutex_lock(&mut);

    while (!neutral && (currentDirection != direction ||
                        currentQueue.front() != carID || timeout)) {
        struct timespec ts {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_wait_time / 1000;
        ts.tv_nsec += (max_wait_time % 1000) * 1000000;
        int rc = pthread_cond_timedwait(&currentCond, &mut, &ts);
        if (rc == ETIMEDOUT) {
            // wait for the bridge to be empty then change direction
            timeout = true;
            while (onBridge > 0) {
                pthread_cond_wait(&currentCond, &mut);
            }
            currentDirection = direction;
            dirChanged = true;
            timeout = false;
        }
    }
    if (!neutral && currentDirection == direction && !dirChanged) {
        sleep_milli(PASS_DELAY);
    }
    WriteOutput(carID, 'C', id, START_PASSING);
    onBridge++;

    removeFromQueue(carID, direction);
    pthread_cond_broadcast(&currentCond);
    lastCarID = carID;
    currentDirection = direction;
    dirChanged = false;
    neutral = false;
    pthread_mutex_unlock(&mut);
}
void Crossroad::leaveCrossroad(int carID, int direction) {
    pthread_mutex_lock(&mut);
    WriteOutput(carID, 'C', direction, FINISH_PASSING);
    onBridge--;
    bool allEmpty = true;
    if (lastCarID == carID) {
        for (int i = direction; i < direction + 4; i++) {
            i %= 4;
            if (!queues[i].empty()) {
                currentDirection = i;
                dirChanged = true;
                pthread_cond_broadcast(&conds[i]);
                allEmpty = false;
                break;
            }
        }
        if (allEmpty) {
            dirChanged = false;
            neutral = true;
        }
    }
    pthread_mutex_unlock(&mut);
}
