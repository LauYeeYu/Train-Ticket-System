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
#include "utility.h"
#include "vector.h"

bool CanBuyTicket(const TrainTicketCount& ticketCount, int day, int from, int to, int count) {
    for (int i = from; i < to; ++i) {
#ifdef ROLLBACK
        if (ticketCount.remained[i] < count) {
#else
        if (ticketCount.remained[day][i] < count) {
#endif // ROLLBACK
            return false;
        }
    }
    return true;
}

void TrainManage::Add(ParameterTable& input) {
    Train train;
    train.trainID = input['i'];
    if (trainIndex_.Contains(ToHashPair(train.trainID))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Add failed: train "
                  << train.trainID << " already exists." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
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

    train.type = input['y'][0];

    long position = trainData_.Add(train);
#ifdef ROLLBACK
    trainIndex_.Insert(ToHashPair(train.trainID), position, input.TimeStamp());
#else
    trainIndex_.Insert(ToHashPair(train.trainID), position);
#endif // ROLLBACK

#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] Train " << train.trainID
              << " added successfully." << std::endl;
#else
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
}

void TrainManage::Delete(ParameterTable& input) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Delete failed: train " << input['i']
                  << " does not exist." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (train.released) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Delete failed: train "
                  << train.trainID
                  << " has been released. Released Train cannot be deleted!"
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
#ifdef ROLLBACK
    trainData_.Delete(position, input.TimeStamp());
    trainIndex_.Erase(ToHashPair(input['i']), input.TimeStamp());
#else
    trainData_.Delete(position);
    trainIndex_.Erase(ToHashPair(input['i']));
#endif // ROLLBACK
#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] Train " << train.trainID
              << " has been deleted successfully." << std::endl;
#else
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
}

void TrainManage::Release(ParameterTable& input) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Release failed: train "
                  << input['i'] << " does not exist." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (train.released) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Release failed: train "
                  << train.trainID
                  << " has been released. There no need to release again."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    train.released = true;

    TrainTicketCount ticketCount;
#ifdef ROLLBACK
    train.ticketData = ticketData_.Add(ticketCount);
    for (int i = 1; i < train.startDate.day; ++i) {
        ticketData_.Add(ticketCount);
    }
    for (int j = 1; j < train.stationNum; ++j) {
        ticketCount.remained[j] = train.seatNum;
    }
    for (int i = train.startDate.day; i <= train.endDate.day; ++i) {
        ticketData_.Add(ticketCount);
    }
    for (int i = train.endDate.day + 1; i < 98; ++i) {
        ticketData_.Add(ticketCount);
    }
    for (int i = 1; i < 100; ++i) {
        ticketCount.remained[i] = ticketCount.remained[i] = -1;
    }
    ticketData_.Add(ticketCount); // 98
    ticketData_.Add(ticketCount); // 99
#else
    for (int i = train.startDate.day; i <= train.endDate.day; ++i) {
        for (int j = 1; j < train.stationNum; ++j) {
            ticketCount.remained[i][j] = train.seatNum;
        }
    }
    for (int i = 1; i < 100; ++i) {
        ticketCount.remained[98][i] = ticketCount.remained[99][i] = -1;
    }

    train.ticketData = ticketData_.Add(ticketCount);
#endif // ROLLBACK

    for (int i = 1; i <= train.stationNum; ++i) {
#ifdef ROLLBACK
        stationIndex_.Insert(ToHashPair(train.stations[i]),
                                        Pair<long, long>(position, i),
                             input.TimeStamp());
#else
        stationIndex_.Insert(ToHashPair(train.stations[i]), Pair<long, long>(position, i));
#endif // ROLLBACK
    }

#ifdef ROLLBACK
    trainData_.Modify(position, train, input.TimeStamp());
#else
    trainData_.Modify(position, train);
#endif // ROLLBACK

#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] Train " << train.trainID
              << " has been released successfully." << std::endl;
#else
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
}

