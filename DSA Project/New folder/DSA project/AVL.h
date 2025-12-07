#ifndef AVL_H
#define AVL_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "Song.h"
using namespace std;

class AVLNode {
public:
    Song* song;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Song* s) : song(s), left(nullptr), right(nullptr), height(1) {}
};

class AVL {
private:
    AVLNode* root;

    int getHeight(AVLNode* node) {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    // Compare by title first, then by id as tiebreaker (id is unique)
    // returns -1 if a < b, 0 if equal (same title & same id), +1 if a > b
    int compareByTitleId(Song* a, Song* b) {
        if (a->title < b->title) return -1;
        if (a->title > b->title) return 1;
        if (a->id < b->id) return -1;
        if (a->id > b->id) return 1;
        return 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    // insertNode: inserts s into subtree rooted at node.
    // If an existing node with same title AND same artist is found, insertion is rejected.
    AVLNode* insertNode(AVLNode* node, Song* s, bool& inserted) {
        if (!node) {
            inserted = true;
            return new AVLNode(s);
        }

        // If exact duplicate (title + artist) found at this node, reject insertion
        if (s->title == node->song->title && s->artist == node->song->artist) {
            inserted = false;
            return node;
        }

        int cmp = compareByTitleId(s, node->song);
        if (cmp < 0) {
            node->left = insertNode(node->left, s, inserted);
        }
        else {
            node->right = insertNode(node->right, s, inserted);
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        // Balance cases
        if (balance > 1 && compareByTitleId(s, node->left->song) < 0) return rightRotate(node);
        if (balance < -1 && compareByTitleId(s, node->right->song) > 0) return leftRotate(node);
        if (balance > 1 && compareByTitleId(s, node->left->song) > 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && compareByTitleId(s, node->right->song) < 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current && current->left) current = current->left;
        return current;
    }

    // delete by (title, id) key
    AVLNode* deleteNodeByKey(AVLNode* node, const string& title, int id, bool& deleted) {
        if (!node) return nullptr;

        if (title < node->song->title || (title == node->song->title && id < node->song->id)) {
            node->left = deleteNodeByKey(node->left, title, id, deleted);
        }
        else if (title > node->song->title || (title == node->song->title && id > node->song->id)) {
            node->right = deleteNodeByKey(node->right, title, id, deleted);
        }
        else {
            // found
            deleted = true;
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (!temp) {
                    delete node;
                    node = nullptr;
                }
                else {
                    AVLNode* toDelete = node;
                    node = temp;
                    delete toDelete;
                }
            }
            else {
                AVLNode* temp = minValueNode(node->right);
                node->song = temp->song;
                node->right = deleteNodeByKey(node->right, temp->song->title, temp->song->id, deleted);
            }
        }

        if (!node) return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0) return rightRotate(node);
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && getBalance(node->right) <= 0) return leftRotate(node);
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void inOrderTraversal(AVLNode* node) {
        if (!node) return;
        inOrderTraversal(node->left);
        cout << "(" << node->song->id << ") " << node->song->title << " - " << node->song->artist << endl;
        inOrderTraversal(node->right);
    }

    // collect all songs with matching title
    void collectByTitle(AVLNode* node, const string& target, vector<Song*>& out) {
        if (!node) return;
        if (target < node->song->title) {
            collectByTitle(node->left, target, out);
        }
        else if (target > node->song->title) {
            collectByTitle(node->right, target, out);
        }
        else {
            // equal: collect this node, and both subtrees (there may be multiple entries with same title)
            collectByTitle(node->left, target, out);
            out.push_back(node->song);
            collectByTitle(node->right, target, out);
        }
    }

    // find title by id (used to delete by id)
    string findTitleById(AVLNode* node, int id) {
        if (!node) return string();
        if (node->song->id == id) return node->song->title;
        string left = findTitleById(node->left, id);
        if (!left.empty()) return left;
        return findTitleById(node->right, id);
    }

    // search for any single node with matching title (BST-style)
    AVLNode* searchNodeByTitle(AVLNode* node, const string& title) {
        if (!node) return nullptr;
        if (node->song->title == title) return node;
        if (title < node->song->title) return searchNodeByTitle(node->left, title);
        return searchNodeByTitle(node->right, title);
    }

public:
    AVL() : root(nullptr) {}

    // Insert: returns true if inserted, false if exact (title+artist) duplicate found
    bool insert(Song* s) {
        bool inserted = false;
        root = insertNode(root, s, inserted);
        return inserted;
    }

    // Remove by id
    bool removeById(int id) {
        string title = findTitleById(root, id);
        if (title.empty()) return false;
        bool deleted = false;
        root = deleteNodeByKey(root, title, id, deleted);
        return deleted;
    }

    // Return all songs that match a title
    vector<Song*> searchAll(const string& title) {
        vector<Song*> result;
        collectByTitle(root, title, result);
        return result;
    }

    // Return one match (compat)
    Song* search(const string& title) {
        AVLNode* node = searchNodeByTitle(root, title);
        return node ? node->song : nullptr;
    }

    void displayAllSongs() {
        cout << "--- All Songs in AVL (by title,id) ---\n";
        inOrderTraversal(root);
    }
};

#endif