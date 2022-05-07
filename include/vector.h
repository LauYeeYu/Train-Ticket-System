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

/**
 * @file lau/vector.h
 *
 * This is a external header file, including the stuff about <code>Vector</code>.
 */

#ifndef TICKET_SYSTEM_INCLUDE_LAU_VECTOR_H
#define TICKET_SYSTEM_INCLUDE_LAU_VECTOR_H

#include <climits>
#include <cstddef>
#include <memory>
#include <utility>

using SizeT = long;

#include "exceptions.h"

/**
 * @class Vector
 *
 * A container that store data in a linear order.  But different from
 * <code>std::vector</code>, this class only store the pointer(s) to data
 * together, making it easier and faster to write data and to change capacity.
 * @tparam T the value type in the vector
 * @tparam Allocator the memory allocator
 */
template<class T>
class Vector {
public:
    class Iterator;
    class ConstIterator;

    class Iterator {
        friend ConstIterator;
        friend Vector<T>;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::output_iterator_tag;

        Iterator() noexcept = default;
        Iterator(const Iterator& obj) noexcept = default;

        Iterator& operator=(const Iterator& obj) noexcept = default;

        ~Iterator() = default;

        explicit operator T*() const noexcept { return objPtr_; }

        Iterator operator+(SizeT n) const noexcept { return Iterator(objPtr_ + n, vectorPtr_); }
        Iterator operator-(SizeT n) const noexcept { return Iterator(objPtr_ - n, vectorPtr_); }

         /**
          * Return the distance between two iterators.  If these two iterators
          * point to different vectors, throw invalid_iterator.
          * @param rhs
          * @return the distance between two iterators
          */
        difference_type operator-(const Iterator &rhs) const {
            if (this->vectorPtr_ != rhs.vectorPtr_) {
                throw InvalidIterator();
            }
            return this->objPtr_ - rhs.objPtr_;
        }

        Iterator& operator+=(SizeT n) noexcept {
            objPtr_ += n;
            return *this;
        }

        Iterator& operator-=(SizeT n) noexcept {
            objPtr_ -= n;
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++tmp;
            return tmp;
        }

        Iterator& operator++() noexcept {
            ++objPtr_;
            return *this;
        }

        Iterator operator--(int) noexcept {
            Iterator tmp = *this;
            --tmp;
            return tmp;
        }

        Iterator& operator--() noexcept {
            --objPtr_;
            return *this;
        }

        T& operator*() const { return **objPtr_; }
        T* operator->() const { return *objPtr_; }

        /**
         * Check whether two iterators are same (pointing to the same memory
         * address) or not.
         */
        bool operator==(const Iterator &rhs)      const noexcept { return (this->objPtr_ == rhs.objPtr_); }
        bool operator==(const ConstIterator &rhs) const noexcept { return (this->objPtr_ == rhs.objPtr_); }
        bool operator!=(const Iterator &rhs)      const noexcept { return (this->objPtr_ != rhs.objPtr_); }
        bool operator!=(const ConstIterator &rhs) const noexcept { return (this->objPtr_ != rhs.objPtr_); }

    private:
        Iterator(T** objPtr, const Vector<T>* vectorPtr) : objPtr_(objPtr), vectorPtr_(vectorPtr) {}

        T** objPtr_                 = nullptr;
        const Vector<T>* vectorPtr_ = nullptr;
    };

    class ConstIterator {
        friend Iterator;
        friend Vector<T>;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() noexcept = default;
        ConstIterator(const Iterator& iterator) noexcept : objPtr_(iterator.objPtr_),
                                                           vectorPtr_(iterator.vectorPtr_) {}

        ConstIterator(const ConstIterator& obj) noexcept = default;

        ConstIterator& operator=(const ConstIterator& obj) noexcept = default;

        ~ConstIterator() = default;

        explicit operator const T*() const noexcept { return objPtr_; }

