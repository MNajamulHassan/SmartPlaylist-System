#ifndef SONGSTACK_H
#define SONGSTACK_H

#include "Song.h"

class SongStack {
private:
    struct StackNode {
        Song* song;
        StackNode* next;
        StackNode(Song* s) : song(s), next(nullptr) {}
    };
    StackNode* top;

public:
    SongStack() : top(nullptr) {}

    void push(Song* s) {
        StackNode* node = new StackNode(s);
        node->next = top;
        top = node;
    }

    Song* pop() {
        if (!top) return nullptr;
        Song* s = top->song;
        StackNode* temp = top;
        top = top->next;
        delete temp;
        return s;
    }

    bool isEmpty() const { return top == nullptr; }

    // Remove all occurrences of a given Song* from the stack (used when a song is deleted)
    void removeOccurrences(Song* s) {
        StackNode* cur = top;
        StackNode* prev = nullptr;
        while (cur) {
            if (cur->song == s) {
                StackNode* toDelete = cur;
                if (prev) prev->next = cur->next;
                else top = cur->next;
                cur = cur->next;
                delete toDelete;
            }
            else {
                prev = cur;
                cur = cur->next;
            }
        }
    }

    // Optional: clear stack (used by destructor if you want)
    void clear() {
        while (!isEmpty()) pop();
    }
};

#endif
