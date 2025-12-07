#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include "Song.h"
#include "AVL.h"
#include "SongStack.h"
#include "SongQueue.h"
using namespace std;

class Playlist {
private:
    Song* head;
    Song* tail;
    Song* current;
    string name;
    int totalSongs;

    AVL songDB;       // AVL tree keyed by (title, id) internally
    SongStack history; // Recently played stack
    SongQueue nextUp;  // Queue of next songs

public:
    Playlist(string n = "My Playlist") {
        head = tail = current = nullptr;
        name = n;
        totalSongs = 0;
    }

    // Add song - rejects if title+artist already exist (checks BEFORE creating Song)
    void addSong(string title, string artist) {
        // Search for existing songs with this title
        vector<Song*> matches = songDB.searchAll(title);
        for (Song* m : matches) {
            if (m->artist == artist && m-> ==title) {
                cout << "Cannot add: a song with title \"" << title << "\" and artist \"" << artist << "\" already exists.\n";
                return; // no Song constructed, id not consumed
            }
        }

        // No exact duplicate found -> create Song and insert
        Song* s = new Song(title, artist);
        songDB.insert(s);

        // Linked list playlist
        if (!head) { head = tail = current = s; }
        else { tail->next = s; s->prev = tail; tail = s; }

        totalSongs++;
        cout << "Added: (" << s->id << ") " << title << " by " << artist << endl;
    }

    // Display playlist (linked-list order)
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

    // Play current song 
    void playCurrent() {
        if (!current) current = head;
        if (!current) { cout << "No songs to play.\n"; return; }

        cout << "Now Playing: (" << current->id << ") " << current->title << " - " << current->artist << endl;
    }

    // Next song
    void nextSong() {
        if (nextUp.isEmpty()) {
            if (!current) current = head;
            if (current && current->next) {
                history.push(current);
                current = current->next;
            }
            else { cout << "End of playlist.\n"; return; }
        }
        else {
            if (current) history.push(current);
            current = nextUp.dequeue();
            if (!current) { cout << "End of playlist.\n"; return; }
        }
        playCurrent();
    }

    // Previous song
    void previousSong() {
        if (history.isEmpty()) { cout << "No previous song.\n"; return; }
        Song* prev = history.pop();
        if (!prev) { cout << "No previous song.\n"; return; }
        current = prev;
        playCurrent();
    }

    // Enqueue next song
    void enqueueNextSong(Song* s) { nextUp.enqueue(s); }

    // Search songs by title (returns all matches)
    void searchSong(const string& title) {
        vector<Song*> matches = songDB.searchAll(title);
        if (matches.empty()) {
            cout << "Song not found: " << title << endl;
            return;
        }
        cout << "Matches for \"" << title << "\":\n";
        for (Song* s : matches) {
            cout << "(" << s->id << ") " << s->title << " - " << s->artist << endl;
        }
    }

    // Display all songs from AVL (ordered by title,id)
    void displayAllSongs() { songDB.displayAllSongs(); }

    // Delete a song by id (id is unique)
    void deleteSongById(int id) {
        // Find in linked list
        Song* temp = head;
        while (temp) {
            if (temp->id == id) break;
            temp = temp->next;
        }
        if (!temp) { cout << "Song with id " << id << " not found in playlist.\n"; return; }

        // Remove from AVL (AVL won't delete Song* memory)
        bool removed = songDB.removeById(id);
        if (!removed) {
            // tree inconsistent, but proceed to remove from list
        }

        // Remove from linked list
        if (temp->prev) temp->prev->next = temp->next;
        else head = temp->next;

        if (temp->next) temp->next->prev = temp->prev;
        else tail = temp->prev;

        if (current == temp) current = temp->next ? temp->next : head;

        totalSongs--;
        cout << "Deleted: (" << temp->id << ") " << temp->title << " - " << temp->artist << endl;
        delete temp;
    }

    // Reverse playlist
    void reversePlaylist() {
        Song* temp = nullptr;
        Song* curr = head;
        while (curr) {
            temp = curr->prev;
            curr->prev = curr->next;
            curr->next = temp;
            curr = curr->prev;
        }
        if (temp) head = temp->prev;
        cout << "Playlist reversed.\n";
    }

    // Sort playlist by title (Bubble Sort) 
    void sortPlaylist() {
        if (!head || !head->next) return;
        bool swapped;
        do {
            swapped = false;
            Song* temp = head;
            while (temp->next) {
                if (temp->title > temp->next->title) {
                    swap(temp->title, temp->next->title);
                    swap(temp->artist, temp->next->artist);
                    swap(temp->id, temp->next->id);
                    swapped = true;
                }
                temp = temp->next;
            }
        } while (swapped);

        cout << "Playlist sorted by title.\n";
    }

    // Shuffle playlist
    void shufflePlaylist() {
        if (!head || totalSongs < 2) return;

        Song** arr = new Song * [totalSongs];
        Song* temp = head;
        for (int i = 0; i < totalSongs; i++) {
            arr[i] = temp;
            temp = temp->next;
        }

        // Simple shuffle
        for (int i = totalSongs - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(arr[i], arr[j]);
        }

        // Rebuild linked list
        head = arr[0];
        head->prev = nullptr;
        for (int i = 0; i < totalSongs - 1; i++) {
            arr[i]->next = arr[i + 1];
            arr[i + 1]->prev = arr[i];
        }
        tail = arr[totalSongs - 1];
        tail->next = nullptr;

        delete[] arr;
        cout << "Playlist shuffled.\n";
    }
};

#endif