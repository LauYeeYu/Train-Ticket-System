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

#ifndef TICKET_SYSTEM_INCLUDE_TRAIN_MANAGE_H
#define TICKET_SYSTEM_INCLUDE_TRAIN_MANAGE_H

#include "BP_tree.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "train.h"
#include "utility.h"
#include "user_manage.h"

using StationPair = Pair<long, long>;

class TrainManage {
public:
    TrainManage() = default;

    ~TrainManage() = default;

    void Add(ParameterTable& input);

    void Delete(ParameterTable& input);

    void Release(ParameterTable& input);

    void QueryTrain(ParameterTable& input);

    void QueryTicket(ParameterTable& input);

    void QueryTransfer(ParameterTable& input);

    void TryBuy(ParameterTable& input, UserManage& userManage);

    long AddOrder(Ticket& ticket, long timeStamp);

    void QueryOrder(ParameterTable& input, UserManage& userManage);

    void Refund(ParameterTable& input, UserManage& userManage);

#ifdef ROLLBACK
    void RollBack(long timeStamp);
#endif

    void Clear();

private:
#ifdef ROLLBACK
    BPTree<HashPair, long>        trainIndex_   = BPTree<HashPair, long>("train_index", "train_index_log");
    TileStorage<Train>            trainData_    = TileStorage<Train>("train_data", "train_data_log");
    TileStorage<TrainTicketCount> ticketData_   = TileStorage<TrainTicketCount>("ticket_data", "ticket_data_log");
    BPTree<HashPair, StationPair> stationIndex_ = BPTree<HashPair, StationPair>("station_index", "station_index_log");
    TileStorage<Ticket>         userTicketData_ = TileStorage<Ticket>("user_ticket_data", "user_ticket_data_log");
#else
    BPTree<HashPair, long>        trainIndex_     = BPTree<HashPair, long>("train_index");
    TileStorage<Train>            trainData_      = TileStorage<Train>("train_data");
    TileStorage<TrainTicketCount> ticketData_     = TileStorage<TrainTicketCount>("ticket_data");
    BPTree<HashPair, StationPair> stationIndex_   = BPTree<HashPair, StationPair>("station_index");
    TileStorage<Ticket>           userTicketData_ = TileStorage<Ticket>("user_ticket_data");
#endif
};

#endif // TICKET_SYSTEM_INCLUDE_TRAIN_MANAGE_H
