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

#include "token_scanner.h"

#include <iostream>

std::string TokenScanner::NextToken() noexcept {
    if (mode_ == multiple) {
        // Skip delimiter
        while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
        int start = current_;

        // Find another delimiter
        while (current_ < buffer_.size() && buffer_[current_] != delimiter_) ++current_;
        return buffer_.substr(start, current_ - start);
    } else { // mode_ == single
        if (current_ >= buffer_.size()) return std::string();

        int start = current_;

        // Find another delimiter
        while (current_ < buffer_.size() && buffer_[current_] != delimiter_) ++current_;
        ++current_;
        return buffer_.substr(start, current_ - start - 1);
    }
}

std::string TokenScanner::PeekNextToken() noexcept {
    if (mode_ == multiple) {
        // Skip delimiter
        while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
        int end = current_;

        // Find another delimiter
        while (end < buffer_.size() && buffer_[end] != delimiter_) ++end;
        return buffer_.substr(current_, end - current_);
    } else { // mode_ == single
        if (current_ >= buffer_.size()) return std::string();

        int end = current_;

        // Find another delimiter
        while (end < buffer_.size() && buffer_[end] != delimiter_) ++end;
        return buffer_.substr(current_, end - current_);
    }
}

bool TokenScanner::HasMoreToken() noexcept {
    if (mode_ == multiple) {
        while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
    }
    return current_ < buffer_.size();
}

TokenScanner& TokenScanner::NewLine() {
    current_ = 0;
    std::getline(std::cin, buffer_);
    return *this;
}

SizeT TokenScanner::TotalLength() const noexcept {
    return buffer_.length();
}

TokenScanner& TokenScanner::ChangeMode(TokenScannerMode mode) noexcept {
    mode_ = mode;
    return *this;
}

TokenScanner& TokenScanner::ResetState() noexcept {
    current_ = 0;
    return *this;
}

TokenScanner& TokenScanner::Read(std::string newInput) noexcept {
    buffer_ = std::move(newInput);
    current_ = 0;
    return *this;
}

TokenScanner& TokenScanner::SkipDelimiter() noexcept {
    while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
    return *this;
}

TokenScanner& TokenScanner::SetDelimiter(char delimiter) noexcept {
    delimiter_ = delimiter;
    return *this;
}

const std::string& TokenScanner::GetInputString() const noexcept {
    return buffer_;
}

char TokenScanner::GetDelimiter() const noexcept {
    return delimiter_;
}

TokenScanner::TokenScannerMode TokenScanner::GetMode() const noexcept {
    return mode_;
}

TokenScanner& TokenScanner::operator=(const TokenScanner& obj) = default;

TokenScanner& TokenScanner::operator=(TokenScanner&& obj) noexcept = default;
