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

#ifndef TICKET_SYSTEM_INCLUDE_USER_MANAGE_H
#define TICKET_SYSTEM_INCLUDE_USER_MANAGE_H

#include "BP_tree.h"
#include "fixed_string.h"
#include "linked_hash_map.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "user.h"

class LoginPool {
public:
    LoginPool();

    ~LoginPool();

    bool Contains(FixedString<20> name); // Tell whether a user has logged in

    const User& getData(FixedString<20> name);

    void Clear();

private:
    LinkedHashMap<FixedString<20>, User, FixedStringHash> loginUserMap_;
};

class UserManage {
public:
    UserManage();

    ~UserManage();

    void Adduser(ParameterTable& input, long timeStamp);

    void Login(ParameterTable& input);

    void Logout(ParameterTable& input);

    void Query(ParameterTable& input);

    void Modify(ParameterTable& input, long timeStamp);

    bool Logged(const std::string& name);

    long LastOrder(const std::string& name);

    long Addorder(const std::string& name, long position, long timeStamp);

    long ModifyLastOrderPtr(long position, long timeStamp);

    void RollBack(long time);

    void Clear();

private:
    LoginPool loginPool_;

    BPTree<UserName, long> userIndex_ = BPTree<UserName, long>("user_index");
    TileStorage<User>      userData_  = TileStorage<User>("user_data");
};

#endif // TICKET_SYSTEM_INCLUDE_USER_MANAGE_H
