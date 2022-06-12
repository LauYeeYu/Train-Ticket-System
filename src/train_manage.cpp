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

#include "token_scanner.h"
#include "train.h"
#include "utility"

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
    ticketCount.trainDataPtr = position;

    long ticketPosition = ticketData_.Add(ticketCount);
    train.ticketData = ticketPosition;
    for (int i = 1; i <= train.stationNum; ++i) {
        stationIndex_.Insert(ToHashPair(train.stations[i]), Pair<long, long>(ticketPosition, i));
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
