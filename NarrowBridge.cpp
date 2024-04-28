#include "NarrowBridge.h"

void NarrowBridge::enterBridge(int carID, int direction) {
    __synchronized__;
    std::queue<int> &currentQueue = (direction == 0 ? northQueue : southQueue);
    std::queue<int> &oppositeQueue = (direction == 1 ? northQueue : southQueue);

    while (this->currentDirection != direction || !this->bridge_empty) {
        this->direction.wait();
    }

    this->bridge_empty = false;
    this->currentDirection = direction;
}

void NarrowBridge::addToQueue(int carID, int direction) {
    std::queue<int> &queue = (direction == 0 ? northQueue : southQueue);
    pthread_mutex_lock(&mut);
    queue.push(carID);
    pthread_mutex_unlock(&mut);
}
void NarrowBridge::removeFromQueue(int carID, int direction) {
    std::queue<int> &queue = (direction == 0 ? northQueue : southQueue);
    pthread_mutex_lock(&mut);
    if (!queue.empty() && queue.front() == carID) {
        queue.pop();
    }
    pthread_mutex_unlock(&mut);
}

void NarrowBridge::leaveBridge(int carID, int direction) {
    __synchronized__;
    std::queue<int> &currentQueue = (direction == 0 ? northQueue : southQueue);
    if (currentQueue.empty()) {
        this->bridge_empty = true;
        this->currentDirection = 1 - this->currentDirection;
        this->direction.notifyAll();
    }
}
