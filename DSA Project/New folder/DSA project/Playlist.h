#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <functional>
#include "Song.h"
#include "AVL.h"
#include "SongStack.h"
using namespace std;

class Playlist {
private:
    Song* head;
    Song* tail;
    Song* current;
    string name;
    int totalSongs;

    AVL songDB;           // AVL tree is master (does NOT own Song* memory)
    SongStack history;    // back stack
    SongStack forward;    // forward stack (for previous->next behavior)

    bool isAlphabetic(const string& s) {
        if (s.empty()) return false;
        for (char c : s)
            if (!isalpha((unsigned char)c) && c != ' ') return false;
        return true;
    }

public:
    Playlist(string n = "My Playlist") {
        head = tail = current = nullptr;
        name = n;
        totalSongs = 0;
    }

    void addSong(string title, string artist) {
        if (!isAlphabetic(title) || !isAlphabetic(artist)) {
            cout << "Invalid title or artist. Only letters and spaces allowed.\n";
            return;
        }

        // Duplicate check (case-insensitive)
        vector<Song*> matches = songDB.searchAll(title);
        for (Song* m : matches)
            if (m->artist == artist && m->title == title) {
                cout << "Cannot add: \"" << title << "\" by \"" << artist << "\" already exists.\n";
                return;
            }

        Song* s = new Song(title, artist);
        songDB.insert(s);

        // Append to linked list
        if (!head) { head = tail = current = s; }
        else { tail->next = s; s->prev = tail; tail = s; }

        totalSongs++;
        cout << "Added: (" << s->id << ") " << title << " - " << artist << endl;

        // Clearing forward history because new action invalidates forward history
        forward.clear();
    }

    void displayPlaylist() {
        if (!head) { cout << "Playlist empty.\n"; return; }
        cout << "--- Playlist: " << name << " ---\n";
        Song* temp = head;
        int index = 1;
        while (temp) {
            cout << index++ << ". (" << temp->id << ") " << temp->title << " - " << temp->artist << endl;
            temp = temp->next;
        }
        cout << "Total songs: " << totalSongs << "\n";
    }

    void playCurrent() {
        if (!current) current = head;
        if (!current) { cout << "No songs to play.\n"; return; }
        cout << "Now Playing: (" << current->id << ") " << current->title << " - " << current->artist << endl;
    }

    // Move forward in playlist. If forward stack exists (due to previous), pop it; otherwise normal next.
    void nextSong() {
        if (!current) current = head;
        if (!current) { cout << "No songs to play.\n"; return; }

        // If there is a forward stack (user pressed previous before), pop from forward first.
        if (!forward.isEmpty()) {
            history.push(current);
            Song* f = forward.pop();
            if (f) current = f;
            playCurrent();
            return;
        }

        if (!current->next) { cout << "End of playlist.\n"; return; }
        history.push(current);
        current = current->next;
        playCurrent();
    }

    void previousSong() {
        if (history.isEmpty()) { cout << "No previous song.\n"; return; }
        // push current to forward, then pop history
        if (current) forward.push(current);
        Song* prev = history.pop();
        if (!prev) { cout << "No previous song.\n"; return; }
        current = prev;
        playCurrent();
    }

    void searchSong(const string& title) {
        vector<Song*> matches = songDB.searchAll(title);
        if (matches.empty()) {
            cout << "Song not found: " << title << endl;
            return;
        }
        cout << "Matches for \"" << title << "\":\n";
        for (Song* s : matches)
            cout << "(" << s->id << ") " << s->title << " - " << s->artist << endl;
    }

    void deleteSongById(int id) {
        string title = songDB.findTitleById(id);
        if (title.empty()) { cout << "Song with id " << id << " not found.\n"; return; }

        // Remove from AVL first (AVL nodes deleted, Song* pointer remains)
        bool removed = songDB.removeById(id);
        if (!removed) { cout << "Error: could not remove from AVL.\n"; return; }

        // Find in linked list
        Song* temp = head;
        while (temp && temp->id != id) temp = temp->next;
        if (!temp) return;

        // Remove from history and forward stacks to avoid dangling pointers
        history.removeOccurrences(temp);
        forward.removeOccurrences(temp);

        // Update linked list pointers
        if (temp->prev) temp->prev->next = temp->next;
        else head = temp->next;

        if (temp->next) temp->next->prev = temp->prev;
        else tail = temp->prev;

        // Update current
        if (current == temp) current = temp->next ? temp->next : head;

        totalSongs--;
        cout << "Deleted: (" << temp->id << ") " << temp->title << " - " << temp->artist << endl;

        delete temp;
    }

    void reversePlaylist() {
        if (!head) return;
        Song* tmp = nullptr;
        Song* cur = head;
        while (cur) {
            tmp = cur->prev;
            cur->prev = cur->next;
            cur->next = tmp;
            cur = cur->prev;
        }
        if (tmp) head = tmp->prev;

        // Fix tail and reset current to head
        tail = head;
        while (tail && tail->next) tail = tail->next;
        current = head;

        cout << "Playlist reversed.\n";
    }

    void sortPlaylist() {
        if (!head || !head->next) return;

        vector<Song*> sorted = songDB.getInOrder();
        if (sorted.empty()) return;

        head = sorted[0];
        head->prev = nullptr;
        Song* tmp = head;
        for (size_t i = 1; i < sorted.size(); ++i) {
            tmp->next = sorted[i];
            sorted[i]->prev = tmp;
            tmp = tmp->next;
        }
        tail = tmp;
        tail->next = nullptr;

        current = head;
        cout << "Playlist sorted by title (AVL master).\n";
    }

    void shufflePlaylist() {
        if (!head || totalSongs < 2) return;

        vector<Song*> arr;
        Song* tmp = head;
        while (tmp) { arr.push_back(tmp); tmp = tmp->next; }

        random_device rd;
        mt19937 g(rd());
        std::shuffle(arr.begin(), arr.end(), g);

        head = arr[0]; head->prev = nullptr;
        for (size_t i = 1; i < arr.size(); ++i) {
            arr[i - 1]->next = arr[i];
            arr[i]->prev = arr[i - 1];
        }
        tail = arr.back();
        tail->next = nullptr;

        current = head;
        // Clearing forward stack because order changed
        forward.clear();
        cout << "Playlist shuffled.\n";
    }

    ~Playlist() {
        // Playlist owns Song* memory: delete all songs first
        Song* tmp = head;
        while (tmp) {
            Song* next = tmp->next;
            delete tmp;
            tmp = next;
        }
        head = tail = current = nullptr;
        totalSongs = 0;

        // Clear AVL nodes (they do not delete Song*)
        songDB.clear();

        // Clear stacks
        history.clear();
        forward.clear();
    }
};

#endif
