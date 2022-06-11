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

#ifndef TICKET_SYSTEM_INCLUDE_TRAIN_H
#define TICKET_SYSTEM_INCLUDE_TRAIN_H

#include <iostream>

#include "fixed_string.h"

using TrainID = FixedString<20>;
using Station = FixedString<40>;

struct Date {
    Date() = default;
    explicit Date(int day) : day(day) {}
    explicit Date(const std::string& string);

    bool operator<(const Date& rhs) const;
    bool operator>(const Date& rhs) const;
    bool operator==(const Date& rhs) const;
    Date& operator+=(int rhs);
    Date operator+(int rhs);

    int day;
};

struct Time {
    Time() = default;
    explicit Time(int minute) : minute(minute) {}
    explicit Time(const std::string& string);
    Time(const Time&) = default;
    bool operator<(const Time& rhs) const;
    bool operator>(const Time& rhs) const;
    bool operator==(const Time& rhs) const;
    Time& operator+=(const Time& rhs);
    Time operator-(const Time& rhs);
    Time operator+(const Time& rhs);
    Time& operator+=(int rhs);
    Time operator+(int rhs);

    int minute;
};

struct Train {
    TrainID trainID;
    Station stations[101];
    long queueFirst = -1;
    long queueLast = -1;
    int  stationNum;
    int  seatNum;
    int  prefixPriceSum[101];
    Time departureTime[101];
    Time arrivalTime[101];
    Date startDate, endDate;
    char type;
    bool status = false; // Indicate whether the train is released or not
};

struct Ticket {
    long trainPosition;
    int  number;
    int  price;
    int  from, to;
    int  state; // 1 for bought, 0 for queuing, -1 for refunded
    long last = -1; // the last query
    long queue = -1; // the next queuing order
};

struct TrainTicketCount {
    int remained[100];
};

#endif // TICKET_SYSTEM_INCLUDE_TRAIN_H