        ConstIterator operator+(SizeT n) const noexcept { return ConstIterator(objPtr_ + n, vectorPtr_); }
        ConstIterator operator-(SizeT n) const noexcept { return ConstIterator(objPtr_ - n, vectorPtr_); }

        /**
          * Return the distance between two iterators.  If these two iterators
          * point to different vectors, throw invalid_iterator.
          * @param rhs
          * @return the distance between two iterators
          */
        difference_type operator-(const ConstIterator &rhs) const {
            if (this->vectorPtr_ != rhs.vectorPtr_) {
                throw InvalidIterator();
            }
            return this->objPtr_ - rhs.objPtr_;
        }

        ConstIterator& operator+=(SizeT n) noexcept {
            objPtr_ += n;
            return *this;
        }

        ConstIterator& operator-=(SizeT n) noexcept {
            objPtr_ -= n;
            return *this;
        }

        ConstIterator operator++(int) noexcept {
            ConstIterator tmp = *this;
            ++tmp;
            return tmp;
        }

        ConstIterator& operator++() noexcept {
            ++objPtr_;
            return *this;
        }

        ConstIterator operator--(int) noexcept {
            ConstIterator tmp = *this;
            --tmp;
            return tmp;
        }

        ConstIterator& operator--() noexcept {
            --objPtr_;
            return *this;
        }

        const T& operator*() const { return **objPtr_; }

        const T* operator->() const { return *objPtr_; }

        bool operator==(const Iterator &rhs) const noexcept      { return (this->objPtr_ == rhs.objPtr_); }
        bool operator==(const ConstIterator &rhs) const noexcept { return (this->objPtr_ == rhs.objPtr_); }
        bool operator!=(const Iterator &rhs) const noexcept      { return (this->objPtr_ != rhs.objPtr_); }
        bool operator!=(const ConstIterator &rhs) const noexcept { return (this->objPtr_ != rhs.objPtr_); }

    private:
        ConstIterator(T** objPtr, const Vector<T>* vectorPtr) : objPtr_(objPtr), vectorPtr_(vectorPtr) {}

        T** objPtr_                 = nullptr;
        const Vector<T>* vectorPtr_ = nullptr;
    };

    Vector() = default;

    Vector(SizeT count, const T& value) {
        this->Reserve(count);
        for (SizeT i = 0; i < count; ++i) {
            this->PushBack(value);
        }
    }

    explicit Vector(SizeT count) {
        this->Reserve(count);
        for (SizeT i = 0; i < count; ++i) {
            this->PushBack(T());
        }
    }

    template<class InputIterator>
    Vector(const InputIterator& begin,
           const InputIterator& end) {
        this->Reserve(end - begin);
        for (auto element = begin; element != end; ++element) {
            this->PushBack(*element);
        }
    }

    Vector(const Vector& obj) : capacity_(obj.size_),
                                size_(obj.size_),
                                beginIndex_(0) {
        target_ = new T*[capacity_];
        for (SizeT i = 0; i < size_; ++i) {
            target_[i] = new T(obj.target_[i + obj.beginIndex_]);
        }
    }

    Vector(Vector&& obj) noexcept : capacity_(obj.capacity_),
                                    size_(obj.size_),
                                    beginIndex_(obj.beginIndex_),
                                    target_(obj.target_) {
        obj.target_ = nullptr;
        obj.capacity_ = 0;
        obj.size_ = 0;
        obj.beginIndex_ = 0;
    }

    Vector(std::initializer_list<T> init) {
        this->Reserve(init.size());
        for (const auto& element : init) {
            this->PushBack(element);
        }
    }

    Vector& operator=(const Vector& obj) {
        if (&obj == this) return *this;
        this->Clear();
        capacity_ = obj.size_;
        size_ = obj.size_;
        beginIndex_ = 0;
        target_ = new T*[capacity_];
        for (SizeT i = 0; i < size_; ++i) {
            target_[i] = new T(*(obj.target_[i + obj.beginIndex_]));
        }
        return *this;
    }

