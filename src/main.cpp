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

bool Request(ParameterTable& parameterTable, UserManage& users, TrainManage& trains) {
    if (parameterTable.GetCommand() == "add_user") {
        users.AddUser(parameterTable);
    } else if (parameterTable.GetCommand() == "login") {
        users.Login(parameterTable);
    } else if (parameterTable.GetCommand() == "logout") {
        users.Logout(parameterTable);
    } else if (parameterTable.GetCommand() == "query_profile") {
        users.Query(parameterTable);
    } else if (parameterTable.GetCommand() == "modify_profile") {
        users.Modify(parameterTable);
    } else if (parameterTable.GetCommand() == "add_train") {
        trains.Add(parameterTable);
    } else if (parameterTable.GetCommand() == "delete_train") {
        trains.Delete(parameterTable);
    } else if (parameterTable.GetCommand() == "release_train") {
        trains.Release(parameterTable);
    } else if (parameterTable.GetCommand() == "query_train") {
        trains.QueryTrain(parameterTable);
    } else if (parameterTable.GetCommand() == "query_ticket") {
        trains.QueryTicket(parameterTable);
    } else if (parameterTable.GetCommand() == "query_transfer") {
        trains.QueryTransfer(parameterTable);
    } else if (parameterTable.GetCommand() == "buy_ticket") {
        trains.TryBuy(parameterTable, users);
    } else if (parameterTable.GetCommand() == "query_order") {
        trains.QueryOrder(parameterTable, users);
    } else if (parameterTable.GetCommand() == "refund_ticket") {
        trains.Refund(parameterTable, users);
    } else if (parameterTable.GetCommand() == "rollback") {
        // NOT SUPPORTED AT THIS MOMENT
    } else if (parameterTable.GetCommand() == "clean") {
        users.Clear();
        trains.Clear();
    } else if (parameterTable.GetCommand() == "exit") {
        return false;
    }
    return true;
}

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