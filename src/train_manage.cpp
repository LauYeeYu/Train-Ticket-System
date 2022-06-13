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

bool CanBuyTicket(const TrainTicketCount& ticketCount, int day, int from, int to, int count) {
    for (int i = from; i < to; ++i) {
        if (ticketCount.remained[day][i] < count) {
            return false;
        }
    }
    return true;
}

void TrainManage::Add(ParameterTable& input) {
    Train train;
    train.trainID = input['i'];
    if (trainIndex_.Contains(ToHashPair(train.trainID))) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
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
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
}

void TrainManage::Delete(ParameterTable& input) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    long position = trainIndex_.Find();
    trainData_.Delete(position);
    trainIndex_.Erase(ToHashPair(input['i']));
}

void TrainManage::Release(ParameterTable& input) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (train.released) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
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

    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
}

void TrainManage::QueryTrain(ParameterTable& input) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long position = trainIndex_.Find();
    Date date(input['d']);
    int day = date.day;
    Train train = trainData_.Get(position);
    if (date < train.startDate || date > train.endDate) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    if (train.released) {
        TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
        std::cout << "[" << input.TimeStamp() << "] " << train.trainID << " " << train.type << ENDL;

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
        std::cout << "[" << input.TimeStamp() << "] " << train.trainID << " " << train.type << ENDL;

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

void TrainManage::QueryTicket(ParameterTable& input) {
    auto start = stationIndex_.MultiFind(ToHashPair(input['s']));
    auto end = stationIndex_.MultiFind(ToHashPair(input['t']));
    Date date(input['d']);
    LinkedHashMap<long, long> ticketIndex;
    ticketIndex.ReserveAtLeast(512);
    Vector<Journey> journeys;
    for (auto& i : start) {
        ticketIndex[i.first] = i.second;
    }
    for (auto& i : end) {
        if (ticketIndex.Contains(i.first) && ticketIndex[i.first] < i.second) {
            Train train = trainData_.Get(i.first);
            int tmpDate = date.day - train.departureTime[ticketIndex[i.first]].minute / 1440;
            if (tmpDate < train.startDate.day || tmpDate > train.endDate.day) {
                continue;
            }
            TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
            int ticketNum = ticketCount.remained[tmpDate][ticketIndex[i.first]];
            for (int j = ticketIndex[i.first] + 1; j < train.stationNum; ++j) {
                ticketNum = std::min(ticketNum,ticketCount.remained[tmpDate][j]);
            }
            Journey journey;
            journey.trainID = train.trainID;
            journey.startStation = train.stations[ticketIndex[i.first]];
            journey.endStation = train.stations[i.second];
            journey.startDate.day = tmpDate + train.departureTime[ticketIndex[i.first]].minute / 1440;
            journey.startTime = train.departureTime[ticketIndex[i.first]];
            journey.endDate.day = tmpDate + train.arrivalTime[i.second].minute / 1440;
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
    std::cout << "[" << input.TimeStamp() << "] " << journeys.Size() << ENDL;
    for (auto& i : journeys) {
        std::cout << i << ENDL;
    }
}

void TrainManage::TryBuy(ParameterTable& input, UserManage& userManage) {
    if (!userManage.Logged(input['u'])) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (!train.released) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
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
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }
    for (int i = departure + 1; i <= train.stationNum; ++i) {
        if (train.stations[i] == input['t']) {
            arrival = i;
        }
    }
    if (arrival == 0) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    // stuff of the date
    Date date(input['d']);
    Date trainDate = date - train.departureTime[departure].minute / 1440;

    if (trainDate < train.startDate || trainDate > train.endDate) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
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
        ticket.ticketPosition = train.ticketData;
        ticket.index = trainDate.day;
        ticket.price = train.prefixPriceSum[arrival] - train.prefixPriceSum[departure];
        ticket.from = departure;
        ticket.to = arrival;
        ticket.state = 1;
        ticket.seatNum = n;
        userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
        std::cout << "[" << input.TimeStamp() << "] " << ticket.price * n << ENDL;
    } else {
        if (input['q'].empty() || input['q'][0] == 'f') {
            std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
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
            ticket.ticketPosition = train.ticketData;
            ticket.index = trainDate.day;
            ticket.price = train.prefixPriceSum[arrival] - train.prefixPriceSum[departure];
            ticket.from = departure;
            ticket.to = arrival;
            ticket.state = 0;
            ticket.seatNum = n;
            ticketCount.remained[99][trainDate.day]
                = userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
            if (ticketCount.remained[98][trainDate.day] == -1) {
                ticketCount.remained[98][trainDate.day] = ticketCount.remained[99][trainDate.day];
            }
            long ticketPtr = userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);;
            Ticket queueTicket = userTicketData_.Get(ticketCount.remained[99][trainDate.day]);
            queueTicket.queue = ticketPtr;
            userTicketData_.Modify(ticketCount.remained[99][trainDate.day], queueTicket);
            std::cout << "[" << input.TimeStamp() << "] queue" << ENDL;
        }
    }

}

