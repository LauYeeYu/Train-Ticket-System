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

#ifndef TICKET_SYSTEM_INCLUDE_USER_MANAGE_H
#define TICKET_SYSTEM_INCLUDE_USER_MANAGE_H

#include "BP_tree.h"
#include "fixed_string.h"
#include "linked_hash_map.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "train.h"
#include "user.h"
#include "utility.h"

class TrainManage;

class LoginPool {
public:
    LoginPool() = default;

    ~LoginPool() = default;

    void Login(const User& user);

    void Logout(const UserName& userName);

    bool Contains(const FixedString<20>& name); // Tell whether a user has logged in

    bool Contains(const std::string& name); // Tell whether a user has logged in

    const User& GetData(const FixedString<20>& name);

    const User& GetData(const std::string& name);

    void ModifyProfile(const User& user);

    void Clear();

    bool Empty();

private:
    LinkedHashMap<UserName, User, FixedStringHash1> loginUserMap_;
};

class UserManage {
public:
    UserManage() = default;

    ~UserManage() = default;

    void AddUser(ParameterTable& input);

    void Login(ParameterTable& input);

    void Logout(ParameterTable& input);

    void Query(ParameterTable& input);

    void Modify(ParameterTable& input);

    bool Logged(const std::string& name);

    long AddOrder(const std::string& name, Ticket& ticket, long timeStamp, TrainManage& trainManage);

#ifdef ROLLBACK
    void RollBack(long timeStamp);
#endif

    void Clear();

    const User& GetUser(const std::string& name);

private:
    void Adduser_(User& user, long timeStamp);

    LoginPool loginPool_;

#ifdef ROLLBACK
    BPTree<HashPair, long> userIndex_ = BPTree<HashPair, long>("user_index", "user_index_log");
    TileStorage<User>      userData_  = TileStorage<User>("user_data", "user_data_log");
#else
    BPTree<HashPair, long> userIndex_ = BPTree<HashPair, long>("user_index");
    TileStorage<User>      userData_  = TileStorage<User>("user_data");
#endif
};

#endif // TICKET_SYSTEM_INCLUDE_USER_MANAGE_H
