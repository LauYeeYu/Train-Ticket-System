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

#ifndef TICKET_SYSTEM_INCLUDE_FIXED_STRING_H
#define TICKET_SYSTEM_INCLUDE_FIXED_STRING_H

#include <functional>
#include <ostream>

#include "utility.h"

template<long size>
struct FixedString {
public:
    FixedString() {
        data_[0] = '\0';
    }

    explicit FixedString(const char* str) {
        int i = 0;
        while (i < size && str[i] != '\0') {
            data_[i] = str[i];
            ++i;
        }
        data_[i] = '\0';
    }

    explicit FixedString(const std::string& str) {
        int i = 0;
        while (i < size && str[i] != '\0') {
            data_[i] = str[i];
            ++i;
        }
        data_[i] = '\0';
    }

    FixedString(const FixedString& str) {
        int i = 0;
        while (i < size && str.data_[i] != '\0') {
            data_[i] = str.data_[i];
            ++i;
        }
        data_[i] = '\0';
    }

    FixedString& operator=(const FixedString& str) {
        if (&str == this) return *this;

        int i = 0;
        while (i < size && str.data_[i] != '\0') {
            data_[i] = str.data_[i];
            ++i;
        }
        data_[i] = '\0';
        return *this;
    }

    FixedString& operator=(const std::string& str) {
        int i = 0;
        while (i < size && str[i] != '\0') {
            data_[i] = str[i];
            ++i;
        }
        data_[i] = '\0';
        return *this;
    }

    char& operator[](long index) { return data_[index]; }

    const char& operator[](long index) const { return data_[index]; }

    bool operator==(const FixedString& rhs) const {
        for (long i = 0; i < size; ++i) {
            if (this->data_[i] != rhs.data_[i]) {
                return false;
            }
            if (this->data_[i] == '\0') {
                return true;
            }
        }
        return true;
    }

    friend bool operator==(const FixedString& lhs, const std::string& rhs) {
        if (rhs.size() > size) return false;
        for (long i = 0; i < rhs.size(); ++i) {
            if (lhs.data_[i] != rhs[i]) {
                return false;
            }
        }
        return lhs.data_[rhs.size()] == '\0';
    }

    friend bool operator==(const std::string& lhs, const FixedString& rhs) {
        if (lhs.size() > size) return false;
        for (long i = 0; i < lhs.size(); ++i) {
            if (rhs.data_[i] != lhs[i]) {
                return false;
            }
        }
        return rhs.data_[rhs.size()] == '\0';
    }

    bool operator<(const FixedString& rhs) const {
        for (long i = 0; i < size; ++i) {
            if (this->data_[i] < rhs.data_[i]) {
                return true;
            } else if (this->data_[i] > rhs.data_[i]) {
                return false;
            }
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const FixedString& string) {
        os << string.data_;
        return os;
    }

private:
    char data_[size + 1];
};

class FixedStringHash1 {
public:
    FixedStringHash1() = default;
    FixedStringHash1(const FixedStringHash1&) = default;
    FixedStringHash1& operator=(const FixedStringHash1&) = default;
    ~FixedStringHash1() = default;

    template<long size>
    std::size_t operator()(const FixedString<size>& string) const {
        std::size_t hash = 0;
        for (long i = 0; i < size && string[i] != 0; ++i) {
            hash = hash * kPrime_;
            hash = hash + static_cast<unsigned char>(string[i]);
        }
        return hash;
    }

    std::size_t operator()(const std::string& string) const {
        std::size_t hash = 0;
        for (char i : string) {
            hash = hash * kPrime_;
            hash = hash + static_cast<unsigned char>(i);
        }
        return hash;
    }

private:
    constexpr static std::size_t kPrime_ = 1e9 + 7;
};

class FixedStringHash2 {
public:
    FixedStringHash2() = default;
    FixedStringHash2(const FixedStringHash2&) = default;
    FixedStringHash2& operator=(const FixedStringHash2&) = default;
    ~FixedStringHash2() = default;

    template<long size>
    std::size_t operator()(FixedString<size> string) const {
        std::size_t hash = 0;
        for (long i = 0; i < size && string[i] != 0; ++i) {
            hash = hash * kPrime_;
            hash = hash + static_cast<unsigned char>(string[i]);
        }
        return hash;
    }

    std::size_t operator()(const std::string& string) const {
        std::size_t hash = 0;
        for (char i : string) {
            hash = hash * kPrime_;
            hash = hash + static_cast<unsigned char>(i);
        }
        return hash;
    }

private:
    constexpr static std::size_t kPrime_ = 1e9 + 9;
};

template<long size>
HashPair ToHashPair(const FixedString<size>& string) {
    return HashPair(FixedStringHash1()(string),
                    FixedStringHash2()(string));
}

HashPair ToHashPair(const std::string& string);

class Hash {
public:
    Hash() = default;
    Hash(const Hash&) = default;
    Hash& operator=(const Hash&) = default;
    ~Hash() = default;

    std::size_t operator()(const Pair<std::size_t, std::size_t>& pair) const {
        return pair.first ^ pair.second;
    }
};

#endif // TICKET_SYSTEM_INCLUDE_FIXED_STRING_H