    Vector& operator=(Vector&& obj) noexcept {
        this->Clear();
        capacity_ = obj.capacity_;
        size_ = obj.size_;
        beginIndex_ = obj.beginIndex_;
        target_ = obj.beginIndex_;
        obj.target_ = nullptr;
        obj.capacity_ = 0;
        obj.size_ = 0;
        obj.beginIndex_ = 0;
    }

    ~Vector() { this->Clear(); }

    /**
     * Access specified element with bounds checking.  If pos is not in range
     * [0, size), an <code>lau::OutOfRange</code> will be thrown.
     * @param index
     * @return a reference to the element at the input index
     */
    T& At(SizeT index) {
        if (index >= size_ || index < 0) throw OutOfBound();
        return *target_[index + beginIndex_];
    }

    /**
     * Access specified element with bounds checking.  If pos is not in range
     * [0, size), an <code>lau::OutOfRange</code> will be thrown.
     * @param index
     * @return a const reference to the element at the input index
     */
    const T& At(SizeT index) const {
        if (index >= size_ || index < 0) throw OutOfBound();
        return *target_[index + beginIndex_];
    }

    /**
     * Access specified element with bounds checking.  If pos is not in range
     * [0, size), an <code>lau::OutOfRange</code> will be thrown.
     * @param index
     * @return a reference to the element at the input index
     */
    T& operator[](SizeT index) {
        if (index >= size_ || index < 0) throw OutOfBound();
        return *target_[index + beginIndex_];
    }

    /**
     * assign specified element with bounds checking.  If pos is not in range
     * [0, size) , a <code>lau::OutOfRange</code> will be thrown.
     * @param index
     * @return a const reference to the element at the input index
     */
    const T& operator[](SizeT index) const {
        if (index >= size_ || index < 0) throw OutOfBound();
        return *target_[index + beginIndex_];
    }

    /**
     * Access the first element.  If the container is empty, a
     * <code>lau::EmptyContainer</code> will be thrown.
     * @return a constant reference to the first element
     */
    const T& Front() const {
        if (Empty()) throw EmptyContainer();
        return *target_[beginIndex_];
    }

    /**
     * Access the last element.  If the container is empty, a
     * <code>lau::EmptyContainer</code> will be thrown.
     * @return a const reference to the last element
     */
    const T& Back() const {
        if (Empty()) throw EmptyContainer();
        return *target_[size_ + beginIndex_ - 1];
    }

    /**
     * Access the first element.  If the container is empty, a
     * <code>lau::EmptyContainer</code> will be thrown.
     * @return a reference to the first element
     */
    T& Front() {
        if (Empty()) throw EmptyContainer();
        return *target_[beginIndex_];
    }

    /**
     * Access the last element.  If the container is empty, a
     * <code>lau::EmptyContainer</code> will be thrown.
     * @return a reference to the last element
     */
    T& Back() {
        if (Empty()) throw EmptyContainer();
        return *target_[size_ + beginIndex_ - 1];
    }

    [[nodiscard]] Iterator Begin() noexcept { return Iterator(target_ + beginIndex_, this); }
    [[nodiscard]] Iterator begin() noexcept { return this->Begin(); }

    [[nodiscard]] ConstIterator ConstBegin() const noexcept { return ConstIterator(target_ + beginIndex_, this); }
    [[nodiscard]] ConstIterator begin() const noexcept { return this->ConstBegin(); }
    [[nodiscard]] ConstIterator Begin() const noexcept { return this->ConstBegin(); }

    [[nodiscard]] Iterator End() noexcept { return Iterator(target_ + beginIndex_ + size_, this); }
    [[nodiscard]] Iterator end() noexcept { return End(); }

