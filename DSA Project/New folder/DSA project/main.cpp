#include <iostream>
#include <string>
#include "Playlist.h"
using namespace std;

bool isAlphabetic(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isalpha(c) && c != ' ') return false;
    return true;
}

bool isInteger(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(c)) return false;
    return true;
}

int main() {
    Playlist myPlaylist("My Mix");

    string input, title, artist, idStr;
    int id;

    while (true) {
        cout << "\n--- MENU ---\n"
             << "1. Add Song\n"
             << "2. Display Playlist\n"
             << "3. Play Song\n"
             << "4. Next Song\n"
             << "5. Previous Song\n"
             << "6. Search by Title\n"
             << "7. Delete by Id\n"
             << "8. Reverse Playlist\n"
             << "9. Sort Playlist\n"
             << "10. Shuffle Playlist\n"
             << "0. Exit\n"
             << "Choice: ";
        cin >> input;

        if (input == "0") break;

        if (input == "1") {
            cin.ignore();
            cout << "Title: "; getline(cin, title);
            cout << "Artist: "; getline(cin, artist);
            myPlaylist.addSong(title, artist);
        }
        else if (input == "2") myPlaylist.displayPlaylist();
        else if (input == "3") myPlaylist.playCurrent();
        else if (input == "4") myPlaylist.nextSong();
        else if (input == "5") myPlaylist.previousSong();
        else if (input == "6") {
            cin.ignore();
            cout << "Search title: "; getline(cin, title);
            myPlaylist.searchSong(title);
        }
        else if (input == "7") {
            cin.ignore();
            cout << "Enter id to delete: ";
            getline(cin, idStr);
            if (!isInteger(idStr)) {
                cout << "Invalid id. Please enter a positive integer.\n";
            } else {
                id = stoi(idStr);
                myPlaylist.deleteSongById(id);
            }
        }
        else if (input == "8") myPlaylist.reversePlaylist();
        else if (input == "9") myPlaylist.sortPlaylist();
        else if (input == "10") myPlaylist.shufflePlaylist();
        else cout << "Invalid choice.\n";
    }

    cout << "Exiting program.\n";
    return 0;
}