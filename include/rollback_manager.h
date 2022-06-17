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

#ifndef TICKET_SYSTEM_INCLUDE_ROLLBACK_MANAGER_H
#define TICKET_SYSTEM_INCLUDE_ROLLBACK_MANAGER_H

#include <fstream>
#include <cstring>

#ifdef ROLLBACK

template<int kBlockSize>
class RollBackManager {
    using Ptr = long;

private:
    std::fstream file;
    
    Ptr lastPos;
    struct RollBackNode {
        char info[kBlockSize];
        Ptr pre;
        Ptr pos;
        long timeStamp;
        RollBackNode(Ptr _pre = -1, Ptr _pos = -1, long _timeStamp = 0):
            pre(_pre), pos(_pos), timeStamp(_timeStamp) {}
    };

    void InitMeta() {
        file.seekp(0, std::ios::end);
        if (file.tellp() == 0) {
            lastPos = -1;
            file.write((char*)&lastPos, sizeof(lastPos));
        } else {
            file.seekg(0);
            file.read((char*)&lastPos, sizeof(lastPos));
        }
    }

public:
    RollBackManager(const char* filename)
        : file(filename, std::ios::in | std::ios::out | std::ios::binary) {
        InitMeta();
    }

    ~RollBackManager() {
        file.seekp(0);
        file.write((char*)&lastPos, sizeof(lastPos));
    }

    void Insert(char* info, Ptr pos, long timeStamp) {
        RollBackNode cur(lastPos, pos, timeStamp);
        memcpy(cur.info, info, kBlockSize);
        if (lastPos == -1) {
            file.seekp(sizeof(Ptr));
        } else {
            file.seekp(lastPos + sizeof(RollBackNode));
        }
        file.seekp(0, std::ios::end);
        lastPos = file.tellp();
        file.write((char*)&cur, sizeof(cur));
    }

    void RollBack(std::fstream &file2, long timeStamp) {
        RollBackNode cur;
        while (lastPos != -1) {
            file.seekg(lastPos);
            file.read((char*)&cur, sizeof(cur));
            if (cur.timeStamp < timeStamp) { //maybe <=
                break;
            }
            file2.seekp(cur.pos);
            file2.write(cur.info, kBlockSize);
            lastPos = cur.pre;
        }
    }
};

#endif // ROLLBACK

#endif // TICKET_SYSTEM_INCLUDE_ROLLBACK_MANAGER_H
