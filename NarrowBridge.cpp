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
    while (!neutral &&
           (currentDirection != direction || currentQueue.front() != carID)) {
        pthread_cond_wait(direction == 0 ? &zeroCond : &oneCond, &mut);
    }
    if (!neutral && currentDirection == direction) {
        sleep_milli(PASS_DELAY);
    }
    WriteOutput(carID, 'N', direction, START_PASSING);

    removeFromQueue(carID, direction);
    pthread_cond_broadcast(direction == 0 ? &zeroCond : &oneCond);
    lastCarID = carID;
    currentDirection = direction;
    neutral = false;
    pthread_mutex_unlock(&mut);
}
void NarrowBridge::leaveBridge(int carID, int direction) {
    pthread_mutex_lock(&mut);
    WriteOutput(carID, 'N', direction, FINISH_PASSING);
    if (lastCarID == carID && zeroQueue.empty() && oneQueue.empty()) {
        neutral = true;
        pthread_cond_broadcast(&zeroCond);
        pthread_cond_broadcast(&oneCond);
    } else if (lastCarID == carID && zeroQueue.empty()) {
        currentDirection = 1;
        pthread_cond_broadcast(&oneCond);
    } else if (lastCarID == carID && oneQueue.empty()) {
        currentDirection = 0;
        pthread_cond_broadcast(&zeroCond);
    }
    pthread_mutex_unlock(&mut);
}
