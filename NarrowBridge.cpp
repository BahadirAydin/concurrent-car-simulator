#include "NarrowBridge.h"
#include <iostream>

void NarrowBridge::enterBridge(int carID, int direction) {
    __synchronized__;
    std::queue<int> &currentQueue = (direction == 0 ? northQueue : southQueue);
    std::queue<int> &oppositeQueue = (direction == 1 ? northQueue : southQueue);

    if (currentQueue.empty() && oppositeQueue.empty()) {
        this->currentDirection = direction;
    }

    currentQueue.push(carID);

    while (this->currentDirection != direction) {
        this->direction.wait();
    }
}

void NarrowBridge::leaveBridge(int carID, int direction) {
    __synchronized__;
    std::queue<int> &currentQueue = (direction == 0 ? northQueue : southQueue);
    if (!currentQueue.empty() && currentQueue.front() == carID) {
        currentQueue.pop();
    }
    if (currentQueue.empty()) {
        this->currentDirection = !this->currentDirection;
        this->direction.notifyAll();
    }
}