long TrainManage::AddOrder(Ticket& ticket, long timeStamp) {
    return userTicketData_.Add(ticket);
}

void TrainManage::QueryOrder(ParameterTable& input, UserManage& userManage) {
    if (!userManage.Logged(input['u'])) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    long OrderPtr = userManage.GetUser(input['u']).orderInfo;
    Vector<Ticket> tickets;
    while (OrderPtr != -1) {
        tickets.PushBack(userTicketData_.Get(OrderPtr));
        OrderPtr = tickets.Back().last;
    }
    std::cout << "[" << input.TimeStamp() << "] " << tickets.Size() << ENDL;
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

void TrainManage::Refund(ParameterTable& input, UserManage& userManage) {
    if (!userManage.Logged(input['u'])) {
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
        return;
    }

    // Get the pointer to the order
    long orderPtr = userManage.GetUser(input['u']).orderInfo;
    int number;
    if (input['n'].empty()) number = 1;
    else number = StringToInt(input['n']);
    Ticket ticket;
    ticket = userTicketData_.Get(orderPtr);
    if (ticket.state == 1) --number;
    while (number > 0) {
        orderPtr = ticket.last;
        if (orderPtr == -1) {
            std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
            return;
        }
        ticket = userTicketData_.Get(orderPtr);
        if (ticket.state == 1) --number;
    }

    // Refund the ticket
    ticket.state = -1;
    userTicketData_.Modify(orderPtr, ticket);
    TrainTicketCount ticketCount = ticketData_.Get(ticket.ticketPosition);
    for (int i = ticket.from; i < ticket.to; ++i) {
        ticketCount.remained[ticket.index][i] += ticket.seatNum;
    }

    // check the pending queue
    long queuePtr = ticketCount.remained[98][ticket.index];
    long nextPtr;
    // Nothing to pend
    if (queuePtr == -1) {
        ticketData_.Modify(ticket.ticketPosition, ticketCount);
        std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
        return;
    }
    ticket = userTicketData_.Get(queuePtr);

    // the case that the head nodes can be served
    while (CanBuyTicket(ticketCount, ticket.index, ticket.from, ticket.to, ticket.seatNum)) {
        ticket.state = 1;
        nextPtr = ticket.queue;
        ticket.queue = -1;
        userTicketData_.Modify(queuePtr, ticket);
        for (int i = ticket.from; i < ticket.to; ++i) {
            ticketCount.remained[ticket.index][i] -= ticket.seatNum;
        }
        queuePtr = nextPtr;
        if (queuePtr == -1) {
            ticketCount.remained[98][ticket.index] = -1;
            ticketCount.remained[99][ticket.index] = -1;
            ticketData_.Modify(ticket.ticketPosition, ticketCount);
            std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
            return;
        }
        ticket = userTicketData_.Get(queuePtr);
    }
    ticketCount.remained[98][ticket.index] = queuePtr;

    // this node cannot be served, so we need to move to the next node
    long lastPtr = queuePtr;
    queuePtr = ticket.queue;

    while (queuePtr != -1) {
        ticket = userTicketData_.Get(queuePtr);
        if (CanBuyTicket(ticketCount, ticket.index, ticket.from, ticket.to, ticket.seatNum)) {
            ticket.state = 1;
            nextPtr = ticket.queue;
            ticket.queue = -1;
            userTicketData_.Modify(queuePtr, ticket);
            for (int i = ticket.from; i < ticket.to; ++i) {
                ticketCount.remained[ticket.index][i] -= ticket.seatNum;
            }
            queuePtr = nextPtr;
        } else {
            lastPtr = queuePtr;
            queuePtr = ticket.queue;
        }
    }
    ticketCount.remained[99][ticket.index] = lastPtr;
    ticketData_.Modify(ticket.ticketPosition, ticketCount);
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
}

void TrainManage::QueryTransfer(ParameterTable& input) {
    Vector<Train> trains; // train2
    bool Found = false;
    int cost = 0, time = 0;
    Journey journey1, journey2;
    Vector<HashPair> stationHash;
    stationHash.Resize(101);

    auto start = stationIndex_.MultiFind(ToHashPair(input['s']));
    auto end = stationIndex_.MultiFind(ToHashPair(input['t']));
    Date date(input['d']);
    bool rule; // true for time, false for cost
    if (input['p'].empty() || input['p'][0] == 't') rule = true;
    else rule = false;

    auto* stations2 = new LinkedHashMap<HashPair, long, HashPairHash>[end.Size()];
    for (int i = 0; i < end.Size(); ++i) {
        trains.PushBack(trainData_.Get(end[i].first));
        for (int j = 1; j < end[i].second; ++j) {
            stations2[i][ToHashPair(trains.Back().stations[j])] = j;
        }
    }

    for (auto& startPtr: start) {
        Train train1 = trainData_.Get(startPtr.first);
        int tmpDate = date.day - train1.departureTime[startPtr.second].minute / 1440;
        if (tmpDate < train1.startDate.day || tmpDate > train1.endDate.day) continue;
        TrainTicketCount ticketCount1 = ticketData_.Get(train1.ticketData);
        int startDate = date.day - train1.departureTime[startPtr.second].minute / 1440;
        int remained1 = ticketCount1.remained[startDate][startPtr.second];

        for (int j = startPtr.second + 1; j <= train1.stationNum; ++j) {
            stationHash[j] = ToHashPair(train1.stations[j]);
        }
        for (int train2 = 0; train2 < end.Size(); ++train2) {
            if (end[train2].first == startPtr.first) continue; // eliminate the same train
            for (int j = startPtr.second + 1; j <= train1.stationNum; ++j) {
                remained1 = std::min(remained1, ticketCount1.remained[startDate][j - 1]);
                if (!stations2[train2].Contains(stationHash[j])) continue;
                int stationIndex2 = stations2[train2][stationHash[j]];
                int day = date.day - train1.departureTime[startPtr.second].minute / 1440
                          + train1.arrivalTime[j].minute / 1440;
                // eliminate the wrong date
                if (day > trains[train2].endDate.day) continue;
                if (day == trains[train2].endDate.day &&
                    train1.arrivalTime[j].minute % 1440 >
                    trains[train2].departureTime[stationIndex2].minute % 1440) {
                    continue;
                }
                int tmpCost, tmpTime, day2;
                if (rule) {
                    if (day < trains[train2].startDate.day
                              + trains[train2].departureTime[stationIndex2].minute / 1440) {
                        day2 = trains[train2].startDate.day
                               + trains[train2].arrivalTime[end[train2].second].minute / 1440;
                    } else if (train1.arrivalTime[j].minute % 1440 >
                               trains[train2].departureTime[stationIndex2].minute % 1440) {
                        day2 = day + 1
                               + trains[train2].arrivalTime[end[train2].second].minute / 1440
                               - trains[train2].departureTime[stationIndex2].minute / 1440;
                    } else {
                        day2 = day
                               + trains[train2].arrivalTime[end[train2].second].minute / 1440
                               - trains[train2].departureTime[stationIndex2].minute / 1440;
                    }
                    int dayCount = day2 - (day - train1.departureTime[startPtr.second].minute / 1440);
                    tmpTime = dayCount * 1440 + train1.departureTime[startPtr.second].minute % 1440
                              - train1.arrivalTime[end[train2].second].minute % 1440;
                    if (Found && tmpTime > time) continue;
                    tmpCost = train1.prefixPriceSum[j] - train1.prefixPriceSum[startPtr.second]
                              + trains[train2].prefixPriceSum[end[train2].second]
                              - trains[train2].prefixPriceSum[stationIndex2];
                    if (Found) {
                        if (tmpCost > cost) continue;
                        if (tmpCost == cost) {
                            if (journey1.trainID < train1.trainID) continue;
                            if (journey1.trainID == train1.trainID &&
                                journey2.trainID < trains[train2].trainID)
                                continue;
                        }
                    }
                } else {
                    tmpCost = train1.prefixPriceSum[j] - train1.prefixPriceSum[startPtr.second]
                              + trains[train2].prefixPriceSum[end[train2].second]
                              - trains[train2].prefixPriceSum[stationIndex2];
                    if (Found && tmpCost > cost) continue;
                    if (day < trains[train2].startDate.day
                              + trains[train2].departureTime[stationIndex2].minute / 1440) {
                        day2 = trains[train2].startDate.day
                               + trains[train2].arrivalTime[end[train2].second].minute / 1440;
                    } else if (train1.arrivalTime[j].minute % 1440 >
                               trains[train2].departureTime[stationIndex2].minute % 1440) {
                        day2 = day + 1
                               + trains[train2].arrivalTime[end[train2].second].minute / 1440
                               - trains[train2].departureTime[stationIndex2].minute / 1440;
                    } else {
                        day2 = day
                               + trains[train2].arrivalTime[end[train2].second].minute / 1440
                               - trains[train2].departureTime[stationIndex2].minute / 1440;
                    }
                    int dayCount = day2 - (day - train1.departureTime[startPtr.second].minute / 1440);
                    tmpTime = dayCount * 1440 + train1.departureTime[startPtr.second].minute % 1440
                              - train1.arrivalTime[end[train2].second].minute % 1440;
                    if (Found) {
                        if (tmpTime > time) continue;
                        if (tmpTime == time) {
                            if (journey1.trainID < train1.trainID) continue;
                            if (journey1.trainID == train1.trainID &&
                                journey2.trainID < trains[train2].trainID)
                                continue;
                        }
                    }
                }
                Found = true;
                cost = tmpCost;
                time = tmpTime;
                journey1.trainID = train1.trainID;
                journey1.startStation = train1.stations[startPtr.second];
                journey1.endStation = train1.stations[j];
                journey1.startTime = train1.departureTime[startPtr.second];
                journey1.startDate.day = day - train1.arrivalTime[j].minute / 1440
                                         + train1.departureTime[startPtr.second].minute / 1440;
                journey1.endTime = train1.arrivalTime[j];
                journey1.endDate.day = journey1.startDate.day;
                journey1.price = train1.prefixPriceSum[j] - train1.prefixPriceSum[startPtr.second];
                journey1.seat = remained1;

                journey2.trainID = trains[train2].trainID;
                journey2.startStation = trains[train2].stations[stationIndex2];
                journey2.endStation = trains[train2].stations[end[train2].second];
                journey2.startTime = trains[train2].departureTime[stationIndex2];
                journey2.startDate.day = day2 - trains[train2].arrivalTime[end[train2].second].minute / 1440
                                         + trains[train2].departureTime[stationIndex2].minute / 1440;
                journey2.endTime = trains[train2].arrivalTime[end[train2].second];
                journey2.endDate.day = day2;
                journey2.price = trains[train2].prefixPriceSum[end[train2].second]
                                 - trains[train2].prefixPriceSum[stationIndex2];
                TrainTicketCount ticketCount2 = ticketData_.Get(trains[train2].ticketData);
                int index2 = journey2.startDate.day - trains[train2].departureTime[stationIndex2].minute / 1440;
                journey2.seat = ticketCount2.remained[index2][stationIndex2];
                for (int k = stationIndex2 + 1; k < end[train2].second; ++k) {
                    journey2.seat = std::min(journey2.seat, ticketCount2.remained[index2][k]);
                }

            }
        }
    }
    delete[] stations2;

    if (Found) {
        std::cout << "[" << input.TimeStamp() << "] " << journey1 << ENDL
                  << journey2 << ENDL;
    } else {
        std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
    }
}
