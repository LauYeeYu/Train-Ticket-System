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

#ifndef TICKET_SYSTEM_INCLUDE_FIXED_STRING_H
#define TICKET_SYSTEM_INCLUDE_FIXED_STRING_H

#include <functional>

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

    char& operator[](long index) { return data_[index]; }

    bool operator==(const FixedString& rhs) {
        for (long i = 0; i < size; ++i) {
            if (this->data_[i] != rhs.data_[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator<(const FixedString& rhs) {
        for (long i = 0; i < size; ++i) {
            if (this->data_[i] < rhs.data_[i]) {
                return true;
            } else if (this->data_[i] > rhs.data_[i]) {
                return false;
            }
        }
        return false;
    }

private:
    char data_[size + 1];
};

class FixedStringHash {
public:
    FixedStringHash() = default;
    FixedStringHash(const FixedStringHash&) = default;
    FixedStringHash& operator=(const FixedStringHash&) = default;
    ~FixedStringHash() = default;

    template<long size>
    std::size_t operator()(FixedString<size> string) {
        std::size_t hash = 0;
        for (long i = 0; i < size && string[i] != 0; ++i) {
            hash = (hash * kPrime_) % kMod_;
            hash = (hash + string[i] * kPrime_) % kMod_;
        }
        return hash;
    }

private:
    constexpr static std::size_t kPrime_ = 233;
    constexpr static std::size_t kMod_ = 137438953481;
};

#endif // TICKET_SYSTEM_INCLUDE_FIXED_STRING_H
