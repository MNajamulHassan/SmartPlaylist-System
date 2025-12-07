#ifndef SONGSTACK_H
#define SONGSTACK_H

#include "Song.h"

class SongStack {
private:
    struct StackNode {
        Song* song;
        StackNode* next;
        StackNode* prev;
        StackNode(Song* s) : song(s), next(nullptr), prev(nullptr) {}
    };
    StackNode* top;

public:
    SongStack() { top = nullptr; }

    void push(Song* s) {
        StackNode* node = new StackNode(s);
        node->next = top;
        if (top) top->prev = node;
        top = node;
    }

    Song* pop() {
        if (!top) return nullptr;
        Song* s = top->song;
        StackNode* temp = top;
        top = top->next;
        if (top) top->prev = nullptr;
        delete temp;
        return s;
    }

    bool isEmpty() { return top == nullptr; }
};

#endif
