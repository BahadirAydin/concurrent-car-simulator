#include "NarrowBridge.h"

void NarrowBridge::addToQueue(int carID, int direction) {
    std::queue<int> &queue = (direction == 0 ? zeroQueue : oneQueue);
    pthread_mutex_lock(&queueMut);
    queue.push(carID);
    pthread_mutex_unlock(&queueMut);
}
void NarrowBridge::removeFromQueue(int carID, int direction) {
    std::queue<int> &queue = (direction == 0 ? zeroQueue : oneQueue);
    std::queue<int> &oppositeQueue = (direction == 1 ? oneQueue : zeroQueue);
    pthread_mutex_lock(&queueMut);
    if (!queue.empty() && queue.front() == carID) {
        queue.pop();
    }
    pthread_mutex_unlock(&queueMut);
}

void NarrowBridge::enterBridge(int carID, int direction) {
    std::queue<int> &currentQueue = (direction == 0 ? zeroQueue : oneQueue);
    std::queue<int> &oppositeQueue = (direction == 1 ? oneQueue : zeroQueue);
    pthread_mutex_lock(&mut);

    while (!neutral && (currentDirection != direction ||
                        currentQueue.front() != carID || timeout)) {
        struct timespec ts {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_wait_time / 1000;
        ts.tv_nsec += (max_wait_time % 1000) * 1000000;
        int rc = pthread_cond_timedwait(direction == 0 ? &zeroCond : &oneCond,
                                        &mut, &ts);
        if (rc == ETIMEDOUT) {
            // wait for the bridge to be empty then change direction
            timeout = true;
            while (onBridge > 0) {
                pthread_cond_wait(direction == 0 ? &zeroCond : &oneCond, &mut);
            }
            currentDirection = direction;
            dirChanged = true;
            timeout = false;
        }
    }
    if (!neutral && currentDirection == direction && !dirChanged) {
        sleep_milli(PASS_DELAY);
    }
    WriteOutput(carID, 'N', direction, START_PASSING);
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
    WriteOutput(carID, 'N', direction, FINISH_PASSING);
    onBridge--;
    if (lastCarID == carID && zeroQueue.empty() && oneQueue.empty()) {
        dirChanged = false;
        neutral = true;
        pthread_cond_broadcast(&zeroCond);
        pthread_cond_broadcast(&oneCond);
    } else if (lastCarID == carID && zeroQueue.empty()) {
        dirChanged = true;
        currentDirection = 1;
        pthread_cond_broadcast(&oneCond);
    } else if (lastCarID == carID && oneQueue.empty()) {
        dirChanged = true;
        currentDirection = 0;
        pthread_cond_broadcast(&zeroCond);
    }
    pthread_mutex_unlock(&mut);
}
