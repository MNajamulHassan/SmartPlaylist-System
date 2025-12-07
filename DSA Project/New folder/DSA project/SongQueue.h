#ifndef SONGQUEUE_H
#define SONGQUEUE_H

#include "Song.h"

class SongQueue {
private:
    struct QueueNode {
        Song* song;
        QueueNode* next;
        QueueNode(Song* s) : song(s), next(nullptr) {}
    };
    QueueNode* front;
    QueueNode* rear;

public:
    SongQueue() { front = rear = nullptr; }

    void enqueue(Song* s) {
        QueueNode* node = new QueueNode(s);
        if (!rear) {
            front = rear = node;
            return;
        }
        rear->next = node;
        rear = node;
    }

    Song* dequeue() {
        if (!front) return nullptr;
        QueueNode* temp = front;
        Song* s = temp->song;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        return s;
    }

    bool isEmpty() { return front == nullptr; }
};

#endif