    [[nodiscard]] ConstIterator ConstEnd() const noexcept { return ConstIterator(target_ + beginIndex_ + size_, this); }
    [[nodiscard]] ConstIterator end() const noexcept { return this->ConstEnd(); }
    [[nodiscard]] ConstIterator End() const noexcept { return this->ConstEnd(); }

    /**
     * Check whether the container is empty.
     */
    [[nodiscard]] bool Empty() const noexcept { return size_ == 0; }

    /**
     * Return the number of elements.
     */
    [[nodiscard]] SizeT Size() const noexcept { return size_; }

    /**
     * Clear the whole vector class.
     */
    Vector& Clear() noexcept {
        for (SizeT i = 0; i < size_; ++i) {
            delete target_[i + beginIndex_];
            target_[i + beginIndex_] = nullptr;
        }
        delete[] target_;
        target_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        beginIndex_ = 0;
        return *this;
    }

    /**
     * Insert value before position.
     * @param position
     * @param value
     * @return an iterator pointing to the inserted value
     */
    Iterator Insert(const Iterator& position, const T& value) {
        SizeT index = position - this->Begin();
        return Insert(index, value);
    }

    /**
     * Insert value before position.
     * @param position
     * @param value
     * @return an iterator pointing to the inserted value
     */
    Iterator Insert(const ConstIterator& position, const T& value) {
        SizeT index = position - this->ConstBegin();
        return Insert(index, value);
    }

    /**
     * Insert value at index.  If <code>index > size</code>, a
     * <code>lau::OutOfRange</code> will be thrown.  After this
     * operation, <code>this->at(ind)</code> will be <code>value</code>.
     * @param index
     * @param value
     * @return an iterator pointing to the inserted value
     */
    Iterator Insert(SizeT index, const T& value) {
        if (index > size_) throw OutOfBound();
        if (index == 0) {
            PushFront(value);
            return Begin();
        }

        if (NeedEnlarging_()) Enlarge_();
        for (SizeT i = size_; i > index; --i) {
            target_[i + beginIndex_] = target_[i + beginIndex_ - 1];
        }
        target_[index + beginIndex_] = new T(value);
        ++size_;
        return Iterator(target_ + beginIndex_ + index, this);
    }

    /**
     * Erase the element at pos.  If the iterator pos refers the last
     * element, the <code>End()</code> iterator is returned.
     * @param position
     * @return an iterator pointing to the following element
     */
    Iterator Erase(const Iterator& position) {
        SizeT index = position - this->Begin();
        return Erase(index);
    }

    /**
     * Erase the element at pos.  If the iterator pos refers the last
     * element, the <code>End()</code> iterator is returned.
     * @param position
     * @return an iterator pointing to the following element
     */
    Iterator Erase(const ConstIterator& position) {
        SizeT index = position - this->ConstBegin();
        return Erase(index);
    }

    /**
     * Erase the element at index.  If <code>index >= size</code>, a
     * <code>lau::OutOfRange</code> will be thrown.
     * @param index
     * @return an iterator pointing to the following element
     */
    Iterator Erase(SizeT index) {
        if (index >= size_) throw OutOfBound();
        --size_;
        if (index == 0) {
            delete target_[beginIndex_];
            target_[beginIndex_] = nullptr;
            ++beginIndex_;
        } else {
            delete target_[index + beginIndex_];
            for (SizeT i = index; i < size_; ++i) {
                target_[i + beginIndex_] = target_[i + beginIndex_ + 1];
            }
        }
        if (size_ == 0) beginIndex_ = 0;
        return Iterator(target_ + beginIndex_ + index, this);
    }

    /**
     * Add an element to the end.
     * @param value
     * @return a reference to the current class
     */
    Vector& PushBack(const T& value) {
        if (NeedEnlarging_()) Enlarge_();
        target_[size_ + beginIndex_] = new T(value);
        ++size_;
        return *this;
    }

