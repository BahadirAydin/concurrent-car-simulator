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
    bool resetTimer = false;
    while (true) {
        pthread_mutex_lock(&queueMut);
        int frontID = currentQueue->front();
        pthread_mutex_unlock(&queueMut);
        bool waitCond = !neutral && (currentDirection != direction ||
                                     frontID != carID || timeout);
        if (!waitCond) {
            if (!neutral && currentDirection == direction && !dirChanged) {
                pthread_mutex_unlock(&mut);
                sleep_milli(PASS_DELAY);
                pthread_mutex_lock(&mut);
            }
            pthread_mutex_lock(&queueMut);
            frontID = currentQueue->front();
            pthread_mutex_unlock(&queueMut);
            waitCond = !neutral && (currentDirection != direction ||
                                    frontID != carID || timeout);
            if (!waitCond) {
                break;
            }
        }
        struct timespec ts {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_wait_time / 1000;
        ts.tv_nsec += (max_wait_time % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += ts.tv_nsec / 1000000000;
            ts.tv_nsec %= 1000000000;
        }
        int ret = pthread_cond_timedwait(currentCond, &mut, &ts);
        if (ret == ETIMEDOUT && !timeout) {
            // wait for the bridge to be empty then change direction
            timeout = true;
            while (onBridge > 0) {
                pthread_cond_wait(currentCond, &mut);
            }
            pthread_mutex_lock(&queueMut);
            int j = currentDirection;
            bool allEmpty = true;
            for (int i = 0; i < 3; i++) {
                j++;
                j %= 4;
                bool empty = queues[j].empty();
                if (!empty) {
                    currentDirection = j;
                    currentQueue = &queues[j];
                    currentCond = &conds[j];
                    dirChanged = true;
                    allEmpty = false;
                    break;
                }
            }
            pthread_mutex_unlock(&queueMut);
            if (allEmpty) {
                dirChanged = false;
                neutral = true;
            }
            for (int i = 0; i < 4; i++) {
                pthread_cond_broadcast(&conds[i]);
            }
            timeout = false;
            pthread_cond_broadcast(currentCond);
        }
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
    WriteOutput(carID, 'C', id, FINISH_PASSING);
    onBridge--;
    bool allEmpty = true;
    if (onBridge == 0 && timeout) {
        for (int i = 0; i < 4; i++) {
            pthread_cond_broadcast(&conds[i]);
        }
    } else if (lastCarID == carID) {
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
        }
        pthread_mutex_unlock(&queueMut);
    }
    pthread_mutex_unlock(&mut);
}
