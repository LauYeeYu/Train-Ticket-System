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

#include "train_manage.h"

#include "linked_hash_map.h"
#include "token_scanner.h"
#include "train.h"
#include "utility"
#include "vector.h"

void TrainManage::Add(ParameterTable& input, long timeStamp) {
    Train train;
    train.trainID = input['i'];
    if (trainIndex_.Contains(ToHashPair(train.trainID))) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
    }
    train.stationNum = StringToInt(input['n']);
    train.seatNum = StringToInt(input['m']);

    TokenScanner stations(input['s'], '|', TokenScanner::single);
    for (int i = 1; i <= train.stationNum; ++i) {
        train.stations[i] = stations.NextToken();
    }

    TokenScanner price(input['p'], '|', TokenScanner::single);
    int sum = 0;
    train.prefixPriceSum[1] = 0;
    for (int i = 2; i <= train.stationNum; ++i) {
        sum += StringToInt(price.NextToken());
        train.prefixPriceSum[i] = sum;
    }

    Time time(input['x']);
    TokenScanner travelTime(input['t'], '|', TokenScanner::single);
    TokenScanner stopTime(input['o'], '|', TokenScanner::single);
    train.departureTime[1] = train.arrivalTime[1] = time;
    for (int i = 2; i < train.stationNum; ++i) {
        time += StringToInt(travelTime.NextToken());
        train.arrivalTime[i] = time;
        time += StringToInt(stopTime.NextToken());
        train.departureTime[i] = time;
    }
    time += StringToInt(travelTime.NextToken());
    train.arrivalTime[train.stationNum] = train.departureTime[train.stationNum] = time;

    TokenScanner dates(input['d'], '|', TokenScanner::single);
    train.startDate = Date(dates.NextToken());
    train.endDate = Date(dates.NextToken());

    train.type = input['t'][0];

    long position = trainData_.Add(train);
    trainIndex_.Insert(ToHashPair(train.trainID), position);
    std::cout << "[" << timeStamp << "] 0" << ENDL;
}

void TrainManage::Delete(ParameterTable& input, long timeStamp) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }
    long position = trainIndex_.Find();
    trainData_.Delete(position);
    trainIndex_.Erase(ToHashPair(input['i']));
}

void TrainManage::Release(ParameterTable& input, long timeStamp) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (train.released) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }
    train.released = true;

    TrainTicketCount ticketCount;
    for (int i = train.startDate.day; i <= train.endDate.day; ++i) {
        for (int j = 1; j < train.stationNum; ++j) {
            ticketCount.remained[i][j] = train.seatNum;
        }
    }
    for (int i = 1; i < 100; ++i) {
        ticketCount.remained[98][i] = ticketCount.remained[99][i] = -1;
    }

    long ticketPosition = ticketData_.Add(ticketCount);
    train.ticketData = ticketPosition;
    for (int i = 1; i <= train.stationNum; ++i) {
        stationIndex_.Insert(ToHashPair(train.stations[i]), Pair<long, long>(position, i));
    }

    trainData_.Modify(position, train);

    std::cout << "[" << timeStamp << "] 0" << ENDL;
}

