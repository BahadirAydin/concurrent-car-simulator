#include "NarrowBridge.h"
#define OVERFLOW 1000000000

void NarrowBridge::addToQueue(int carID, int direction) {
    std::queue<int> &queue = (direction == 0 ? zeroQueue : oneQueue);
    pthread_mutex_lock(&queueMut);
    queue.push(carID);
    pthread_mutex_unlock(&queueMut);
}
void NarrowBridge::removeFromQueue(int carID, int direction) {
    std::queue<int> &queue = (direction == 0 ? zeroQueue : oneQueue);
    pthread_mutex_lock(&queueMut);
    if (!queue.empty() && queue.front() == carID) {
        queue.pop();
    }
    pthread_mutex_unlock(&queueMut);
}

void NarrowBridge::enterBridge(int carID, int direction) {
    std::queue<int> *currentQueue = (direction == 0 ? &zeroQueue : &oneQueue);
    pthread_mutex_lock(&mut);

    while (true) {
        pthread_mutex_lock(&queueMut);
        int frontID = currentQueue->front();
        pthread_mutex_unlock(&queueMut);
        bool waitCond = !neutral && (currentDirection != direction ||
                                     frontID != carID || timeout);
        if (!waitCond) {
            break;
        }
        struct timespec ts {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_wait_time / 1000;
        ts.tv_nsec += (max_wait_time % 1000) * 1000000;
        if (ts.tv_nsec >= OVERFLOW) {
            ts.tv_sec += ts.tv_nsec / OVERFLOW;
            ts.tv_nsec %= OVERFLOW;
        }
        int ret = pthread_cond_timedwait(direction == 0 ? &zeroCond : &oneCond,
                                         &mut, &ts);
        if (ret == ETIMEDOUT && !timeout) {
            // wait for the bridge to be empty then change direction
            timeout = true;
            while (onBridge > 0) {
                pthread_cond_wait(direction == 0 ? &zeroCond : &oneCond, &mut);
            }
            if (currentDirection != direction) {
                currentDirection = direction;
                currentQueue = (direction == 0 ? &zeroQueue : &oneQueue);
                dirChanged = true;
            }
            timeout = false;
        }
    }
    if (!neutral && currentDirection == direction && !dirChanged) {
        sleep_milli(PASS_DELAY);
    }
    WriteOutput(carID, 'N', id, START_PASSING);
    onBridge++;

    removeFromQueue(carID, direction);
    pthread_cond_broadcast(direction == 0 ? &zeroCond : &oneCond);
    lastCarID = carID;
    currentDirection = direction;
    dirChanged = false;
    neutral = false;
    pthread_mutex_unlock(&mut);
}
void NarrowBridge::leaveBridge(int carID, int direction) {
    pthread_mutex_lock(&mut);
    pthread_mutex_lock(&queueMut);
    WriteOutput(carID, 'N', id, FINISH_PASSING);
    onBridge--;
    if (onBridge == 0 && timeout) {
        pthread_cond_broadcast(currentDirection == 0 ? &oneCond : &zeroCond);
    } else if (lastCarID == carID && zeroQueue.empty() && oneQueue.empty()) {
        dirChanged = false;
        neutral = true;
    } else if (lastCarID == carID && zeroQueue.empty()) {
        dirChanged = true;
        currentDirection = 1;
        pthread_cond_broadcast(&oneCond);
    } else if (lastCarID == carID && oneQueue.empty()) {
        dirChanged = true;
        currentDirection = 0;
        pthread_cond_broadcast(&zeroCond);
    }
    pthread_mutex_unlock(&queueMut);
    pthread_mutex_unlock(&mut);
}
