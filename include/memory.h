#ifndef MEMORY
#define MEMORY

#include <fstream>
#include "bptree.h"
#include "linked_hash_map.h"

class MemoryManager {
    const int LIM = 200;
    std::fstream file;

    using Ptr = long;

private:
    struct MemNode {
        MemNode* pre;
        MemNode* nxt;
        Ptr pos;
        char bpInfo[4096];
        MemNode(Ptr _pos = -1): pos(_pos) {
            pre = nxt = nullptr;
        }
    } *head, *rear;
    LinkedHashMap<Ptr, MemNode*> mp;

    struct Trash {
        Ptr pos;
        Trash* nxt;
        Trash(Ptr _pos = -1, Trash* _nxt = nullptr): pos(_pos), nxt(_nxt) {}
    } *trash_head;

public:
    MemoryManager(const char* filename) : file(filename, std::ios::in | std::ios::out | std::ios::binary) {
        head = rear = nullptr;
        trash_head = nullptr;
    }
    ~MemoryManager() {
        ClearMemory();
        file.close();
    }

    void ClearMemory() {
        mp.Clear();
        for (MemNode* p = head; p != nullptr;) {
            MemNode* q = p -> nxt;
            file.seekp(p -> pos);
            file.write(p -> bpInfo, 4096);
            delete p;
            p = q;
        }
        head = rear = nullptr;
        for (Trash* p = trash_head; p != nullptr; ) {
            Trash* q = p -> nxt;
            delete p;
            p = q;
        }
        trash_head = nullptr;
    }

    MemNode* findMemory() {
        MemNode* cur = nullptr;
        if (mp.Size() >= LIM) {
            file.seekp(rear -> pos);
            file.write(rear -> bpInfo, 4096);
            mp.Erase(mp.Find(rear -> pos));
            cur = rear;
            rear = rear -> pre;
            rear -> nxt = nullptr;
        } else {
            cur = new MemNode();
            if (head == nullptr) {
                head = rear = cur;
            }
        }
        cur -> pre = nullptr;
        if (head != cur) {
            cur -> nxt = head;
            head -> pre = cur;
            head = cur;
        }
        return cur;
    }


    Ptr Last;
    char* AddNode() {
        MemNode* cur = findMemory();
        if (trash_head != nullptr) {
            cur -> pos = trash_head -> pos;
            Trash *tmp = trash_head;
            trash_head = trash_head -> nxt;
            delete tmp;
        } else {
            file.seekp(0, std::ios::end);
            cur -> pos = file.tellp();
            file.write(cur -> bpInfo, 4096);
        }
        mp[cur -> pos] = cur;
        Last = cur -> pos;
        return cur -> bpInfo;
    }

    void DelNode(Ptr pos) {
        auto it = mp.Find(pos);
        if (it != mp.end()) {
            MemNode* cur = it -> second;
            if (head == cur) {
                head = cur -> nxt;
            } else {
                cur -> pre -> nxt = cur -> nxt;
            }
            if (rear == cur) {
                rear = rear -> pre;
            } else {
                cur -> nxt -> pre = cur -> pre;
            }
            mp.Erase(it);
            delete cur;
        }
        trash_head = new Trash(pos, trash_head);
    }

    char* ReadNode(Ptr pos) {
        MemNode* cur = mp[pos];
        if (cur != nullptr) {
            if (head != cur) {
                cur -> pre -> nxt = cur -> nxt;
                if (rear != cur) {
                    cur -> nxt -> pre = cur -> pre;
                } else {
                    rear = rear -> pre;
                }
                cur -> nxt = head;
                head -> pre = cur;
                head = cur;
            }
        } else {
            cur = findMemory();
            cur -> pos = pos;
            mp[pos] = cur;
            file.seekg(pos);
            file.read(cur -> bpInfo, 4096);
        }
        return cur -> bpInfo;
    }
};

#endif