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

#ifndef TICKET_SYSTEM_INCLUDE_PARAMETER_TABLE_H
#define TICKET_SYSTEM_INCLUDE_PARAMETER_TABLE_H

#include <iostream>

class ParameterTable {
public:
    ParameterTable() = default;

    ~ParameterTable() = default;

    [[nodiscard]] std::string& operator[](char c);
    [[nodiscard]] const std::string& operator[](char c) const;

    [[nodiscard]] const std::string& GetCommand() const;

    void ReadNewLine();

    [[nodiscard]] long TimeStamp() const;

#ifdef LAU_TEST
    void Print() const;
#endif // LAU_TEST

private:
    long timeStamp_;
    std::string command_;
    std::string table_[26];
};

#endif // TICKET_SYSTEM_INCLUDE_PARAMETER_TABLE_H
