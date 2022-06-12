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

#ifndef TICKET_SYSTEM_INCLUDE_LINKED_HASH_MAP_H
#define TICKET_SYSTEM_INCLUDE_LINKED_HASH_MAP_H

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.h"
#include "exceptions.h"


using SizeT = long;

class RehashPolicy {
public:
    RehashPolicy() noexcept = default;

    explicit RehashPolicy(SizeT minimumSize) {
        ReserveAtLeast(minimumSize);
    }

    RehashPolicy(const RehashPolicy&) noexcept = default;

    RehashPolicy& operator=(const RehashPolicy&) noexcept = default;

    ~RehashPolicy() = default;

    [[nodiscard]] SizeT NextSize() noexcept {
        if (index_ == maxIndex_) {
            return BucketSize_[index_];
        }
        ++index_;
        return BucketSize_[index_];
    }

    [[nodiscard]] SizeT PreviousSize() noexcept {
        if (index_ == 0) {
            return BucketSize_[index_];
        }
        --index_;
        return BucketSize_[index_];
    }

    [[nodiscard]] constexpr static SizeT MaxSize() noexcept {
        return BucketSize_[maxIndex_];
    }

    SizeT ReserveAtLeast(SizeT size) {
        if (size == 0) {
            index_ = 0;
            return BucketSize_[index_];
        }
        SizeT left = 0, right = maxIndex_;
        while (left < right) {
            SizeT mid = (left + right) / 2;
            if (BucketSize_[mid] >= size) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        index_ = left;
        return BucketSize_[index_];
    }

    void SetSize(SizeT size) {
        if (size == 0) {
            index_ = 0;
            return;
        }
        SizeT left = 0, right = maxIndex_;
        while (left < right) {
            SizeT mid = (left + right) / 2;
            if (BucketSize_[mid] >= size) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        index_ = left;
    }

private:
    constexpr static SizeT BucketSize_[32] = {
        0,
        127,
        257,
        521,
        1031,
        2053,
        4099,
        8209,
        16411,
        32771,
        65537,
        131101,
        262147,
        524309,
        1048583,
        2097169,
        4194319,
        8388617,
        19260817, // in memory of him
        33554467,
        67108879,
        134217757,
        268435459,
        536870923,
        1073741827,
        2147483659,
        4294967291,
        8589934583,
        17179869143,
        34359738337,
        68719476731,
        137438953481
    };

    constexpr static SizeT maxIndex_ = 31;

    SizeT index_ = 0;
};

template <class T, class Hash = std::hash<T>, class Equal = std::equal_to<T>>
class LinkedHashTable {
public:
    class Iterator;
    class ConstIterator;

    struct Node {
        friend class LinkedHashTable;
    public:
        Node() = default;
        Node(Node&) = default;
        Node(const Node&) = default;
        Node(Node&&) = default;

        Node(std::size_t hashIn, const T& valueIn) : value(valueIn), hash(hashIn) {}
        Node(std::size_t hashIn, T&& valueIn) : value(valueIn), hash(hashIn) {}
        explicit Node(const T& valueIn) : value(valueIn), hash(Hash()(value)) {}
        explicit Node(T&& valueIn) : value(valueIn), hash(Hash()(value)) {}

        // Every time you change the value, you should also change the hash
        T value;
        std::size_t hash = 0;

    private:
        Node* next = nullptr;
        Node* linkedNext = nullptr;
        Node* linkedPrevious = nullptr;
    };

    class Iterator {
        friend class LinkedHashTable;
        friend class ConstIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        Iterator() = default;
        Iterator(const Iterator&) = default;

        Iterator& operator=(const Iterator&) = default;

        ~Iterator() = default;

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator++() {
            if (target_ == nullptr) throw InvalidIterator();
            target_ = target_->linkedNext;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        Iterator& operator--() {
            if (target_ == nullptr) {
                target_ = table_->tail_;
                if (target_ == nullptr) throw InvalidIterator();
                return *this;
            }
            if (target_->linkedPrevious == nullptr) throw InvalidIterator();
            target_ = target_->linkedPrevious;
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const Iterator& rhs) const {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator==(const ConstIterator& rhs) const {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }
        bool operator!=(const Iterator& rhs) const {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
        }
        bool operator!=(const ConstIterator& rhs) const {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
        }

        Node& operator*()  const {
            if (target_ == nullptr) throw InvalidIterator();
            return *target_;
        }
        Node* operator->() const {
            if (target_ == nullptr) throw InvalidIterator();
            return target_;
        }

    private:
        Iterator(Node* target, const LinkedHashTable* table) : target_(target), table_(table) {}

        Node* target_ = nullptr;
        const LinkedHashTable* table_ = nullptr;
    };

    class ConstIterator {
        friend class LinkedHashTable;
        friend class Iterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() = default;
        ConstIterator(const ConstIterator&) = default;
        explicit ConstIterator(const Iterator& obj) : target_(obj.target_), table_(obj.table_) {}

        ConstIterator& operator=(const ConstIterator&) = default;

        ~ConstIterator() = default;

        ConstIterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstIterator& operator++() {
            if (target_ == nullptr) throw InvalidIterator();
            target_ = target_->linkedNext;
            return *this;
        }

        ConstIterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        ConstIterator& operator--() {
            if (target_ == nullptr) {
                target_ = table_->tail_;
                if (target_ == nullptr) throw InvalidIterator();
                return *this;
            }
            if (target_->linkedPrevious == nullptr) throw InvalidIterator();
            target_ = target_->linkedPrevious;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator==(const ConstIterator& rhs) const {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator!=(const Iterator& rhs) const {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
        }

        bool operator!=(const ConstIterator& rhs) const {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
        }

        const Node& operator*()  const {
            if (target_ == nullptr) throw InvalidIterator();
            return *target_;
        }

        const Node* operator->() const {
            if (target_ == nullptr) throw InvalidIterator();
            return target_;
        }

    private:
        ConstIterator(const Node* target, const LinkedHashTable* table) : target_(target), table_(table) {}

        const Node* target_ = nullptr;
        const LinkedHashTable* table_ = nullptr;
    };
   
    LinkedHashTable() : hash_(), equal_(), rehashPolicy_(),
                        head_(nullptr), tail_(nullptr), bucket_(nullptr),
                        size_(0), bucketSize_(0) {}
   
    LinkedHashTable(const LinkedHashTable& obj) : hash_(obj.hash_),
                                                  equal_(obj.equal_),
                                                  rehashPolicy_(obj.rehashPolicy_),
                                                  head_(nullptr),
                                                  tail_(nullptr),
                                                  bucket_(nullptr),
                                                  size_(obj.size_),
                                                  bucketSize_(obj.bucketSize_) {
        bucket_ = new Node*[bucketSize_];
        for (int i = 0; i < bucketSize_; ++i) {
            bucket_[i] = nullptr;
        }

        for (Node* node = obj.head_; node != nullptr; node = node->linkedNext) {
            Node* newNode;
            try {
                newNode = new Node(node->hash, node->value);
            } catch (...) {
                Node* toDelete = head_;
                while (toDelete != nullptr) {
                    Node* next = toDelete->linkedNext;
                    delete toDelete;
                    toDelete = next;
                }
                delete[] bucket_;
                throw;
            }
            this->Insert_(newNode);
        }
    }

    LinkedHashTable(LinkedHashTable&& obj) noexcept
        : hash_(obj.hash_),
          equal_(obj.equal_),
          rehashPolicy_(obj.rehashPolicy_),
          head_(obj.head_),
          tail_(obj.tail_),
          bucket_(obj.bucket_),
          size_(obj.size_),
          bucketSize_(obj.bucketSize_) {
        obj.head_ = nullptr;
        obj.tail_ = nullptr;
        obj.bucket_ = nullptr;
        obj.size_ = 0;
        obj.bucketSize_ = 0;
    }

    LinkedHashTable& operator=(const LinkedHashTable& obj) {
        if (this == &obj) {
            return *this;
        }

        Node** newBucket = new Node*[obj.bucketSize_];
        Node* newHead = nullptr;
        Node* newTail = nullptr;

        for (int i = 0; i < obj.bucketSize_; ++i) {
            newBucket[i] = nullptr;
        }

        for (Node* node = obj.head_; node != nullptr; node = node->linkedNext) {
            Node* newNode;
            try {
                newNode = new Node(node->hash, node->value);
            } catch (...) {
                Node* toDelete = newHead;
                while (toDelete != nullptr) {
                    toDelete = toDelete->linkedNext;
                    Node* next = toDelete->linkedNext;
                    delete toDelete;
                    toDelete = next;
                }
                delete[] newBucket;
                throw;
            }
            // Handling the linked list
            newNode->linkedPrevious = newTail;
            if (newHead == nullptr) newHead = newNode;
            else newTail->linkedNext = newNode;
            newTail = newNode;

            // Handling the bucket
            std::size_t index = (newNode->hash) % obj.bucketSize_;
            newNode->next = newBucket[index];
            newBucket[index] = newNode;
        }

        this->Clear();
        this->hash_ = obj.hash_;
        this->equal_ = obj.equal_;
        this->rehashPolicy_ = obj.rehashPolicy_;
        this->size_ = obj.size_;
        this->bucketSize_ = obj.bucketSize_;
        this->bucket_ = newBucket;
        this->head_ = newHead;
        this->tail_ = newTail;

        return *this;
    }

    LinkedHashTable& operator=(LinkedHashTable&& obj) noexcept {
        this->Clear();
        hash_ = obj.hash_;
        equal_ = obj.equal_;
        rehashPolicy_ = obj.rehashPolicy_;
        head_ = obj.head_;
        tail_ = obj.tail_;
        bucket_ = obj.bucket_;
        size_ = obj.size_;
        bucketSize_ = obj.bucketSize_;
        obj.head_ = nullptr;
        obj.tail_ = nullptr;
        obj.bucket_ = nullptr;
        obj.size_ = 0;
        obj.bucketSize_ = 0;
        return *this;
    }
   
    ~LinkedHashTable() { this->Clear(); }

    /**
     * Delete all the contents of the hash table.
     * @return
     */
    LinkedHashTable& Clear() {
        Node* toDelete = head_;
        while (toDelete != nullptr) {
            Node* next = toDelete->linkedNext;
            delete toDelete;
            toDelete = next;
        }
        delete[] bucket_;
        bucket_ = nullptr;
        bucketSize_ = 0;
        rehashPolicy_.SetSize(0);
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
        return *this;
    }

    Pair<Iterator, bool> Insert(const T& value) {
        if (NeedRehash_()) Rehash_();
        std::size_t hash = hash_(value);
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
            }
            tmpNode = tmpNode->next;
        }
        Node* newNode = new Node(hash, value);
        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    LinkedHashTable& Erase(Iterator position) {
        if (position.table_ != this) throw InvalidIterator();
        if (position.target_ == nullptr) throw InvalidIterator();
        Erase_(position.target_);
        return *this;
    }

    LinkedHashTable& Erase(const T& value) {
        std::size_t bucketIndex = hash_(value) % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];

        if (equal_(tmpNode->value, value)) {
            bucket_[bucketIndex] = tmpNode->next;
            if (tmpNode == head_) head_ = tmpNode->linkedNext;
            if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
            if (tmpNode->linkedNext != nullptr) {
                tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
            }
            if (tmpNode->linkedPrevious != nullptr) {
                tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
            }
            delete tmpNode;
            --size_;
            return *this;
        }

        Node* previousNode = tmpNode;
        tmpNode = tmpNode->next;
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                if (tmpNode == head_) head_ = tmpNode->linkedNext;
                if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
                if (tmpNode->linkedNext != nullptr) {
                    tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
                }
                if (tmpNode->linkedPrevious != nullptr) {
                    tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
                }
                previousNode->next = tmpNode->next;
                delete tmpNode;
                --size_;
                return *this;
            }
            previousNode = tmpNode;
            tmpNode = tmpNode->next;
        }
        return *this;
    }

    template<class K>
    LinkedHashTable& Erase(const K& value) {
        std::size_t bucketIndex = hash_(value) % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];

        if (equal_(tmpNode->value, value)) {
            bucket_[bucketIndex] = tmpNode->next;
            if (tmpNode == head_) head_ = tmpNode->linkedNext;
            if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
            if (tmpNode->linkedNext != nullptr) {
                tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
            }
            if (tmpNode->linkedPrevious != nullptr) {
                tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
            }
            delete tmpNode;
            --size_;
            return *this;
        }

        Node* previousNode = tmpNode;
        tmpNode = tmpNode->next;
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                if (tmpNode == head_) head_ = tmpNode->linkedNext;
                if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
                if (tmpNode->linkedNext != nullptr) {
                    tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
                }
                if (tmpNode->linkedPrevious != nullptr) {
                    tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
                }
                previousNode->next = tmpNode->next;
                delete tmpNode;
                --size_;
                return *this;
            }
            previousNode = tmpNode;
            tmpNode = tmpNode->next;
        }
        return *this;
    }

    LinkedHashTable& ReserveAtLeast(SizeT minimumSize) {
        if (minimumSize <= bucketSize_ ||
            minimumSize >= rehashPolicy_.NextSize()) {
            return *this;
        }
        SizeT newSize = rehashPolicy_.ReserveAtLeast(minimumSize);
        Node** newBucket;
        try {
            newBucket = new Node*[newSize];
        } catch (...) {
            rehashPolicy_.SetSize(bucketSize_);
            throw;
        }
        delete[] bucket_;
        bucket_ = newBucket;
        bucketSize_ = newSize;
        for (SizeT i = 0; i < newSize; ++i) {
            newBucket[i] = nullptr;
        }
        for (Node* node = head_; node != nullptr; node = node->next) {
            Reinsert_(node);
        }
        return *this;
    }

    [[nodiscard]] Iterator Begin() { return Iterator(head_, this); }
    [[nodiscard]] ConstIterator Begin() const { return ConstIterator(head_, this); }
    [[nodiscard]] ConstIterator ConstBegin() const { return ConstIterator(head_, this); }


    [[nodiscard]] Iterator End() { return Iterator(nullptr, this); }
    [[nodiscard]] ConstIterator End() const { return ConstIterator(nullptr, this); }
    [[nodiscard]] ConstIterator ConstEnd() const { return ConstIterator(nullptr, this); }
    [[nodiscard]] Iterator Find(const T& value) { return Iterator(Find_(value), this); }

    template<class K>
    [[nodiscard]] Iterator Find(const K& value) { return Iterator(Find_(value), this); }

    [[nodiscard]] ConstIterator Find(const T& value) const { return ConstIterator(Find_(value), this); }

    template<class K>
    [[nodiscard]] ConstIterator Find(const K& value) const { return ConstIterator(Find_(value), this); }

    [[nodiscard]] SizeT Count(const T& value) const { return Find_(value) != nullptr ? 1 : 0; }

    template<class K>
    [[nodiscard]] SizeT Count(const K& value) const { return Find_(value) != nullptr ? 1 : 0; }

    [[nodiscard]] bool Contains(const T& value) const { return Find_(value) != nullptr; }

    template<class K>
    [[nodiscard]] bool Contains(const K& value) const { return Find_(value) != nullptr; }

    [[nodiscard]] SizeT Size() const { return size_; }
    [[nodiscard]] bool Empty() const { return size_ == 0; }
   