    /**
     * Add an element to the end.  This operation constructs a new element
     * in place.  The constructor of the element is called with exactly the
     * same arguments as supplied to the function.
     * @tparam Args
     * @param args
     * @return a reference to the current class
     */
    template<class... Args>
    Vector& EmplaceBack(Args&&... args) {
        if (NeedEnlarging_()) Enlarge_();
        target_[size_ + beginIndex_] = new T(std::forward<Args>(args)...);
        ++size_;
        return *this;
    }

    /**
     * Add an element to the front.
     * @param value
     * @return a reference to the current class
     */
    Vector& PushFront(const T& value) {
        if (beginIndex_ > 0) {
            target_[beginIndex_ - 1] = new T(value);
            --beginIndex_;
            ++size_;
        } else {
            if (NeedEnlarging_()) Enlarge_();
            for (SizeT i = size_; i > 0; --i) {
                target_[i] = target_[i - 1];
            }
            target_[0] = new T(value);
            ++size_;
        }
        return *this;
    }

    /**
     * Add an element to the front.  This operation constructs a new element
     * in place.  The constructor of the element is called with exactly the
     * same arguments as supplied to the function.
     * @tparam Args
     * @param args
     * @return a reference to the current class
     */
    template<class... Args>
    Vector& EmplaceFront(Args... args) {
        if (beginIndex_ > 0) {
            target_[beginIndex_ - 1] = new T(std::forward<Args>(args)...);
            --beginIndex_;
            ++size_;
        } else {
            if (NeedEnlarging_()) Enlarge_();
            for (SizeT i = size_; i > 0; --i) {
                target_[i] = target_[i - 1];
            }
            target_[0] = new T(std::forward<Args>(args)...);
            ++size_;
        }
        return *this;
    }

    /**
     * Remove the last element from the end.  If <code>size() == 0</code>, a
     * <code>lau::EmptyContainer</code> will be thrown.
     * @return a reference to the current class
     */
    Vector& PopBack() {
        if (size_ == 0) throw EmptyContainer();
        delete target_[size_ + beginIndex_ - 1];
        target_[size_ + beginIndex_ - 1] = nullptr;
        --size_;
        if (size_ == 0) beginIndex_ = 0;
        return *this;
    }

    /**
     * Remove the first element from the beginning.  If
     * <code>size() == 0</code>, a <code>lau::EmptyContainer</code> will be
     * thrown.
     * @return a reference to the current class
     */
    Vector& PopFront() {
        if (size_ == 0) throw EmptyContainer();
        --size_;
        delete target_[beginIndex_];
        target_[beginIndex_] = nullptr;
        ++beginIndex_;
        if (size_ == 0) beginIndex_ = 0;
        return *this;
    }

    /**
     * Reserve enough space of newCapacity.
     * @param newCapacity
     * @return a reference to the current class
     */
    Vector& Reserve(SizeT newCapacity) {
        if (newCapacity <= capacity_ - beginIndex_) return *this;

        T** tmp = target_;
        target_ = new T*[newCapacity];
        for (SizeT i = 0; i < size_; ++i) target_[i] = tmp[i + beginIndex_];
        for (SizeT i = size_; i < capacity_; ++i) target_[i] = nullptr;
        beginIndex_ = 0;
        delete[] tmp;
        capacity_ = newCapacity;
        return *this;
    }

    /**
     * Swap two vector
     * @param other
     * @return a reference to the current class
     */
    Vector& Swap(Vector& other) noexcept {
        SizeT tmp;

        tmp = other.size_;
        other.size_ = this->size_;
        this->size_ = tmp;

        tmp = other.capacity_;
        other.capacity_ = this->capacity_;
        this->capacity_ = tmp;

        tmp = other.beginIndex_;
        other.beginIndex_ = this->beginIndex_;
        this->beginIndex_ = tmp;

        T** tmpTarget = other.target_;
        other.target_ = this->target_;
        this->target_ = tmpTarget;

        return *this;
    }

