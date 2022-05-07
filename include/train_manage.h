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

#ifndef TICKET_SYSTEM_INCLUDE_TRAIN_MANAGE_H
#define TICKET_SYSTEM_INCLUDE_TRAIN_MANAGE_H

#include "BP_tree.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "train.h"

class TrainManage {
public:
    TrainManage();

    ~TrainManage();

    void Add(ParameterTable& input, long timeStamp);

    void Delete(ParameterTable& input, long timeStamp);

    void Release(ParameterTable& input, long timeStamp);

    void QueryTrain(ParameterTable& input);

    void QueryTicket(ParameterTable& input);

    void QueryTransfer(ParameterTable& input);

    void TryBuy(ParameterTable& input, long timeStamp);

    void QueryOrder(ParameterTable& input);

    void Refund(ParameterTable& input, long timeStamp);

    void RollBack(long timeStamp);

    void Clear();

private:
    BPTree<TrainID, long>         trainIndex_     = BPTree<TrainID, long>("train_index");
    TileStorage<Train>            trainData_      = TileStorage<Train>("train_data");
    TileStorage<TrainTicketCount> ticketData_     = TileStorage<TrainTicketCount>("ticket_data");
    BPTree<Station, long>         startIndex_     = BPTree<Station, long>("start_station_index");
    BPTree<Station, long>         terminalIndex_  = BPTree<Station, long>("terminal_station_index");
    TileStorage<Ticket>           userTicketData_ = TileStorage<Ticket>("user_ticket_data");
};

#endif // TICKET_SYSTEM_INCLUDE_TRAIN_MANAGE_H