void TrainManage::QueryTrain(ParameterTable& input) {
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Query failed: train "
                  << input['i'] << " does not exist." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
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
#ifdef ROLLBACK
        TrainTicketCount ticketCount = ticketData_.Get(train.ticketData
            + sizeof(TrainTicketCount) * day);
#else
        TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
#endif // ROLLBACK
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] ID: " << train.trainID
                  << " type: " << train.type << " total " << train.stationNum
                  << "stations." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] " << train.trainID << " " << train.type << ENDL;
#endif // PRETTY_PRINT

        std::cout << train.stations[1] << " xx-xx xx:xx -> "
                  << date + train.departureTime[1].minute / 1440 << " "
                  << train.departureTime[1] << " "
                  << train.prefixPriceSum[1] << " "
#ifdef ROLLBACK
                  << ticketCount.remained[1]
#else
                  << ticketCount.remained[day][1]
#endif // ROLLBACK
                  << ENDL;
        for (int i = 2; i < train.stationNum; ++i) {
            std::cout << train.stations[i] << " "
                      << date + train.arrivalTime[i].minute / 1440 << " "
                      << train.arrivalTime[i] << " -> "
                      << date + train.departureTime[i].minute / 1440 << " "
                      << train.departureTime[i] << " "
                      << train.prefixPriceSum[i] << " "
#ifdef ROLLBACK
                      << ticketCount.remained[i]
#else
                      << ticketCount.remained[day][i]
#endif // ROLLBACK
                      << ENDL;
        }
        std::cout << train.stations[train.stationNum] << " "
                  << date + train.arrivalTime[train.stationNum].minute / 1440 << " "
                  << train.arrivalTime[train.stationNum] << " -> xx-xx xx:xx "
                  << train.prefixPriceSum[train.stationNum] << " x" << ENDL;
    } else {
        std::cout << "[" << input.TimeStamp() << "] " << train.trainID << " " << train.type << ENDL;

        std::cout << train.stations[1] << " xx-xx xx:xx -> "
                  << date + train.departureTime[1].minute / 1440 << " "
                  << train.departureTime[1] << " "
                  << train.prefixPriceSum[1] << " " << train.seatNum << ENDL;
        for (int i = 2; i < train.stationNum; ++i) {
            std::cout << train.stations[i] << " "
                      << date + train.arrivalTime[i].minute / 1440 << " "
                      << train.arrivalTime[i] << " -> "
                      << date + train.departureTime[i].minute / 1440 << " "
                      << train.departureTime[i] << " "
                      << train.prefixPriceSum[i] << " " << train.seatNum << ENDL;
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
#ifdef ROLLBACK
            TrainTicketCount ticketCount = ticketData_.Get(train.ticketData
                + sizeof(TrainTicketCount) * tmpDate);
            int ticketNum = ticketCount.remained[ticketIndex[i.first]];
            for (int j = ticketIndex[i.first] + 1; j < i.second; ++j) {
                ticketNum = std::min(ticketNum, ticketCount.remained[j]);
            }
#else
            TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
            int ticketNum = ticketCount.remained[tmpDate][ticketIndex[i.first]];
            for (int j = ticketIndex[i.first] + 1; j < i.second; ++j) {
                ticketNum = std::min(ticketNum, ticketCount.remained[tmpDate][j]);
            }
#endif // ROLLBACK
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
    if (input['p'].empty() || input['p'][0] == 't') {
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
#ifdef ROLLBACK
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: user hasn't logged in yet." << ENDL;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // ROLLBACK
        return;
    }
    if (!trainIndex_.Contains(ToHashPair(input['i']))) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: train doesn't exist." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    long position = trainIndex_.Find();
    Train train = trainData_.Get(position);
    if (!train.released) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: train hasn't been released." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    int n = StringToInt(input['n']);
    if (n > train.seatNum) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: the required seat number exceeds the total seat number."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
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
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: the departure station doesn't exist." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    for (int i = departure + 1; i <= train.stationNum; ++i) {
        if (train.stations[i] == input['t']) {
            arrival = i;
        }
    }
    if (arrival == 0) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: the arrival station doesn't exist." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    // stuff of the date
    Date date(input['d']);
    Date trainDate = date - train.departureTime[departure].minute / 1440;

    if (trainDate < train.startDate || trainDate > train.endDate) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp()
                  << "] Buy failed: the train doesn't run on the required date."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    // Now there exist a train that can serve the request
#ifdef ROLLBACK
    TrainTicketCount ticketCount = ticketData_.Get(train.ticketData
        + sizeof(TrainTicketCount) * trainDate.day);
    int ticketNum = ticketCount.remained[departure];
    for (int i = departure + 1; i < arrival; ++i) {
        ticketNum = std::min(ticketNum, ticketCount.remained[i]);
    }
#else
    TrainTicketCount ticketCount = ticketData_.Get(train.ticketData);
    int ticketNum = ticketCount.remained[trainDate.day][departure];
    for (int i = departure + 1; i < arrival; ++i) {
        ticketNum = std::min(ticketNum, ticketCount.remained[trainDate.day][i]);
    }
#endif // ROLLBACK

    // the process of purchasing
    if (ticketNum >= n) { // Enough ticket(s)
#ifdef ROLLBACK
        for (int i = departure; i < arrival; ++i) {
            ticketCount.remained[i] -= n;
        }
        ticketData_.Modify(train.ticketData + sizeof(TrainTicketCount) * trainDate.day,
                           ticketCount,
                           input.TimeStamp());
#else
        for (int i = departure; i < arrival; ++i) {
            ticketCount.remained[trainDate.day][i] -= n;
        }
        ticketData_.Modify(train.ticketData, ticketCount);
#endif // ROLLBACK
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
        ticket.state = TicketState::bought;
        ticket.seatNum = n;
        userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Price: " << ticket.price * n << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] " << ticket.price * n << ENDL;
#endif // PRETTY_PRINT
    } else {
        if (input['q'].empty() || input['q'][0] == 'f') {
#ifdef PRETTY_PRINT
            std::cout << "[" << input.TimeStamp()
                      << "] Buy failed: the required seat number exceeds the number of available seats."
                      << std::endl;
#else
            std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
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
            ticket.state = TicketState::pending;
            ticket.seatNum = n;
#ifdef ROLLBACK
            ticketCount = ticketData_.Get(train.ticketData
                + sizeof(TrainTicketCount) * 99);
            if (ticketCount.remained[trainDate.day] != -1) {
                long lastQueuePtr = ticketCount.remained[trainDate.day];
                Ticket lastQueue = userTicketData_.Get(lastQueuePtr);
                lastQueue.queue = userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
                userTicketData_.Modify(lastQueuePtr, lastQueue, input.TimeStamp());
                ticketCount.remained[trainDate.day] = lastQueue.queue;
                ticketData_.Modify(train.ticketData + sizeof(TrainTicketCount) * 99,
                                   ticketCount,
                                   input.TimeStamp());
            } else {
                int ptr = userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
                ticketCount.remained[trainDate.day] = ptr;
                ticketData_.Modify(train.ticketData + sizeof(TrainTicketCount) * 99,
                                   ticketCount,
                                   input.TimeStamp());
                ticketCount = ticketData_.Get(train.ticketData
                                              + sizeof(TrainTicketCount) * 98);
                ticketCount.remained[trainDate.day] = ptr;
                ticketData_.Modify(train.ticketData + sizeof(TrainTicketCount) * 98,
                                   ticketCount,
                                   input.TimeStamp());
            }
#else
            if (ticketCount.remained[99][trainDate.day] != -1) {
                long lastQueuePtr = ticketCount.remained[99][trainDate.day];
                Ticket lastQueue = userTicketData_.Get(lastQueuePtr);
                lastQueue.queue = userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
                userTicketData_.Modify(lastQueuePtr, lastQueue);
                ticketCount.remained[99][trainDate.day] = lastQueue.queue;
            } else {
                ticketCount.remained[99][trainDate.day]
                    = userManage.AddOrder(input['u'], ticket, input.TimeStamp(), *this);
                ticketCount.remained[98][trainDate.day] = ticketCount.remained[99][trainDate.day];
            }
            ticketData_.Modify(train.ticketData, ticketCount);
#endif // ROLLBACK
#ifdef PRETTY_PRINT
            std::cout << "[" << input.TimeStamp() << "] You are in the pending queue." << ENDL;
#else
            std::cout << "[" << input.TimeStamp() << "] queue" << ENDL;
#endif // PRETTY_PRINT
        }
    }

}

