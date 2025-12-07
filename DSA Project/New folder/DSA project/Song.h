#ifndef SONG_H
#define SONG_H

#include <string>
using namespace std;

class Song {
public:
    int id;        // unique identifier
    string title;
    string artist;

    Song* next;  // for linked list
    Song* prev;  // for linked list

    Song(string t = "", string a = "") {
        static int nextId = 0; // local static counter to generate unique ids
        id = ++nextId;
        title = t;
        artist = a;
        next = prev = nullptr;
    }
};

#endif