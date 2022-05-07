// Train Ticket System
// Copyright (C) 2022 Lau Yee-Yu
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

#ifndef TICKET_SYSTEM_INCLUDE_BP_TREE_H
#define TICKET_SYSTEM_INCLUDE_BP_TREE_H

#include <iostream>
#include <functional>

template<class Key,
         class Value,
         class KeyCompare   = std::less<Key>,
         class ValueCompare = std::less<Value>,
         class KeyEqual     = std::equal_to<Key>,
         class ValueEqual   = std::equal_to<Value>>
class BPTree {
public:
    using Ptr = long;

    struct PairData {
        Key key;
        Value value;
        long timeStamp;
    };

    /**
     * Binding the tree with a certain file.  If the file is empty, a head
     * node for a new class is stored, and the tree is constructed.  If not,
     * the metadata of the old tree is read.
     */
    BPTree(const char* fileName);

    BPTree(const std::string& fileName);

    BPTree(const BPTree&) = delete;

    BPTree& operator=(const BPTree&) = delete;

    ~BPTree();

    /**
     * Insert a key value pair into this tree.
     * @param key
     * @param value
     * @return a bool to tell whether this operation is success or not
     */
    bool Insert(const Key& key, const Value& value, long timeStamp);

    /**
     * Move the pair with the input key to deleted zone.
     * @param key
     */
    void Erase(const Key& key);

    /**
     * Roll back to the prevoius time stamp.  If the tree have been cleared
     * after that time stamp, the bahaviour is undefined.
     *
     */
    void RollBack(long timeStamp);

    /**
     * Clear all the data in this tree. All the space is ready to be reused.
     */
    void Clear();

    /**
     * Tell whether there is a node with the input key.
     */
    bool Contains(const Key& key);

    /**
     * Find the first node with the key.
     @return the first node with the input key
     */
    Value FindFirst(const Key& key);

    /**
     * Get all the value with the input key.
     @return string with the input key
     */
    std::basic_string<Value> FindAll(const Key& key);

private:
    constexpr static long kBlockSize_  = 4096;
    constexpr static long kTargetSize_ = (kBlockSize_ - 2 * sizeof(Ptr)) / (sizeof(Ptr) + sizeof(PairData)) / 2 - 1;
    struct Head_ {
        long nodeSize; // the proper size making sure the block size is 4KiB
        Ptr head = -1;
        Ptr garbage = -1; // a single linked list to store the erased pair(s)

        bool cached = false;
        PairData cachedData; // cached data

        // maybe some other cached stuff
    };

    struct Block_ {
        PairData data[2 * kTargetSize_];
        Ptr      pointers[2 * kTargetSize_]; // For leaf nodes, use it as linked pointer to other leafs
        long     count = 0;
        bool     leaf  = false;
    };

    static_assert(kTargetSize_ >= 2 && sizeof(Block_) <= kBlockSize_);

    template<class Type>
    Type Read_(Ptr position);

    /**
     * Read the block from the file.
     * @param position
     * @return the block at the position
     */
    Block_ ReadBlock_(Ptr position);

    /**
     * Read the head meta from the class.
     * @param position
     * @return the ptr data
     */
    Head_ ReadHead_(Ptr position);

    /**
     * Allocate a space of 4KiB at the end of the file.
     */
    Ptr New_(long size);

    /**
     * Store the value at the given position.
     */
    template<class Type>
    void Write_(Ptr position, const Type& value);

    Head_ head_;
};

#endif //TICKET_SYSTEM_INCLUDE_BP_TREE_H