private:
    /**
     * Check whether the hash table needs to be rehashed.
     * @return
     */
    [[nodiscard]] bool NeedRehash_() const noexcept {
        if (size_ >= RehashPolicy::MaxSize()) return false;
        return (size_ >= bucketSize_);
    }

    /**
     * Rehash the hash table.
     */
    void Rehash_() {
        bucketSize_ = rehashPolicy_.NextSize();
        Node** newBucket;
        try {
            newBucket = new Node*[bucketSize_];
        } catch (...) {
            bucketSize_ = rehashPolicy_.PreviousSize();
            throw;
        }
        delete[] bucket_;
        bucket_ = newBucket;
        for (int i = 0; i < bucketSize_; ++i) bucket_[i] = nullptr;
        for (Node* node = head_; node != nullptr; node = node->linkedNext) Reinsert_(node);
    }

    /**
     * Insert the node into the bucket, the size will not be updated.  Note
     * that the node must be unique.
     * @param node
     */
    void Insert_(Node* node) noexcept {
        node->linkedPrevious = tail_;
        if (head_ == nullptr) head_ = node;
        else tail_->linkedNext = node;
        tail_ = node;
        this->Reinsert_(node);
    }

    /**
     * Reinsert the node into the buckets.  This function only handles the
     * stuff about the hash table, it does not handle the linked list.
     * @param node the node to reinsert
     */
    void Reinsert_(Node* node) noexcept {
        std::size_t index = (node->hash) % bucketSize_;
        node->next = bucket_[index];
        bucket_[index] = node;
    }

    [[nodiscard]] Node* Find_(const T& value) const {
        if (size_ == 0) return nullptr;
        std::size_t bucketIndex = hash_(value) % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                return tmpNode;
            }
            tmpNode = tmpNode->next;
        }
        return nullptr;
    }

    template<class K>
    [[nodiscard]] Node* Find_(const K& key) const {
        if (size_ == 0) return nullptr;
        std::size_t bucketIndex = hash_(key) % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, key)) {
                return tmpNode;
            }
            tmpNode = tmpNode->next;
        }
        return nullptr;
    }

    void Erase_(Node* node) {
        if (node == head_) head_ = node->linkedNext;
        if (node == tail_) tail_ = node->linkedPrevious;
        if (node->linkedNext != nullptr) {
            node->linkedNext->linkedPrevious = node->linkedPrevious;
        }
        if (node->linkedPrevious != nullptr) {
            node->linkedPrevious->linkedNext = node->linkedNext;
        }
        std::size_t hash = node->hash % bucketSize_;
        Node* tmpNode = bucket_[hash];
        if (tmpNode == node) {
            bucket_[hash] = node->next;
        } else {
            while (tmpNode->next != node) tmpNode = tmpNode->next;
            tmpNode->next = node->next;
        }
        delete node;
        --size_;
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    Node** bucket_ = nullptr;
    SizeT size_ = 0;
    SizeT bucketSize_ = 0;
    Hash hash_;
    Equal equal_;
    RehashPolicy rehashPolicy_;
};

