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

#ifndef TICKET_SYSTEM_INCLUDE_UTILITY_H
#define TICKET_SYSTEM_INCLUDE_UTILITY_H

#include <iostream>
#include <utility>

#ifdef BOOST
#define ENDL "\n"
#else
#define ENDL std::endl
#endif

template<class T1, class T2>
class Pair {
public:
	T1 first;
	T2 second;
	constexpr Pair() : first(), second() {}
	Pair(const Pair &other) = default;
	Pair(const T1 &x, const T2 &y) : first(x), second(y) {}
	template<class U1, class U2>
	Pair(U1 &&x, U2 &&y) : first(x), second(y) {}
	template<class U1, class U2>
	Pair(const Pair<U1, U2> &other) : first(other.first), second(other.second) {}
	template<class U1, class U2>
	Pair(Pair<U1, U2> &&other) : first(other.first), second(other.second) {}

    bool operator==(const Pair &other) const {
        return first == other.first && second == other.second;
    }

    bool operator!=(const Pair &other) const {
        return !(*this == other);
    }

    bool operator<(const Pair &other) const {
        return first < other.first || (first == other.first && second < other.second);
    }
};

using HashPair = Pair<std::size_t, std::size_t>;

class HashPairHash {
public:
    bool operator()(const HashPair& pair) const {
        return pair.first ^ pair.second;
    }
};

int StringToInt(const std::string& string);

#endif // TICKET_SYSTEM_INCLUDE_UTILITY_H
