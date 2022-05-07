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

#ifndef TICKET_SYSTEM_INCLUDE_EXCEPTIONS_H
#define TICKET_SYSTEM_INCLUDE_EXCEPTIONS_H

#include <cstddef>
#include <cstring>
#include <string>

class Exception {
protected:
	const char* description;
public:
	explicit Exception(const char* description) : description(description) {}
	Exception(const Exception &ec) = default;
	virtual const char* what() noexcept {
		return description;
	}
};

class OutOfBound : public Exception {
public:
    explicit OutOfBound(const char* description = "OutOfBound") : Exception(description) {}
};

class RuntimeError : public Exception {
public:
    explicit RuntimeError(const char* description = "RuntimeError") : Exception(description) {}
};

class InvalidIterator : public Exception {
public:
    explicit InvalidIterator(const char* description = "InvalidIterator") : Exception(description) {}
};

class EmptyContainer : public Exception {
public:
    explicit EmptyContainer(const char* description = "EmptyContainer") : Exception(description) {}
};

#endif // TICKET_SYSTEM_INCLUDE_EXCEPTIONS_H
