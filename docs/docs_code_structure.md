# Code Structure for the Train Ticket System

## In File `main.cpp`

```c++
bool Request(ParameterTable& parameterTable, UserManage& users, TrainManage& trains);

void Init();

int main();
```

## In File `fixed_string.h`

```c++
template<long size>
struct FixedString {
    char data[size + 1];

    char& operator[](long index);
    bool operator==(const FixedString& rhs);
    bool operator<(const FixedString& rhs);
};

template<long size>
struct FixedStringHash {
    std::size_t operator()(); // get the string hash
};
```

## In File `parameter_table.h`

```c++
#include <iostream>

class ParameterTable {
public:
    ParameterTable();

    ~ParameterTable();

    std::string& operator[](char c);
    const std::string& operator[](char c) const;

    const std::string& GetCommand() const;

    void ReadNewLine();
    
    long Timestamp() const;

private:
    long timeStamp_;
    std::string command_;
    std::string table_[26];
};
```

## In File `BP_tree.h`

```c++
template<class Key,
         class Value,
         class KeyCompare   = std::less<Key>,
         class ValueCompare = std::less<Value>,
         class KeyEqual     = std::equal_to<Key>,
         class ValueEqual   = std::equal_to<Value>>
class BPTree {
public:
    using Ptr = long;

    struct PairData {
        Key key;
        Value value;
        long timeStamp;
    };

    /**
     * Binding the tree with a certain file.  If the file is empty, a head
     * node for a new class is stored, and the tree is constructed.  If not,
     * the metadata of the old tree is read.
     */
    BPTree(const char* fileName);

    BPTree(const std::string& fileName);

    BPTree(const BPTree&) = delete;

    BPTree& operator=(const BPTree&) = delete;

    ~BPTree();

    /**
     * Insert a key value pair into this tree.
     * @param key
     * @param value
     * @return a bool to tell whether this operation is success or not
     */
    bool Insert(const Key& key, const Value& value, long timeStamp);

    /**
     * Move the pair with the input key to deleted zone.
     * @param key
     */
    void Erase(const Key& key);

    /**
     * Roll back to the prevoius time stamp.  If the tree have been cleared
     * after that time stamp, the bahaviour is undefined.
     *
     */
    void RollBack(long timeStamp);

    /**
     * Clear all the data in this tree. All the space is ready to be reused.
     */
    void Clear();

    /**
     * Tell whether there is a node with the input key.
     */
    bool Contains(const Key& key);

    /**
     * Find the first node with the key.
     @return the first node with the input key
     */
    Value FindFirst(const Key& key);

    /**
     * Get all the value with the input key.
     @return string with the input key
     */
    std::basic_string<Value> FindAll(const Key& key);

private:
    struct Head_ {
        long nodeSize; // the proper size making sure the block size is 4KiB
        Ptr head = -1;
        Ptr garbage = -1; // a single linked list to store the erased pair(s)

        bool cached = false;
        PairData cachedData; // cached data

        // maybe some other cached stuff
    };

    struct Block_ {
        PairData data[2 * kTargetSize_];
        Ptr      pointers[2 * kTargetSize_]; // For leaf nodes, use it as linked pointer to other leafs
        long     count = 0;
        bool     leaf  = false;
    };

    constexpr static long kBlockSize_  = 4096;
    constexpr static long kTargetSize_ = (kBlockSize - 2 * sizeof(Ptr)) / (sizeof(Ptr) + sizeof(PairData)) / 2 - 1;

    static_assert(kTargetSize_ >= 2 && sizeof(Block_) > kBlockSize_);

    template<class Type>
    Type Read_(Ptr position);

    /**
     * Read the block from the file.
     * @param position
     * @return the block at the position
     */
    Block_ ReadBlock_(Ptr position);

    /**
     * Read the head meta from the class.
     * @param position
     * @return the ptr data
     */
    Head_ ReadHead_(Ptr position);

    /**
     * Allocate a space of 4KiB at the end of the file.
     */
    Ptr New_(long size);

    /**
     * Store the value at the given position.
     */
    template<class Type>
    void Write_(Ptr position, const Type& value);

    Head_ head_;
};
```

In File `tile_storage.h`

```c++
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
```

## In File `user.h`

