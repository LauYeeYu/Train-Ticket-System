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

#include "train.h"

bool Date::operator<(const Date& rhs) const {
    return this->day < rhs.day;
}

bool Date::operator>(const Date& rhs) const {
    return this->day > rhs.day;
}

bool Date::operator==(const Date& rhs) const {
    return this->day == rhs.day;
}

Date& Date::operator+=(int rhs) {
    day += rhs;
    return *this;
}

Date Date::operator+(int rhs) {
    Date result = *this;
    result += rhs;
    return result;
}

Date::Date(const std::string& string) {
    char c = string[1];
    if (c == '6') {
        day  = (string[3] - '0') * 10 + string[4] - '0';
    } else if (c == '7') {
        day = (string[3] - '0') * 10 + string[4] - '0' + 30;
    } else if (c == '8') {
        day = (string[3] - '0') * 10 + string[4] - '0' + 61;
    } else if (c == '9') {
        day = (string[3] - '0') * 10 + string[4] - '0' + 92;
    } else {
        day = -1;
    }
}

std::ostream& operator<<(std::ostream& os, const Date& date) {
    if (date.day > 92) {
        os << "09-" << (date.day - 92) / 10 << (date.day - 92) % 10;
    } else if (date.day > 61) {
        os << "08-" << (date.day - 61) / 10 << (date.day - 61) % 10;
    } else if (date.day > 30) {
        os << "07-" << (date.day - 30) / 10 << (date.day - 30) % 10;
    } else {
        os << "06-" << date.day / 10 << date.day % 10;
    }
    return os;
}

Date& Date::operator-=(int rhs) {
    day -= rhs;
    return *this;
}

Date Date::operator-(int rhs) {
    Date result = *this;
    result -= rhs;
    return result;
}

bool Date::operator!=(const Date& rhs) const {
    return this->day != rhs.day;
}

bool Time::operator<(const Time& rhs) const {
    return this->minute < rhs.minute;
}

bool Time::operator>(const Time& rhs) const {
    return this->minute > rhs.minute;
}

bool Time::operator==(const Time& rhs) const {
    return this->minute == rhs.minute;
}

Time& Time::operator+=(const Time& rhs) {
    minute += rhs.minute;
    return *this;
}

Time Time::operator-(const Time& rhs) {
    Time result = *this;
    result.minute -= rhs.minute;
    return result;
}

Time Time::operator+(const Time& rhs) {
    Time result = *this;
    result.minute += rhs.minute;
    return result;
}

Time& Time::operator+=(int rhs) {
    minute += rhs;
    return *this;
}

Time Time::operator+(int rhs) {
    Time result = *this;
    result.minute += rhs;
    return result;
}

Time::Time(const std::string& string) {
    minute = (string[0] - '0') * 600 + (string[1] - '0') * 60
           + (string[3] - '0') * 10 + string[4] - '0';
}

std::ostream& operator<<(std::ostream& os, const Time& time) {
    os << (time.minute / 60) % 24 / 10 << (time.minute / 60) % 24 % 10
       << ":" << time.minute % 60 / 10 << time.minute % 60 % 10;
    return os;
}

bool Time::operator!=(const Time& rhs) const {
    return this->minute != rhs.minute;
}