    /**
     * Swap the data of two indexes
     * @param index1
     * @param index2
     * @return a reference to the current class
     */
    Vector& SwapElement(SizeT index1, SizeT index2) {
        if (index1 < 0 || index1 >= size_ || index2 < 0 || index2 >= size_) {
            throw OutOfBound();
        }
        T* tmp = target_[index1 + beginIndex_];
        target_[index1 + beginIndex_] = target_[index2 + beginIndex_];
        target_[index2 + beginIndex_] = tmp;
        return *this;
    }

    /**
     * Swap the data of two iterators
     * @param iterator1
     * @param iterator2
     * @return a reference to the current class
     */
    Vector& SwapElement(const Iterator& iterator1, const Iterator& iterator2) {
        Iterator beginIterator = this->Begin();
        SizeT index1 = iterator1 - beginIterator;
        SizeT index2 = iterator2 - beginIterator;
        return SwapElement(index1, index2);
    }

    /**
     * Reduce the capacity to its size.
     * @return a reference to the current class
     */
    Vector& ShrinkToFit() {
        if (size_ == capacity_) return *this;
        T** tmp = target_;
        target_ = new T*[size_];
        for (SizeT i = 0; i < size_; ++i) {
            target_[i] = tmp[i + beginIndex_];
        }
        delete[] tmp;
        capacity_ = size_;
        beginIndex_ = 0;
        return *this;
    }

    /**
     * Resize the container to contain count elements.  If the current size
     * is greater than count, the container is reduced to its first count
     * elements.  If the current size is less than count, additional
     * default elements are appended.
     * @param count
     * @return a reference to the current class
     */
    Vector& Resize(SizeT count) {
        if (size_ > count) {
            for (SizeT i = count; i < size_; ++i) {
                delete target_[i + beginIndex_];
                target_[i + beginIndex_] = nullptr;
            }
        } else {
            Reserve(count);
            for (SizeT i = size_; i < count; ++i) {
                target_[i + beginIndex_] = new T();
            }
        }
        size_ = count;
        return *this;
    }

    /**
     * Resize the container to contain count elements.  If the current size
     * is greater than count, the container is reduced to its first count
     * elements.  If the current size is less than count, additional
     * value elements are appended.
     * @param count
     * @param value
     * @return a reference to the current class
     */
    Vector& Resize(SizeT count, const T& value) {
        if (size_ > count) {
            for (SizeT i = count; i < size_; ++i) {
                delete target_[i + beginIndex_];
                target_[i + beginIndex_] = nullptr;
            }
        } else {
            Reserve(count);
            for (SizeT i = size_; i < count; ++i) {
                target_[i + beginIndex_] = new T(value);
            }
        }
        size_ = count;
        return *this;
    }

    /**
     * Get the capacity of the vector.  Please note that since its vector can
     * be popped from the front, so the maximum of the vector might be less
     * than the value.
     * @return the capacity of the vector
     */
    [[nodiscard]] SizeT Capacity() const noexcept { return capacity_; }

private:
    T**       target_     = nullptr;
    SizeT     size_       = 0;
    SizeT     capacity_   = 0;
    SizeT     beginIndex_ = 0;

    /**
     * Enlarge the vector for the case that the vector is right at its biggest
     * capacity.
     */
    void Enlarge_() {
        if (capacity_ == 0) {
            Reserve(4);
        } else {
            Reserve(capacity_ * 2);
        }
    }

    /// Check whether a vector needs enlarging.
    [[nodiscard]] bool NeedEnlarging_() const noexcept { return (capacity_ == beginIndex_ + size_); }

};

/**
 * Swap the content of two vectors of one certain type.
 * @tparam T the type of value in vector
 * @param vector1
 * @param vector2
 */
template<class T>
void Swap(Vector<T>& vector1, Vector<T>& vector2) noexcept {
    vector1.Swap(vector2);
}

#endif // TICKET_SYSTEM_INCLUDE_LAU_VECTOR_H
