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
#ifdef BOOST
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
#endif // BOOST

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
#ifdef ROLLBACK
        int rollbackTimeStamp = StringToInt(parameterTable['t']);
        if (rollbackTimeStamp > parameterTable.TimeStamp()) {
#ifdef PRETTY_PRINT
            std::cout << "[" << parameterTable.TimeStamp()
                      << "] Rollback failed: time stamp is newer than the current time stamp."
                      << std::endl;
#else
            std::cout << "[" << parameterTable.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        } else {
            trains.RollBack(rollbackTimeStamp);
            users.RollBack(rollbackTimeStamp);
#ifdef PRETTY_PRINT
            std::cout << "[" << parameterTable.TimeStamp()
                      << "] Rollback succeed: system have rolled back to "
                      << rollbackTimeStamp << std::endl;
#else
            std::cout << "[" << parameterTable.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
        }
#else
        std::cout << "[" << parameterTable.TimeStamp() << "] Rollback is NOT supported!" << std::endl;
#endif // ROLLBACK
    } else if (parameterTable.GetCommand() == "clean") {
        users.Clear();
        trains.Clear();
    } else if (parameterTable.GetCommand() == "exit") {
        std::cout << "[" << parameterTable.TimeStamp() << "] bye" << ENDL;
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
    TryCreateFile("ticket_data");
    TryCreateFile("station_index");
    TryCreateFile("user_ticket_data");
#ifdef ROLLBACK
    TryCreateFile("user_index_log");
    TryCreateFile("user_data_log");
    TryCreateFile("train_index_log");
    TryCreateFile("train_data_log");
    TryCreateFile("ticket_data_log");
    TryCreateFile("station_index_log");
    TryCreateFile("user_ticket_data_log");
#endif // ROLLBACK
}