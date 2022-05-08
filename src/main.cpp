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

#include <fstream>
#include <iostream>

#include "parameter_table.h"
#include "train_manage.h"
#include "user_manage.h"


void TryCreateFile(const char* fileName);

void Init();

bool Request(ParameterTable& parameterTable, UserManage& users, TrainManage& trains);

int main() {
    Init();
    ParameterTable parameterTable;
    TrainManage trainManage;
    UserManage userManage;
    while (std::cin) {
        parameterTable.ReadNewLine();
        if (!Request(parameterTable, userManage, trainManage)) {
            break;
        }
    }
    return 0;
}

bool Request(ParameterTable& parameterTable, UserManage& users, TrainManage& trains);

void TryCreateFile(const char* fileName) {
    std::ifstream tester(fileName);
    if (!(tester.good())) {
        std::ofstream creator(fileName);
        creator.close();
    }
    tester.close();
}

void Init() {
    TryCreateFile("user_index");
    TryCreateFile("user_data");
    TryCreateFile("train_index");
    TryCreateFile("train_data");
    TryCreateFile("ticket_index");
    TryCreateFile("start_station_index");
    TryCreateFile("terminal_station_index");
    TryCreateFile("user_ticket_data");
}