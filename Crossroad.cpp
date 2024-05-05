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
    std::queue<int> *currentQueue = &queues[direction];
    pthread_cond_t *currentCond = &conds[direction];
    pthread_mutex_lock(&mut);

    while (true) {
        pthread_mutex_lock(&queueMut);
        bool waitCond = !neutral && (currentDirection != direction ||
                                     currentQueue->front() != carID || timeout);
        pthread_mutex_unlock(&queueMut);
        if (!waitCond) {
            break;
        }
        struct timespec ts {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_wait_time / 1000;
        ts.tv_nsec += (max_wait_time % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += ts.tv_nsec / 1000000000;
            ts.tv_nsec %= 1000000000;
        }
        int rc = pthread_cond_timedwait(currentCond, &mut, &ts);
        if (rc == ETIMEDOUT) {
            // wait for the bridge to be empty then change direction
            timeout = true;
            while (onBridge > 0) {
                pthread_cond_wait(currentCond, &mut);
            }
            currentDirection = direction;
            currentQueue = &queues[direction];
            currentCond = &conds[direction];
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
    pthread_cond_broadcast(currentCond);
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
        pthread_mutex_lock(&queueMut);
        int j = direction;
        for (int i = 0; i < 4; i++) {
            j %= 4;
            bool empty = queues[j].empty();
            if (!empty) {
                currentDirection = j;
                dirChanged = true;
                pthread_cond_broadcast(&conds[j]);
                allEmpty = false;
                break;
            }
            j++;
        }
        if (allEmpty) {
            dirChanged = false;
            neutral = true;
            for (int i = 0; i < 4; i++) {
                pthread_cond_broadcast(&conds[i]);
            }
        }
        pthread_mutex_unlock(&queueMut);
    }
    pthread_mutex_unlock(&mut);
}
