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

#ifndef TICKET_SYSTEM_INCLUDE_USER_H
#define TICKET_SYSTEM_INCLUDE_USER_H

#include "fixed_string.h"
#include "utility.h"

using UserName   = FixedString<20>;
using Password   = HashPair;
using Name       = FixedString<20>;
using mailAdress = FixedString<30>;

struct User {
    UserName   userName;
    Password   password;
    Name       name; // Actually a UTF-8 string
    mailAdress mailAddress;
    long       orderInfo = -1; // for ``query_order''
    int        privilege = 0;
};

#endif // TICKET_SYSTEM_INCLUDE_USER_H
