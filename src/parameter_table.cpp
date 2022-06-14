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

#include "parameter_table.h"

#include <iostream>

#include "token_scanner.h"

namespace {

long ReadTimeStamp(const std::string& string) {
    long cursor = 1;
    long result = 0;
    if (string[cursor] == '-') {
        cursor++;
        while (cursor < string.size() - 1) {
            result = result * 10 - string[cursor] + '0';
            ++cursor;
        }
    } else {
        while (cursor < string.size() - 1) {
            result = result * 10 + string[cursor] - '0';
            ++cursor;
        }
    }
    return result;
}

}

std::string& ParameterTable::operator[](char c) {
    return table_[c - 'a'];
}

const std::string& ParameterTable::operator[](char c) const {
    return table_[c - 'a'];
}

const std::string& ParameterTable::GetCommand() const {
    return command_;
}

long ParameterTable::TimeStamp() const {
    return timeStamp_;
}

void ParameterTable::ReadNewLine() {
    TokenScanner scanner;
    scanner.NewLine();
    timeStamp_ = ReadTimeStamp(scanner.NextToken());
    command_ = scanner.NextToken();
    while (scanner.HasMoreToken()) {
        char c = scanner.NextToken()[1] - 'a';
        table_[c] = scanner.NextToken();
    }
}

#ifdef LAU_TEST

void ParameterTable::Print() const {
    std::cout << "TimeStamp: " << timeStamp_ << std::endl;
    std::cout << "Command: " << command_ << std::endl;
    for (int i = 0; i < 26; ++i) {
        if (table_[i].empty()) {
            continue;
        }
        std::cout << char('a' + i) << ": " << table_[i] << std::endl;
    }
}

#endif // LAU_TEST
