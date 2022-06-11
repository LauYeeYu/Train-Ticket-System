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

#include <fstream>

template<class T>
class TileStorage {
public:
    using Ptr = long;

    struct Node {
        T    value;
        long timeStamp;
        Ptr  previous = -1;
    };

    /**
     * Binding the class with a certain file.  If the file is not empty, the
     * position of deleted nodes should be read from the very beginning of
     * the file.
     */
    TileStorage(const char* fileName);

    /**
     * Binding the class with a certain file.  If the file is not empty, the
     * position of deleted nodes should be read from the very beginning of
     * the file.
     */
    TileStorage(const std::string& fileName);

    /**
     * Set the data at very beginning to be deletedNodes_;
     */
    ~TileStorage();

    /**
     * Add a new value with a time stamp.
     * @param value
     * @param timeStamp
     * @return the file position in this class.
     */
    Ptr Add(const T& value, long timeStamp);

    /**
     * Get the value at the postion.
     * @return the data at the position
     */
    T Get(Ptr position);

    /**
     * Get the value at the postion.
     * @return the data at the position
     */
    Node GetNode(Ptr position);

    /**
     * Modify the data at the position with the newValue and the time stamp.
     * @return the postion of the new value
     */
    Ptr Modify(Ptr position, const T& newValue, long timeStamp);

    /**
     * Roll back the data at the position to a certain time stamp.  The node belongs to the ``future'' can be deleted
     * @return the postion of the rolled backed node
     */
    Ptr RollBack(Ptr position, long timeStamp);

    void Clear();

private:
    template<class Type>
    Type Read_(Ptr position);

    /**
     * Read the node from the file.
     * @param position
     * @return the node at the position
     */
    Node ReadNode_(Ptr position);

    /**
     * Read the ptr data from the file.  Seemed to be dedicated for the deletedNodes_.
     * @param position
     * @return the ptr data
     */
    Ptr ReadPtr_(Ptr position);

    /**
     * Allocate a space at the end of the file.
     */
    Ptr New_(long size);

    /**
     * Store the value at the given position.
     */
    template<class Type>
    void Write_(Ptr position, const Type& value);

    // the empty nodes to be recycled for new nodes (with a single linked list)
    Ptr deletedNodes_ = -1;

    std::fstream file_;
};

#endif // TICKET_SYSTEM_INCLUDE_TILE_STORAGE_H
