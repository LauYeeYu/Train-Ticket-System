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

#include "user_manage.h"

#include "fixed_string.h"
#include "user.h"
#include "utility.h"
#include "train.h"
#include "train_manage.h"

bool LoginPool::Contains(const std::string& name) {
    return loginUserMap_.Contains(static_cast<FixedString<20>>(name));
}

bool LoginPool::Contains(const FixedString<20>& name) {
    return loginUserMap_.Contains(name);
}

const User& LoginPool::GetData(const FixedString<20>& name) {
    return loginUserMap_[name];
}

const User& LoginPool::GetData(const std::string& name) {
    return loginUserMap_[static_cast<FixedString<20>>(name)];
}

void LoginPool::Clear() {
    loginUserMap_.Clear();
}

bool LoginPool::Empty() {
    return loginUserMap_.Empty();
}

void LoginPool::Login(const User& user) {
    loginUserMap_[user.userName] = user;
}

void LoginPool::Logout(const UserName& userName) {
    loginUserMap_.Erase(loginUserMap_.Find(userName));
}

void LoginPool::ModifyProfile(const User& user) {
    loginUserMap_[user.userName] = user;
}

void UserManage::AddUser(ParameterTable& input) {
    User user;
    user.privilege = StringToInt(input['g']);
    if (userIndex_.Empty()) {
        user.userName = input['u'];
        user.password = ToHashPair(input['p']);
        user.name = input['n'];
        user.mailAddress = input['m'];
        user.privilege = 10;
        Adduser_(user, input.TimeStamp());
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] User "
                  << user.userName << " added successfully." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    if (!loginPool_.Contains(input['c'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Add failed: user "
                  << input['c'] << " hasn't logged in yet." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    if (user.privilege >= loginPool_.GetData(input['c']).privilege) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Add failed: unauthorized operation." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    if (userIndex_.Contains(ToHashPair(input['u']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Add failed: user "
                  << input['u'] << " already exists." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    user.userName = input['u'];
    user.password = ToHashPair(input['p']);
    user.name = input['n'];
    user.mailAddress = input['m'];
    Adduser_(user, input.TimeStamp());

#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] User "
              << user.userName << " added successfully." << std::endl;
#else
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
}

void UserManage::Adduser_(User& user, long timeStamp) {
    long position = userData_.Add(user);
#ifdef ROLLBACK
    userIndex_.Insert(ToHashPair(user.userName), position, timeStamp);
#else
    userIndex_.Insert(ToHashPair(user.userName), position);
#endif // ROLLBACK
}

void UserManage::Login(ParameterTable& input) {
#ifndef GUI
    if (loginPool_.Contains(input['u'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Login failed: user "
                  << input['u'] << " has already logged in." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
#endif // GUI

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Login failed: user "
                  << input['u'] << " doesn't exist." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);
    if (user.password != ToHashPair(input['p'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Login failed: incorrect password."
                  << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
#ifdef GUI
    if (loginPool_.Contains(input['u'])) {
        std::cout << "[" << input.TimeStamp()
                  << "] Login failed: the user has already logged in."
                  << std::endl;
        return;
    }
#endif // GUI
    loginPool_.Login(user);
#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] Login successfully." << std::endl;
#else
    std::cout << "["<< input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
}

void UserManage::Logout(ParameterTable& input) {
    if (!loginPool_.Contains(input['u'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Logout failed: user "
                  << input['u'] << " hasn't logged in yet." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    loginPool_.Logout(static_cast<FixedString<20>>(input['u']));
#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] Logout successfully." << std::endl;
#else
    std::cout << "["<< input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
}

void UserManage::Query(ParameterTable& input) {
    if (!loginPool_.Contains(input['c'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Query failed: user "
                  << input['c'] << " hasn't logged in yet." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Query failed: target user "
                  << input['u'] << " doesn't exist." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);
    const User& operationUser = loginPool_.GetData(input['c']);

    if (user.privilege > operationUser.privilege ||
       (user.privilege == operationUser.privilege &&
       input['c'] != input['u'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Query failed: unauthorized operation." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    std::cout << "[" << input.TimeStamp() << "] "
              << user.userName << " " << user.name << " "
              << user.mailAddress << " " << user.privilege << ENDL;
}

void UserManage::Modify(ParameterTable& input) {
    if (!loginPool_.Contains(input['c'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Modify failed: user "
                  << input['c'] << " hasn't logged in yet." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Modify failed: target user "
                  << input['u'] << " doesn't exist." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);
    const User& operationUser = loginPool_.GetData(input['c']);

    if (user.privilege > operationUser.privilege ||
        (user.privilege == operationUser.privilege &&
         input['c'] != input['u'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Modify failed: unauthorized operation." << std::endl;
#else
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    if (!input['g'].empty()) {
        int privilege = StringToInt(input['g']);
#ifdef GUI
        if (privilege > operationUser.privilege) {
#else
        if (privilege >= operationUser.privilege) {
#endif // GUI
#ifdef PRETTY_PRINT
            std::cout << "[" << input.TimeStamp()
                      << "] Modify failed: privilege is higher than operating user."
                      << std::endl;
#else
            std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
            return;
        }
        user.privilege = privilege;
    }
    if (!input['p'].empty()) {
        user.password = ToHashPair(input['p']);
    }
    if (!input['n'].empty()) {
        user.name = input['n'];
    }
    if (!input['m'].empty()) {
        user.mailAddress = input['m'];
    }

#ifdef ROLLBACK
    userData_.Modify(position, user, input.TimeStamp());
#else
    userData_.Modify(position, user);
#endif // ROLLBACK
    if (loginPool_.Contains(input['u'])) {
        loginPool_.ModifyProfile(user);
    }

    std::cout << "["<< input.TimeStamp() << "] "
              << user.userName << " " << user.name << " "
              << user.mailAddress << " " << user.privilege << ENDL;
}

long UserManage::AddOrder(const std::string& name, Ticket& ticket,
                          long timeStamp, TrainManage& trainManage) {
    if (!userIndex_.Contains(ToHashPair(name))) {
        return -1;
    }
    long position = userIndex_.Find();
    User user = userData_.Get(position);
    ticket.last = user.orderInfo;
    user.orderInfo = trainManage.AddOrder(ticket, timeStamp);
#ifdef ROLLBACK
    userData_.Modify(position, user, timeStamp);
#else
    userData_.Modify(position, user);
#endif // ROLLBACK
    if (loginPool_.Contains(name)) {
        loginPool_.ModifyProfile(user);
    }
    return user.orderInfo;
}

bool UserManage::Logged(const std::string& name) {
    return loginPool_.Contains(name);
}

const User& UserManage::GetUser(const std::string& name) {
    return loginPool_.GetData(name);
}

void UserManage::Clear() {
    userData_.Clear();
    userIndex_.Clear();
    loginPool_.Clear();
}

#ifdef ROLLBACK
void UserManage::RollBack(long timeStamp) {
    loginPool_.Clear();
    userIndex_.RollBack(timeStamp);
    userData_.RollBack(timeStamp);
}
#endif // ROLLBACK
