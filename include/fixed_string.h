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
    char data[size + 1];

    char& operator[](long index);
    bool operator==(const FixedString& rhs);
    bool operator<(const FixedString& rhs);
};

template<long size>
struct FixedStringHash {
    std::size_t operator()(); // get the string hash
};

#endif // TICKET_SYSTEM_INCLUDE_FIXED_STRING_H
