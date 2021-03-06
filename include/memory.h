// Train Ticket System
// Copyright (C) 2022 Lau Yee-Yu & relyt871
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#ifndef TICKET_SYSTEM_INCLUDE_MEMORY_H
#define TICKET_SYSTEM_INCLUDE_MEMORY_H

#include <fstream>
#include <iostream>
#include <cstring>

#include "rollback_manager.h"
#include "linked_hash_map.h"

#ifdef ROLLBACK

template<int kBlockSize>
class MemoryManager {
    using Ptr = long;

private:
    static constexpr int kLimit = std::max(409600 / kBlockSize, 1);

    std::fstream file;
    RollBackManager<kBlockSize> rbManager;

    char meta[kBlockSize];

    struct MemNode {
        MemNode* pre;
        MemNode* nxt;
        Ptr pos;
        char info[kBlockSize];
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

    void InitMeta(bool &isNew) {
        file.seekp(0, std::ios::end);
        if (file.tellp() == 0) {
            memset(meta, 0, sizeof(meta));
            file.write(meta, kBlockSize);
            isNew = true;
        } else {
            file.seekg(0);
            file.read(meta, kBlockSize);
            isNew = false;
        }
    }

public:
    MemoryManager(const char* filename, const char* filename_log, bool &isNew) : 
        file(filename, std::ios::in | std::ios::out | std::ios::binary),
        rbManager(filename_log) {
        head = rear = nullptr;
        trash_head = nullptr;
        InitMeta(isNew);
    }
    ~MemoryManager() {
        ClearMemory();
        file.close();
    }

    char* GetMeta() {
        return meta;
    }

    void UpdateMeta(long timeStamp) {
        rbManager.Insert(meta, 0, timeStamp);
    }

    void ClearMemory() {
        file.seekp(0);
        file.write((char*)&meta, kBlockSize);
        mp.Clear();
        for (MemNode* p = head; p != nullptr;) {
            MemNode* q = p -> nxt;
            file.seekp(p -> pos);
            file.write(p -> info, kBlockSize);
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

    void Clear() {
        mp.Clear();
        for (MemNode* p = head; p != nullptr;) {
            MemNode* q = p -> nxt;
            delete p;
            p = q;
        }
        head = rear = nullptr;
    }

    MemNode* findMemory() {
        MemNode* cur = nullptr;
        if (mp.Size() >= kLimit) {
            file.seekp(rear -> pos);
            file.write(rear -> info, kBlockSize);
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
            file.write(cur -> info, kBlockSize);
        }
        mp[cur -> pos] = cur;
        Last = cur -> pos;
        return cur -> info;
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

    // timeStamp >= 0 means the node will be modified
    char* ReadNode(Ptr pos, long timeStamp) {
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
            file.read(cur -> info, kBlockSize);
        }
        if (timeStamp >= 0) {
            rbManager.Insert(cur -> info, pos, timeStamp);
        }
        return cur -> info;
    }

    void RollBack(long timeStamp) {
        ClearMemory();
        rbManager.RollBack(file, timeStamp);
        file.seekg(0);
        file.read(meta, kBlockSize);
    }
};

#else

template<int kBlockSize>
class MemoryManager {
    using Ptr = long;

private:
    static constexpr int kLimit = std::max(409600 / kBlockSize, 1);

    std::fstream file;

    char meta[kBlockSize];

    struct MemNode {
        MemNode* pre;
        MemNode* nxt;
        Ptr pos;
        char bpInfo[kBlockSize];
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
    Ptr old_trash;

    void InitMeta(bool &isNew) {
        file.seekp(0, std::ios::end);
        if (file.tellp() == 0) {
            memset(meta, 0, sizeof(meta));
            file.write(meta, kBlockSize);
            isNew = true;
            old_trash = -1;
        } else {
            file.seekg(0);
            file.read(meta, kBlockSize);
            isNew = false;
            old_trash = *(reinterpret_cast<Ptr*>(meta + 16));
        }
    }

public:
    MemoryManager(const char* filename, bool &isNew) : 
        file(filename, std::ios::in | std::ios::out | std::ios::binary) {
        head = rear = nullptr;
        trash_head = nullptr;
        InitMeta(isNew);
    }
    ~MemoryManager() {
        ClearMemory();
        file.close();
    }

    char* GetMeta() {
        return meta;
    }

    void ClearMemory() {
        mp.Clear();
        for (MemNode* p = head; p != nullptr;) {
            MemNode* q = p -> nxt;
            file.seekp(p -> pos);
            file.write(p -> bpInfo, kBlockSize);
            delete p;
            p = q;
        }
        head = rear = nullptr;
        if (trash_head == nullptr) {
            *(reinterpret_cast<Ptr*>(meta + 16)) = old_trash;
        } else {
            *(reinterpret_cast<Ptr*>(meta + 16)) = trash_head -> pos;
            for (Trash* p = trash_head; p != nullptr; ) {
                Trash* q = p -> nxt;
                file.seekp(p -> pos);
                if (q == nullptr) {
                    file.write((char*)&old_trash, sizeof(Ptr));
                } else {
                    file.write((char*)&(q -> pos), sizeof(Ptr));
                }
                delete p;
                p = q;
            }
            trash_head = nullptr;
        }
        file.seekp(0);
        file.write((char*)&meta, kBlockSize);
    }

    void Clear() {
        mp.Clear();
        for (MemNode* p = head; p != nullptr;) {
            MemNode* q = p -> nxt;
            delete p;
            p = q;
        }
        head = rear = nullptr;
    }

    MemNode* findMemory() {
        MemNode* cur = nullptr;
        if (mp.Size() >= kLimit) {
            file.seekp(rear -> pos);
            file.write(rear -> bpInfo, kBlockSize);
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
        } else if (old_trash != -1) {
            cur -> pos = old_trash;
            file.seekg(old_trash);
            file.read((char*)&old_trash, sizeof(Ptr));
        } else {
            file.seekp(0, std::ios::end);
            cur -> pos = file.tellp();
            file.write(cur -> bpInfo, kBlockSize);
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
            file.read(cur -> bpInfo, kBlockSize);
        }
        return cur -> bpInfo;
    }
};

#endif

#endif // TICKET_SYSTEM_INCLUDE_MEMORY_H