void TrainManage::QueryTrain(ParameterTable& input, long timeStamp) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    long position = trainIndex_.Find();
    Date date(input['d']);
    int day = date.day;
    Train train = trainData_.Get(position);
    if (date < train.startDate || date > train.endDate) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    if (train.released) {
        TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
        std::cout << "[" << timeStamp << "] " << train.trainID << " " << train.type << ENDL;

        std::cout << train.stations[1] << " xx-xx xx:xx -> "
                  << date + train.departureTime[1].minute / 1440 << " "
                  << train.prefixPriceSum[1] << ticketCount.remained[day][1] << ENDL;
        for (int i = 2; i < train.stationNum; ++i) {
            std::cout << train.stations[i] << " "
                      << date + train.arrivalTime[i].minute / 1440 << " "
                      << train.arrivalTime[i] << " -> "
                      << date + train.departureTime[i].minute / 1440 << " "
                      << train.prefixPriceSum[i] << ticketCount.remained[day][i] << ENDL;
        }
        std::cout << train.stations[train.stationNum] << " "
                  << date + train.arrivalTime[train.stationNum].minute / 1440 << " "
                  << train.arrivalTime[train.stationNum] << " -> xx-xx xx:xx "
                  << train.prefixPriceSum[train.stationNum] << " x" << ENDL;
    } else {
        std::cout << "[" << timeStamp << "] " << train.trainID << " " << train.type << ENDL;

        std::cout << train.stations[1] << " xx-xx xx:xx -> "
                  << date + train.departureTime[1].minute / 1440 << " "
                  << train.prefixPriceSum[1] << train.seatNum << ENDL;
        for (int i = 2; i < train.stationNum; ++i) {
            std::cout << train.stations[i] << " "
            << date + train.arrivalTime[i].minute / 1440 << " "
            << train.arrivalTime[i] << " -> "
            << date + train.departureTime[i].minute / 1440 << " "
            << train.prefixPriceSum[i] << train.seatNum << ENDL;
        }
        std::cout << train.stations[train.stationNum] << " "
                  << date + train.arrivalTime[train.stationNum].minute / 1440 << " "
                  << train.arrivalTime[train.stationNum] << " -> xx-xx xx:xx "
                  << train.prefixPriceSum[train.stationNum] << " x" << ENDL;
    }
}

void TrainManage::QueryTicket(ParameterTable& input, long timeStamp) {
    auto start = stationIndex_.MultiFind(ToHashPair(input['s']));
    auto end = stationIndex_.MultiFind(ToHashPair(input['t']));
    LinkedHashMap<long, long> ticketIndex;
    ticketIndex.ReserveAtLeast(512);
    Vector<Journey> journeys;
    for (auto& i : start) {
        ticketIndex[i.first] = i.second;
    }
    for (auto& i : end) {
        if (ticketIndex.Contains(i.first) && ticketIndex[i.first] < i.second) {
            Train train = trainData_.Get(i.first);
            Date date(input['d']);
            date -= train.departureTime[ticketIndex[i.first]].minute / 1440;
            if (date < train.startDate || date > train.endDate) {
                continue;
            }
            TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
            int ticketNum = ticketCount.remained[date.day][ticketIndex[i.first]];
            for (int j = ticketIndex[i.first] + 1; j < train.stationNum; ++j) {
                ticketNum = std::min(ticketNum,ticketCount.remained[date.day][j]);
            }
            Journey journey;
            journey.trainID = train.trainID;
            journey.startStation = train.stations[ticketIndex[i.first]];
            journey.endStation = train.stations[i.second];
            journey.startDate = date + train.departureTime[ticketIndex[i.first]].minute / 1440;
            journey.startTime = train.departureTime[ticketIndex[i.first]];
            journey.endDate = date + train.arrivalTime[i.second].minute / 1440;
            journey.endTime = train.arrivalTime[i.second];
            journey.price = train.prefixPriceSum[i.second] - train.prefixPriceSum[ticketIndex[i.first]];
            journey.seat = ticketNum;
            journeys.PushBack(journey);
        }
    }
    if (input['p'].empty() || input['p'][1] == 't') {
        journeys.Sort([](const Journey& a, const Journey& b) {
            if (a.endTime.minute - a.startTime.minute != b.endTime.minute - b.startTime.minute) {
                return a.endTime.minute - a.startTime.minute < b.endTime.minute - b.startTime.minute;
            }
            return a.trainID < b.trainID;
        });
    } else {
        journeys.Sort([](const Journey& a, const Journey& b) {
            if (a.price != b.price) {
                return a.price < b.price;
            }
            return a.trainID < b.trainID;
        });
    }
    std::cout << "[" << timeStamp << "] " << journeys.Size() << ENDL;
    for (auto& i : journeys) {
        std::cout << i << ENDL;
    }
}