long TrainManage::AddOrder(Ticket& ticket, long timeStamp) {
    return userTicketData_.Add(ticket);
}

void TrainManage::QueryOrder(ParameterTable& input, UserManage& userManage) {
    if (!userManage.Logged(input['u'])) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Query failed: the user hasn't logged in yet."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
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
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Refund failed: the user hasn't logged in yet."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }

    // Get the pointer to the order
    long orderPtr = userManage.GetUser(input['u']).orderInfo;
    if (orderPtr == -1) {
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Refund failed: no such order."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    int number;
    if (input['n'].empty()) number = 0;
    else number = StringToInt(input['n']) - 1;
    Ticket ticket;
    ticket = userTicketData_.Get(orderPtr);
    while (number > 0) {
        orderPtr = ticket.last;
        if (orderPtr == -1) {
#ifdef PRETTY_PRINT
            std::cout << "[" << input.TimeStamp() << "] Refund failed: no such order."
                      << std::endl;
#else
            std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
            return;
        }
        ticket = userTicketData_.Get(orderPtr);
        --number;
    }

    // Refund the ticket
    if (ticket.state == TicketState::refunded) { // has already refunded
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Refund failed: the ticket has already been refunded."
                  << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] -1" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    if (ticket.state == TicketState::pending) { // in the pending list, not need to modify the train data
        ticket.state = TicketState::refunded;
#ifdef ROLLBACK
        userTicketData_.Modify(orderPtr, ticket, input.TimeStamp());
#else
        userTicketData_.Modify(orderPtr, ticket);
#endif // ROLLBACK
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Refund successfully." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT

        return;
    }
    ticket.state = TicketState::refunded;
#ifdef ROLLBACK
    userTicketData_.Modify(orderPtr, ticket, input.TimeStamp());
    TrainTicketCount ticketCount = ticketData_.Get(ticket.ticketPosition
        + sizeof(TrainTicketCount) * ticket.index);
    for (int i = ticket.from; i < ticket.to; ++i) {
        ticketCount.remained[i] += ticket.seatNum;
    }
#else
    userTicketData_.Modify(orderPtr, ticket);
    TrainTicketCount ticketCount = ticketData_.Get(ticket.ticketPosition);
    for (int i = ticket.from; i < ticket.to; ++i) {
        ticketCount.remained[ticket.index][i] += ticket.seatNum;
    }
#endif // ROLLBACK

    // check the pending queue
#ifdef ROLLBACK
    TrainTicketCount head = ticketData_.Get(ticket.ticketPosition
        + sizeof(TrainTicketCount) * 98);
    long queuePtr = head.remained[ticket.index];
    TrainTicketCount tail = ticketData_.Get(ticket.ticketPosition
        + sizeof(TrainTicketCount) * 99);
#else
    long queuePtr = ticketCount.remained[98][ticket.index];
#endif // ROLLBACK
    long nextPtr;
    // Nothing to pend
    if (queuePtr == -1) {
#ifdef ROLLBACK
        ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * ticket.index,
                           ticketCount,
                           input.TimeStamp());
#else
        ticketData_.Modify(ticket.ticketPosition, ticketCount);
#endif // ROLLBACK
#ifdef PRETTY_PRINT
        std::cout << "[" << input.TimeStamp() << "] Refund successfully." << std::endl;
#else
        std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
        return;
    }
    ticket = userTicketData_.Get(queuePtr);

    // the case that the head nodes can be served
    while (ticket.state == TicketState::pending &&
           CanBuyTicket(ticketCount, ticket.index, ticket.from, ticket.to, ticket.seatNum)) {
        ticket.state = TicketState::bought;
        nextPtr = ticket.queue;
        ticket.queue = -1;
#ifdef ROLLBACK
        userTicketData_.Modify(queuePtr, ticket, input.TimeStamp());
        for (int i = ticket.from; i < ticket.to; ++i) {
            ticketCount.remained[i] -= ticket.seatNum;
        }
#else
        userTicketData_.Modify(queuePtr, ticket);
        for (int i = ticket.from; i < ticket.to; ++i) {
            ticketCount.remained[ticket.index][i] -= ticket.seatNum;
        }
#endif // ROLLBACK
        queuePtr = nextPtr;
        if (queuePtr == -1) {
#ifdef ROLLBACK
            head.remained[ticket.index] = -1;
            tail.remained[ticket.index] = -1;
            ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * ticket.index,
                               ticketCount,
                               input.TimeStamp());
            ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * 98,
                               head,
                               input.TimeStamp());
            ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * 99,
                               tail,
                               input.TimeStamp());