/**
 * In LinkedHashMap, iteration ordering is differ from map,
 * which is the order in which keys were inserted into the map.
 * You should maintain a doubly-linked list running through all
 * of its entries to keep the correct iteration order.
 *
 * Note that insertion order is not affected if a key is re-inserted
 * into the map.
     */
template<class Key,
         class T,
         class Hash = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class LinkedHashMap {
public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::LinkedHashMap as value_type by typedef.
     */
    typedef Pair<const Key, T> value_type;

    class PairHash {
    public:
        PairHash() = default;
        explicit PairHash(const Hash& hash) : hash_(hash) {}
        PairHash(const PairHash&) = default;

        PairHash& operator=(const PairHash&) = default;

        [[nodiscard]] std::size_t operator()(const value_type& pair) const { return hash_(pair.first); }

        template<class K>
        [[nodiscard]] std::size_t operator()(const K& key) const { return hash_(key); }

    private:
        Hash hash_;
    };

    class PairEqual {
    public:
        PairEqual() = default;
        explicit PairEqual(const Equal& equal) : equal_(equal) {}
        PairEqual(const PairEqual&) = default;

        PairEqual& operator=(const PairEqual&) = default;

        [[nodiscard]] bool operator()(const value_type& lhs, const value_type& rhs) const {
            return equal_(lhs.first, rhs.first);
        }

        template<class K>
        [[nodiscard]] bool operator()(const value_type& lhs, const K& rhs) const {
            return equal_(lhs.first, rhs);
        }

        template<class K>
        [[nodiscard]] bool operator()(const K& lhs, const value_type& rhs) const {
            return equal_(lhs, rhs.first);
        }

        template<class K1, class K2>
        [[nodiscard]] bool operator()(const K1& lhs, const K2& rhs) const {
            return equal_(lhs, rhs);
        }

    private:
        Equal equal_;
    };

    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = LinkedHashMap.begin(); --it;
     *       or it = LinkedHashMap.end(); ++end();
     */
    class ConstIterator;
    class Iterator {
        friend class LinkedHashMap;
        friend class ConstIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;


        Iterator() = default;
        Iterator(const Iterator&) = default;

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator++() {
            ++iterator_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        Iterator& operator--() {
            --iterator_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator==(const ConstIterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator!=(const Iterator& rhs) const { return this->iterator_ != rhs.iterator_; }
        bool operator!=(const ConstIterator& rhs) const { return this->iterator_ != rhs.iterator_; }

        value_type& operator*()  const { return iterator_->value; }
        value_type* operator->() const noexcept {return &(iterator_->value);}

    private:
        Iterator(const typename LinkedHashTable<value_type, PairHash, PairEqual>::Iterator& iterator)
            : iterator_(iterator) {}

        typename LinkedHashTable<value_type, PairHash, PairEqual>::Iterator iterator_;
    };

    class ConstIterator {
        friend class LinkedHashMap;
        friend class iterator;

        public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() = default;
        ConstIterator(const ConstIterator& other) = default;
        ConstIterator(const Iterator& obj) : iterator_(obj.iterator_) {}

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstIterator& operator++() {
            ++iterator_;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator tmp = *this;
            --(*this);
            return tmp;
        }

        ConstIterator& operator--() {
            --iterator_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator==(const ConstIterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator!=(const Iterator& rhs) const { return this->iterator_ != rhs.iterator_; }
        bool operator!=(const ConstIterator& rhs) const { return this->iterator_ != rhs.iterator_; }

        const value_type& operator*()  const { return iterator_->value; }
        const value_type* operator->() const { return &(iterator_->value); }

    private:
        ConstIterator(const typename LinkedHashTable<value_type, PairHash, PairEqual>::ConstIterator& iterator)
            : iterator_(iterator) {}
        typename LinkedHashTable<value_type, PairHash, PairEqual>::ConstIterator iterator_;
    };

    LinkedHashMap() = default;
    LinkedHashMap(const LinkedHashMap&) = default;
    LinkedHashMap(LinkedHashMap&&) noexcept = default;

    LinkedHashMap& operator=(const LinkedHashMap&) = default;
    LinkedHashMap& operator=(LinkedHashMap&&) noexcept = default;

    ~LinkedHashMap() = default;

    /**
     * Access specified element with bounds checking.  If no such element
     * exists, an exception of type <code>index_out_of_bound</code>
     * @return the reference to the mapped value of the element with key equal
     * to key.
     */
    T& At(const Key& key) {
        auto iter = table_.Find(key);
        if (iter == table_.End()) {
            throw OutOfBound();
        }
        return iter->value.second;
    }

    const T& At(const Key& key) const {
        auto iter = table_.Find(key);
        if (iter == table_.ConstEnd()) {
            throw OutOfBound();
        }
        return iter->value.second;
    }

    /**
     * Access or create a specified element.  If the element does exist, this
     * function will return the reference to the mapped value of the element.
     * If the element does not exist, this function will insert a new element
     * with the specified key and the default value, and return the reference
     * to the mapped value.
     *
     * @param key the input key
     * @return the reference to the value that is mapped to a key equal to
     * the input key, or the reference to the value that is inserted just now.
     */
    T& operator[](const Key& key) {
        auto [iter, success] = table_.Insert(value_type(key, T()));
        return iter->value.second;
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T& operator[](const Key& key) const {
        auto iter = table_.Find(key);
        if (iter == table_.ConstEnd()) {
            throw OutOfBound();
        }
        return iter->value.second;
    }

    /**
     * return a iterator to the beginning
     */
    Iterator begin() { return Iterator(table_.Begin()); }
    ConstIterator ConstBegin() const { return ConstIterator(table_.Begin()); }

    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    Iterator end() { return Iterator(table_.End()); }
    ConstIterator ConstEnd() const { return ConstIterator(table_.ConstEnd()); }

    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    [[nodiscard]] bool Empty() const { return table_.Empty(); }

    /**
     * returns the number of elements.
     */
    [[nodiscard]] size_t Size() const { return table_.Size(); }

    /**
     * clears the contents
     */
    void Clear() { table_.Clear(); }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    Pair<Iterator, bool> Insert(const value_type& value) {
        auto [iter, success] = table_.Insert(value);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void Erase(Iterator position) {
        if (position.iterator_ == table_.End()) {
            throw InvalidIterator();
        }
        table_.Erase(position.iterator_);
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     */
    size_t Count(const Key& key) const { return table_.Count(key); }

    bool Contains(const Key& key) const { return table_.Contains(key); }

    template<class K>
    bool Contains(const K& key) const { return table_.Contains(key); }

    void ReserveAtLeast(SizeT size) { table_.ReserveAtLeast(size); }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    Iterator Find(const Key& key) { return Iterator(table_.Find(key)); }
    ConstIterator Find(const Key& key) const { return ConstIterator(table_.Find(key)); }

private:
    LinkedHashTable<value_type, PairHash, PairEqual> table_;
};

#endif // TICKET_SYSTEM_INCLUDE_LINKED_HASH_MAP_H
