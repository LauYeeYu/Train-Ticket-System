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
    train.prefixPriceSum[0] = 0;
    for (int i = 1; i <= train.stationNum; ++i) {
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
