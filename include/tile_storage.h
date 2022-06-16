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

#ifndef TICKET_SYSTEM_INCLUDE_TILE_STORAGE_H
#define TICKET_SYSTEM_INCLUDE_TILE_STORAGE_H

#include <cstring>
#include <fstream>

template<class T>
class TileStorage {
public:
    using Ptr = long;

    /**
     * Binding the class with a certain file.  If the file is not empty, the
     * position of deleted nodes should be read from the very beginning of
     * the file.
     */
#ifdef ROLLBACK
    explicit TileStorage(const char* fileName, const char* logFileName)
        : memoryManager_(fileName, logFileName, newFile_) {}
#else
    explicit TileStorage(const char* fileName) : memoryManager_(fileName, newFile_) {}
#endif

    /**
     * Set the data at very beginning to be deletedNodes_;
     */
    ~TileStorage() = default;

    /**
     * Add a new value with a time stamp.
     * @param value
     * @param timeStamp
     * @return the file position in this class.
     */
    Ptr Add(const T& value) {
        char* data = memoryManager_.AddNode();
        memcpy(data, &value, sizeof(T));
        return memoryManager_.Last;
    }

#ifdef ROLLBACK
    void Delete(Ptr pos, long timeStamp) {
        memoryManager_.ReadNode(pos, timeStamp);
        memoryManager_.DelNode(pos);
    }
#else
    void Delete(Ptr pos) {
        memoryManager_.DelNode(pos);
    }
#endif

    /**
     * Get the value at the position.
     * @return the data at the position
     */
#ifdef ROLLBACK
    const T& Get(Ptr position) {
        char* data = memoryManager_.ReadNode(position, -1);
        return *(reinterpret_cast<const T*>(data));
    }
#else
    const T& Get(Ptr position) {
        char* data = memoryManager_.ReadNode(position);
        return *(reinterpret_cast<const T*>(data));
    }
#endif

    /**
     * Modify the data at the position with the newValue and the time stamp.
     * @return the position of the new value
     */
#ifdef ROLLBACK
    void Modify(Ptr position, const T& newValue, long timeStamp) {
        char* data = memoryManager_.ReadNode(position, timeStamp);
        memcpy(data, &newValue, sizeof(T));
    }
#else
    void Modify(Ptr position, const T& newValue) {
        char* data = memoryManager_.ReadNode(position);
        memcpy(data, &newValue, sizeof(T));
    }
#endif

    void Clear() {
        memoryManager_.Clear();
    }

#ifdef ROLLBACK
    void RollBack(long timeStamp) {
        memoryManager_.RollBack(timeStamp);
    }
#endif

private:
    MemoryManager<sizeof(T)> memoryManager_;
    bool newFile_;
};

#endif // TICKET_SYSTEM_INCLUDE_TILE_STORAGE_H