void TrainManage::TryBuy(ParameterTable& input, UserManage& userManage, long timeStamp) {
    if (!userManage.Logged(input['u'])) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (!train.released) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    // find the departure and arrival station
    int departure = 0, arrival = 0;
    for (int i = 1; i <= train.stationNum; ++i) {
        if (train.stations[i] == input['f']) {
            departure = i;
        }
    }
    if (departure == 0) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }
    for (int i = departure + 1; i <= train.stationNum; ++i) {
        if (train.stations[i] == input['t']) {
            arrival = i;
        }
    }
    if (arrival == 0) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    // stuff of the date
    Date date(input['d']);
    Date trainDate = date - train.departureTime[departure].minute / 1440;

    if (trainDate < train.startDate || trainDate > train.endDate) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    // Now there exist a train that can serve the request
    TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
    int ticketNum = ticketCount.remained[trainDate.day][departure];
    int n = StringToInt(input['n']);
    for (int i = departure + 1; i < arrival; ++i) {
        ticketNum = std::min(ticketNum, ticketCount.remained[trainDate.day][i]);
    }

    //
    if (ticketNum >= n) { // Enough ticket(s)
        for (int i = departure; i < arrival; ++i) {
            ticketCount.remained[trainDate.day][i] -= n;
        }
        ticketData_.Modify(train.ticketData, ticketCount);
        Ticket ticket;
        ticket.trainID = train.trainID;
        ticket.startStation = train.stations[departure];
        ticket.endStation = train.stations[arrival];
        ticket.startDate = date;
        ticket.endDate = trainDate + train.arrivalTime[arrival].minute / 1440;
        ticket.startTime = train.departureTime[departure];
        ticket.endTime = train.arrivalTime[arrival];
        ticket.trainPosition = position;
        ticket.index = trainDate.day;
        ticket.price = train.prefixPriceSum[arrival] - train.prefixPriceSum[departure];
        ticket.from = departure;
        ticket.to = arrival;
        ticket.state = 1;
        ticket.seatNum = n;
        userManage.AddOrder(input['u'], ticket, timeStamp, *this);
        std::cout << "[" << timeStamp << "] " << ticket.price * n << ENDL;
    } else {
        if (input['q'].empty() || input['q'][0] == 'f') {
            std::cout << "[" << timeStamp << "] -1" << ENDL;
        } else {
            Ticket ticket;
            ticket.trainID = train.trainID;
            ticket.startStation = train.stations[departure];
            ticket.endStation = train.stations[arrival];
            ticket.startDate = date;
            ticket.endDate = trainDate + train.arrivalTime[arrival].minute / 1440;
            ticket.startTime = train.departureTime[departure];
            ticket.endTime = train.arrivalTime[arrival];
            ticket.trainPosition = position;
            ticket.index = trainDate.day;
            ticket.price = train.prefixPriceSum[arrival] - train.prefixPriceSum[departure];
            ticket.from = departure;
            ticket.to = arrival;
            ticket.state = 0;
            ticket.seatNum = n;
            ticketCount.remained[99][trainDate.day] = userManage.AddOrder(input['u'], ticket, timeStamp, *this);
            if (ticketCount.remained[98][trainDate.day] == -1) {
                ticketCount.remained[98][trainDate.day] = ticketCount.remained[99][trainDate.day];
            }
            ticketData_.Modify(train.ticketData, ticketCount);
            std::cout << "[" << timeStamp << "] queue" << ENDL;
        }
    }

}

long TrainManage::AddOrder(Ticket& ticket, long timeStamp) {
    return userTicketData_.Add(ticket);
}

void TrainManage::QueryOrder(ParameterTable& input, UserManage& userManage, long timeStamp) {
    if (!userManage.Logged(input['u'])) {
        std::cout << "[" << timeStamp << "] -1" << ENDL;
        return;
    }

    long OrderPtr = userManage.GetUser(input['u']).orderInfo;
    Vector<Ticket> tickets;
    while (OrderPtr != -1) {
        tickets.PushBack(userTicketData_.Get(OrderPtr));
        OrderPtr = tickets.Back().last;
    }
    std::cout << "[" << timeStamp << "] " << tickets.Size() << ENDL;
    for (auto& i : tickets) {
        std::cout << i << ENDL;
    }
}

void TrainManage::Clear() {
    trainData_.Clear();
    trainIndex_.Clear();
    ticketData_.Clear();
    userTicketData_.Clear();
    stationIndex_.Clear();
}
