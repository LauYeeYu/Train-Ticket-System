# Code Structure for the Train Ticket System

## In File `main.cpp`

```c++
int main();

void Request(TokenScanner& input, UserManage& users, TrainManage& trains);
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

## In File `BP_tree.h`

```c++
template<class Key,
         class Value,
         class KeyCompare   = std::less<Key>,
         class ValueCompare = std::less<Value>,
         class KeyEqual     = std::equal<Key>,
         class ValueEqual   = std::equal<Value>>
class BPTree {
public:
    using Ptr = long;

    struct PairData {
        Key key;
        Value value;
        long timeStamp;
    }

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
    }

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

namespace user {

using UserName   = FixedString<20>;
using Password   = FixedString<30>;
using Name       = FixedString<20>;
using mailAdress = FixedString<30>;

struct User {
    UserName   userName;
    Password   password;
    Name       name; // Actually a UTF-8 string
    mailAdress mailAdress;
    long       orderInfo = -1; // for ``query_order''
    int        privilege = 0;
};

class UserCompare {
    bool operator()(const User& lhs, const User& rhs);
};

}
```

## In File `user_manage.h`
```c++
#include "fixed_string.h"
#include "user"

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

    void Adduser(TokenScanner& input);

    void Login(TokenScanner& input);

    void Logout(TokenScanner& input);

    void Query(TokenScanner& input);

    void Modify(TokenScanner& input);

    bool Logged(const std::string& name);

    long LastOrder(const std::string& name);

    long Addorder(const std::string& name, long position);

    long LastOrder(const std::string& name);

    void RollBack(long time);

private:
    LoginPool loginPool_;
};
```

## In File `train.h`

```c++
#include "fixed_string.h"

namesapce train {

using TrainID = FixedString<20>;
using Station = FixedString<40>;

struct Date {
    bool operator<(const Date& rhs);
    bool operator>(const Date& rhs);
    bool operator==(const Date& rhs);
    bool operator+=(int rhs);
    bool operator+(int rhs);
    friend std::ostream<<(std::ostream& os, const Date& date);

    int month, day;
};

struct Time {
    bool operator<(const Time& rhs);
    bool operator>(const Time& rhs);
    bool operator==(const Time& rhs);
    bool operator+=(const Time& rhs);
    bool operator+(const Time& rhs);
    bool operator+=(int rhs);
    bool operator+(int rhs);
    int ToInt(); // the minute counting
    friend std::ostream<<(std::ostream& os, const Time& time);

    int day, hour, time;
};

struct Train {
    TrainID trainID;
    Station stations;
    long queuefirst = -1;
    long queuelast = -1;
    int stationNum;
    int seatNum[100];
    int prefixPriceSum[100];
    Time departureTime[100];
    Time arrivalTime[100];
    Date startDate, endDate;
    char Type;
    bool status = false; // Indicate whether the train is released or not
};

struct Ticket {
    long trainPosition;
    int  number;
    int  price;
    int  from, to;
    bool state; // true for bought, false for queuing
    long last = -1; // the last query
    long queue = -1; // the next queuing order
};

} // namespace train
```

## In File `train_manage.h`

```c++
#include "train.h"
class TrainManage {
    public:
    TrainManage();

    ~TrainManage();

    void Add(TokenScanner& input);

    void Release(TokenScanner& input);

    void QueryTrain(TokenScanner& input);

    void QueryTicket(TokenScanner& input);

    void QueryTransfer(TokenScanner& input);

    void TryBuy(TokenScanner& input);

    void QueryOrder(TokenScanner& input);

    void Refund(TokenScanner& input);

    void RollBack();
};
```
    