#else
            ticketCount.remained[98][ticket.index] = -1;
            ticketCount.remained[99][ticket.index] = -1;
            ticketData_.Modify(ticket.ticketPosition, ticketCount);
#endif // ROLLBACK
#ifdef PRETTY_PRINT
            std::cout << "[" << input.TimeStamp() << "] Refund successfully." << std::endl;
#else
            std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
            return;
        }
        ticket = userTicketData_.Get(queuePtr);
    }
#ifdef ROLLBACK
    head.remained[ticket.index] = queuePtr;
#else
    ticketCount.remained[98][ticket.index] = queuePtr;
#endif // ROLLBACK

    // this node cannot be served, so we need to move to the next node
    long lastPtr = queuePtr;
    queuePtr = ticket.queue;

    while (queuePtr != -1) {
        ticket = userTicketData_.Get(queuePtr);
        if (ticket.state == TicketState::pending &&
            CanBuyTicket(ticketCount, ticket.index, ticket.from, ticket.to, ticket.seatNum)) {
            ticket.state = TicketState::bought;
            nextPtr = ticket.queue;
            ticket.queue = -1;
#ifdef ROLLBACK
            userTicketData_.Modify(queuePtr, ticket, input.TimeStamp());
#else
            userTicketData_.Modify(queuePtr, ticket);
#endif // ROLLBACK
            for (int i = ticket.from; i < ticket.to; ++i) {
#ifdef ROLLBACK
                ticketCount.remained[i] -= ticket.seatNum;
#else
                ticketCount.remained[ticket.index][i] -= ticket.seatNum;
#endif // ROLLBACK
            }
            queuePtr = nextPtr;
        } else {
            Ticket lastTicket = userTicketData_.Get(lastPtr);
            lastTicket.queue = queuePtr;
#ifdef ROLLBACK
            userTicketData_.Modify(lastPtr, lastTicket, input.TimeStamp());
#else
            userTicketData_.Modify(lastPtr, lastTicket);
#endif // ROLLBACK
            lastPtr = queuePtr;
            queuePtr = ticket.queue;
        }
    }

