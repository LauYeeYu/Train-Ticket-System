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

#include "utility.h"

#include <iostream>
#include "user_manage.h"


int StringToInt(const std::string& string) {
    int result = 0;
    for (char i : string) {
        result = result * 10 + i - '0';
    }
    return result;
}


HashPair ToHashPair(const std::string& string) {
    return HashPair(FixedStringHash1()(string),
                    FixedStringHash2()(string));
}