```c++
#include "fixed_string.h"

using UserName   = FixedString<20>;
using Password   = FixedString<30>;
using Name       = FixedString<20>;
using mailAdress = FixedString<30>;

struct User {
    UserName   userName;
    Password   password;
    Name       name; // Actually a UTF-8 string
    mailAdress mailAddress;
    long       orderInfo = -1; // for ``query_order''
    int        privilege = 0;
};

class UserCompare {
    bool operator()(const User& lhs, const User& rhs);
};
```

## In File `user_manage.h`
```c++
#include "BP_tree.h"
#include "fixed_string.h"
#include "linked_hash_map.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "user.h"

class LoginPool {
public:
    LoginPool();

    ~LoginPool();

    bool Contains(FixedString<20> name); // Tell whether a user has logged in

    const User& getData(FixedString<20> name);

    void Clear();

private:
    LinkedHashMap<FixedString<20>, User, FixedStringHash<20>> loginUserMap_;
};

class UserManage {
public:
    UserManage();

    ~UserManage();

    void Adduser(ParameterTable& input, long timeStamp);

    void Login(ParameterTable& input);

    void Logout(ParameterTable& input);

    void Query(ParameterTable& input);

    void Modify(ParameterTable& input, long timeStamp);

    bool Logged(const std::string& name);

    long LastOrder(const std::string& name);

    long Addorder(const std::string& name, long position, long timeStamp);

    long ModifyLastOrderPtr(long position, long timeStamp);

    void RollBack(long time);

    void Clear();

private:
    LoginPool loginPool_;

    BPTree<UserName, long> userIndex_ = BPTree<UserName, long>("user_index");
    TileStorage<User> userData_ = TileStorage<User>("user_data");
};
```

## In File `train.h`

```c++
#include "fixed_string.h"

using TrainID = FixedString<20>;
using Station = FixedString<40>;

struct Date {
    bool operator<(const Date& rhs);
    bool operator>(const Date& rhs);
    bool operator==(const Date& rhs);
    bool operator+=(int rhs);
    bool operator+(int rhs);

    int month, day;
};

struct Time {
    bool operator<(const Time& rhs);
    bool operator>(const Time& rhs);
    bool operator==(const Time& rhs);
    Time& operator+=(const Time& rhs);
    bool operator-(const Time& rhs);
    Time operator+(const Time& rhs);
    Time& operator+=(int rhs);
    Time operator+(int rhs);
    int ToInt(); // the minute counting
    
    int day, hour, time;
};

struct Train {
    TrainID trainID;
    Station stations;
    long queueFirst = -1;
    long queueLast = -1;
    int  stationNum;
    int  seatNum[101];
    int  prefixPriceSum[101];
    Time departureTime[101];
    Time arrivalTime[101];
    Date startDate, endDate;
    char Type;
    bool status = false; // Indicate whether the train is released or not
};

struct Ticket {
    long trainPosition;
    int  number;
    int  price;
    int  from, to;
    int state; // 1 for bought, 0 for queuing, -1 for refunded
    long last = -1; // the last query
    long queue = -1; // the next queuing order
};

struct TrainTicketCount {
    int remained[100][100];
};
```

## In File `train_manage.h`

```c++
#include "BP_tree.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "train.h"

class TrainManage {
public:
    TrainManage();

    ~TrainManage();

    void Add(ParameterTable& input, long timeStamp);
    
    void Delete(ParameterTable& input, long timeStamp);

    void Release(ParameterTable& input, long timeStamp);

    void QueryTrain(ParameterTable& input);

    void QueryTicket(ParameterTable& input);

    void QueryTransfer(ParameterTable& input);

    void TryBuy(ParameterTable& input, long timeStamp);

    void QueryOrder(ParameterTable& input);

    void Refund(ParameterTable& input, long timeStamp);

    void RollBack(long timeStamp);

    void Clear();

private:
    BPTree<TrainID, long> trainIndex_ = BPTree<TrainID, long>("train_index");
    TileStorage<Train> trainData_ = TileStorage<Train>("train_data");
    TileStorage<TrainTicketCount> ticketData_ = TileStorage<TrainTicketCount>("ticket_data");
    BPTree<Station, long> startIndex_ = BPTree<Station, long>("start_station_index");
    BPTree<Station, long> terminalIndex_ = BPTree<Station, long>("terminal_station_index");
    TileStorage<Ticket> userTicketData_ = TileStorage<Ticket>("user_ticket_data");
};
```