#ifdef ROLLBACK
    tail.remained[ticket.index] = lastPtr;
    ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * ticket.index,
                       ticketCount,
                       input.TimeStamp());
    ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * 98,
                       head,
                       input.TimeStamp());
    ticketData_.Modify(ticket.ticketPosition + sizeof(TrainTicketCount) * 99,
                       tail,
                       input.TimeStamp());
#else
    ticketCount.remained[99][ticket.index] = lastPtr;
    ticketData_.Modify(ticket.ticketPosition, ticketCount);
#endif // ROLLBACK
#ifdef PRETTY_PRINT
    std::cout << "[" << input.TimeStamp() << "] Refund successfully." << std::endl;
#else
    std::cout << "[" << input.TimeStamp() << "] 0" << ENDL;
#endif // PRETTY_PRINT
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

    for (auto& startPtr : start) {
        Train train1 = trainData_.Get(startPtr.first);
        int tmpDate = date.day - train1.departureTime[startPtr.second].minute / 1440;
        if (tmpDate < train1.startDate.day || tmpDate > train1.endDate.day) continue;
        int startDate = date.day - train1.departureTime[startPtr.second].minute / 1440;
#ifdef ROLLBACK
        TrainTicketCount ticketCount1 = ticketData_.Get(train1.ticketData + sizeof(TrainTicketCount) * startDate);
#else
        TrainTicketCount ticketCount1 = ticketData_.Get(train1.ticketData);
#endif // ROLLBACK

        for (int j = startPtr.second + 1; j <= train1.stationNum; ++j) {
            stationHash[j] = ToHashPair(train1.stations[j]);
        }
        for (int train2 = 0; train2 < end.Size(); ++train2) {
            if (end[train2].first == startPtr.first) continue; // eliminate the same train
#ifdef ROLLBACK
            int remained1 = ticketCount1.remained[startPtr.second];
#else
            int remained1 = ticketCount1.remained[startDate][startPtr.second];
#endif // ROLLBACK
            for (int j = startPtr.second + 1; j <= train1.stationNum; ++j) {
#ifdef ROLLBACK
                remained1 = std::min(remained1, ticketCount1.remained[j - 1]);
#else
                remained1 = std::min(remained1, ticketCount1.remained[startDate][j - 1]);
#endif // ROLLBACK
                if (!stations2[train2].Contains(stationHash[j])) continue;
                int stationIndex2 = stations2[train2][stationHash[j]];
                int arrivalDay1 = date.day - train1.departureTime[startPtr.second].minute / 1440
                                  + train1.arrivalTime[j].minute / 1440;
                // eliminate the wrong date
                if (arrivalDay1 > trains[train2].endDate.day
                                  + trains[train2].departureTime[stationIndex2].minute / 1440) continue;
                if (arrivalDay1 == trains[train2].endDate.day
                                   + trains[train2].departureTime[stationIndex2].minute / 1440 &&
                    train1.arrivalTime[j].minute % 1440 >
                    trains[train2].departureTime[stationIndex2].minute % 1440) {
                    continue;
                }
                int tmpCost, tmpTime, arrivalDay2;
                if (rule) {
                    if (arrivalDay1 < trains[train2].startDate.day
                                      + trains[train2].departureTime[stationIndex2].minute / 1440) {
                        arrivalDay2 = trains[train2].startDate.day
                                      + trains[train2].arrivalTime[end[train2].second].minute / 1440;
                    } else if (train1.arrivalTime[j].minute % 1440 >
                               trains[train2].departureTime[stationIndex2].minute % 1440) {
                        arrivalDay2 = arrivalDay1 + 1
                                      + trains[train2].arrivalTime[end[train2].second].minute / 1440
                                      - trains[train2].departureTime[stationIndex2].minute / 1440;
                    } else {
                        arrivalDay2 = arrivalDay1
                                      + trains[train2].arrivalTime[end[train2].second].minute / 1440
                                      - trains[train2].departureTime[stationIndex2].minute / 1440;
                    }
                    int dayCount = arrivalDay2 - date.day;
                    tmpTime = dayCount * 1440 - train1.departureTime[startPtr.second].minute % 1440
                              + trains[train2].arrivalTime[end[train2].second].minute % 1440;
                    if (Found && tmpTime > time) continue;
                    tmpCost = train1.prefixPriceSum[j] - train1.prefixPriceSum[startPtr.second]
                              + trains[train2].prefixPriceSum[end[train2].second]
                              - trains[train2].prefixPriceSum[stationIndex2];
                    if (Found && tmpTime == time) {
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
                    if (arrivalDay1 < trains[train2].startDate.day
                                      + trains[train2].departureTime[stationIndex2].minute / 1440) {
                        arrivalDay2 = trains[train2].startDate.day
                                      + trains[train2].arrivalTime[end[train2].second].minute / 1440;
                    } else if (train1.arrivalTime[j].minute % 1440 >
                               trains[train2].departureTime[stationIndex2].minute % 1440) {
                        arrivalDay2 = arrivalDay1 + 1
                                      + trains[train2].arrivalTime[end[train2].second].minute / 1440
                                      - trains[train2].departureTime[stationIndex2].minute / 1440;
                    } else {
                        arrivalDay2 = arrivalDay1
                                      + trains[train2].arrivalTime[end[train2].second].minute / 1440
                                      - trains[train2].departureTime[stationIndex2].minute / 1440;
                    }
                    int dayCount = arrivalDay2 - date.day;
                    tmpTime = dayCount * 1440 - train1.departureTime[startPtr.second].minute % 1440
                              + trains[train2].arrivalTime[end[train2].second].minute % 1440;
                    if (Found && tmpCost == cost) {
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
                journey1.startDate.day = date.day;
                journey1.endTime = train1.arrivalTime[j];
                journey1.endDate.day = arrivalDay1;
                journey1.price = train1.prefixPriceSum[j] - train1.prefixPriceSum[startPtr.second];
                journey1.seat = remained1;

                journey2.trainID = trains[train2].trainID;
                journey2.startStation = trains[train2].stations[stationIndex2];
                journey2.endStation = trains[train2].stations[end[train2].second];
                journey2.startTime = trains[train2].departureTime[stationIndex2];
                journey2.startDate.day = arrivalDay2 - trains[train2].arrivalTime[end[train2].second].minute / 1440
                                         + trains[train2].departureTime[stationIndex2].minute / 1440;
                journey2.endTime = trains[train2].arrivalTime[end[train2].second];
                journey2.endDate.day = arrivalDay2;
                journey2.price = trains[train2].prefixPriceSum[end[train2].second]
                                 - trains[train2].prefixPriceSum[stationIndex2];
                int index2 = journey2.startDate.day - trains[train2].departureTime[stationIndex2].minute / 1440;
#ifdef ROLLBACK
                TrainTicketCount ticketCount2 = ticketData_.Get(trains[train2].ticketData
                    + sizeof(TrainTicketCount) * index2);
                journey2.seat = ticketCount2.remained[stationIndex2];
                for (int k = stationIndex2 + 1; k < end[train2].second; ++k) {
                    journey2.seat = std::min(journey2.seat, ticketCount2.remained[k]);
                }
#else
                TrainTicketCount ticketCount2 = ticketData_.Get(trains[train2].ticketData);
                journey2.seat = ticketCount2.remained[index2][stationIndex2];
                for (int k = stationIndex2 + 1; k < end[train2].second; ++k) {
                    journey2.seat = std::min(journey2.seat, ticketCount2.remained[index2][k]);
                }
#endif // ROLLBACK

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

#ifdef ROLLBACK
void TrainManage::RollBack(long timeStamp) {
    trainIndex_.RollBack(timeStamp);
    trainData_.RollBack(timeStamp);
    ticketData_.RollBack(timeStamp);
    stationIndex_.RollBack(timeStamp);
    userTicketData_.RollBack(timeStamp);
}
#endif // ROLLBACK
