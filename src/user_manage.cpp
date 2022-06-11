//
// Created by LauYeeYu on 2022/6/11.
//

#include "user_manage.h"

#include "fixed_string.h"
#include "user.h"
#include "utility.h"

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

void UserManage::Adduser(ParameterTable& input, long timeStamp) {
    User user;
    user.privilege = StringToInt(input['g']);
    if (userIndex_.Empty()) {
        user.userName = input['u'];
        user.password = ToHashPair(input['p']);
        user.name = input['n'];
        user.mailAddress = input['m'];
        user.privilege = 10;
        Adduser_(user, timeStamp);
        return;
    }

    if (!loginPool_.Contains(input['c'])) return;
    if (user.privilege > loginPool_.GetData(input['c']).privilege) return;

    user.userName = input['u'];
    user.password = ToHashPair(input['p']);
    user.name = input['n'];
    user.mailAddress = input['m'];
}

void UserManage::Adduser_(User& user, long timeStamp) {
    long position = userData_.Add(user);
    userIndex_.Insert(ToHashPair(user.userName), position);
}

void UserManage::Login(ParameterTable& input) {
    if (loginPool_.Contains(input['u'])) {
        std::cout << "-1" << ENDL;
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
        std::cout << "-1" << ENDL;
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);
    if (user.password != ToHashPair(input['p'])) {
        std::cout << "-1" << ENDL;
        return;
    }
    loginPool_.Login(user);
    std::cout << "0" << ENDL;
}

void UserManage::Logout(ParameterTable& input) {
    if (!loginPool_.Contains(input['u'])) {
        std::cout << "-1" << ENDL;
        return;
    }
    loginPool_.Logout(static_cast<FixedString<20>>(input['u']));
    std::cout << "0" << ENDL;
}

void UserManage::Query(ParameterTable& input) {
    if (!loginPool_.Contains(input['c'])) {
        std::cout << "-1" << ENDL;
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
        std::cout << "-1" << ENDL;
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);

    if (user.privilege >= loginPool_.GetData(input['c']).privilege &&
        input['c'] != input['u']) {
        std::cout << "-1" << ENDL;
        return;
    }
    std::cout << user.userName << " " << user.name << " "
              << user.mailAddress << " " << user.privilege << ENDL;
}

void UserManage::Modify(ParameterTable& input, long timeStamp) {
    if (!loginPool_.Contains(input['c'])) {
        std::cout << "-1" << ENDL;
        return;
    }

    if (!userIndex_.Contains(ToHashPair(input['u']))) {
        std::cout << "-1" << ENDL;
        return;
    }

    long position = userIndex_.Find();
    User user = userData_.Get(position);

    if (user.privilege >= loginPool_.GetData(input['c']).privilege &&
        input['c'] != input['u']) {
        std::cout << "-1" << ENDL;
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

    std::cout << user.userName << " " << user.name << " "
              << user.mailAddress << " " << user.privilege << ENDL;
}
