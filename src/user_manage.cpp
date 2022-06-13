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
        std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
        return;
    }

    if (!loginPool_.Contains(input['c'])) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    if (user.privilege > loginPool_.GetData(input['c']).privilege) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    user.userName = input['u'];
    user.password = ToHashPair(input['p']);
    user.name = input['n'];
    user.mailAddress = input['m'];

    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
}

void UserManage::Adduser_(User& user, long timeStamp) {
    long position = userData_.Add(user);
    userIndex_.Insert(ToHashPair(user.userName), position);
}

void UserManage::Login(ParameterTable& input) {
    if (loginPool_.Contains(input['u'])) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);
    if (user.password != ToHashPair(input['p'])) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    loginPool_.Login(user);
    std::cout << "["<< input.TimeStamp() << "] 0" << ENDL;
}

void UserManage::Logout(ParameterTable& input) {
    if (!loginPool_.Contains(input['u'])) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    loginPool_.Logout(static_cast<FixedString<20>>(input['u']));
    std::cout << "["<< input.TimeStamp() << "] 0" << ENDL;
}

void UserManage::Query(ParameterTable& input) {
    if (!loginPool_.Contains(input['c'])) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);

    if (user.privilege >= loginPool_.GetData(input['c']).privilege &&
        input['c'] != input['u']) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    std::cout << "[" << input.TimeStamp() << "] "
              << user.userName << " " << user.name << " "
              << user.mailAddress << " " << user.privilege << ENDL;
}

void UserManage::Modify(ParameterTable& input) {
    if (!loginPool_.Contains(input['c'])) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);

    if (user.privilege >= loginPool_.GetData(input['c']).privilege &&
        input['c'] != input['u']) {
        std::cout << "["<< input.TimeStamp() << "] -1" << ENDL;
        return;
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
    if (!input['g'].empty()) {
        user.privilege = StringToInt(input['g']);
    }

    userData_.Modify(position, user);
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
    userData_.Modify(position, user);
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
