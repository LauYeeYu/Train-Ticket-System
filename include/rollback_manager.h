#ifndef ROLLBACK_MANAGER
#define ROLLBACK_MANAGER

#include <fstream>
#include <cstring>

template<int kBlockSize>
class RollBackManager {
    using Ptr = long;

private:
    std::fstream file;
    
    Ptr lastPos;
    struct RollBackNode {
        Ptr pre;
        Ptr pos;
        long timeStamp;
        char info[kBlockSize];
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
    RollBackManager(const char* filename): 
        file(filename, std::ios::in | std::ios::out | std::ios::binary) {
        InitMeta();
    }

    ~RollBackManager() {
        file.seekp(0);
        file.write((char*)&lastPos, sizeof(lastPos));
    }

    void Insert(char* info, Ptr pos, long timeStamp) {
        RollBackNode cur(lastPos, pos, timeStamp);
        memcpy(cur.info, info, kBlockSize);
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
            std::cerr << cur.timeStamp << std::endl;
            file2.seekp(cur.pos);
            file2.write(cur.info, kBlockSize);
            lastPos = cur.pre;
        }
    }
};

#endif
