# Code Structure for the Train Ticket System

## In File `main.cpp`

```c++

void TryCreateFile(const char* fileName);

void Init();

bool Request(ParameterTable& parameterTable, UserManage& users, TrainManage& trains);

int main();
```

## In File `fixed_string.h`

```c++
template<long size>
struct FixedString {
public:
    FixedString();

    explicit FixedString(const char* str);

    explicit FixedString(const std::string& str);

    FixedString(const FixedString& str);

    FixedString& operator=(const FixedString& str);

    char& operator[](long index);

    bool operator==(const FixedString& rhs);

    bool operator<(const FixedString& rhs);
};

class FixedStringHash {
public:
    FixedStringHash() = default;
    FixedStringHash(const FixedStringHash&) = default;
    FixedStringHash& operator=(const FixedStringHash&) = default;
    ~FixedStringHash() = default;

    template<long size>
    std::size_t operator()(FixedString<size> string);
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
    BPTree(const char* filename): memo(filename, isNew);
    
    ~BPTree();

    bool Empty();

    void Clear();

    bool Contains(const KeyT &key);

    ValT Find();

    Vector<ValT> MultiFind(const KeyT &key);

    void Insert(const KeyT &key, const ValT &val);

    void Erase(const KeyT &key);
};
```

In File `tile_storage.h`

```c++
template<class T>
class TileStorage {
public:
    using Ptr = long;
    /**
     * Binding the class with a certain file.  If the file is not empty, the
     * position of deleted nodes should be read from the very beginning of
     * the file.
     */
    TileStorage(const char* fileName);

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
     * Get the value at the position.
     * @return the data at the position
     */
    T Get(Ptr position);

    /**
     * Modify the data at the position with the newValue and the time stamp.
     * @return the position of the new value
     */
    Ptr Modify(Ptr position, const T& newValue, long timeStamp);

    /**
     * Roll back the data at the position to a certain time stamp.  The node belongs to the ``future'' can be deleted
     * @return the position of the rolled backed node
     */
    Ptr RollBack(Ptr position, long timeStamp);

    void Clear();
};
```

## In File `user.h`

```c++
#include "fixed_string.h"
#include "utility.h"

using UserName   = FixedString<20>;
using Password   = HashPair;
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
```

## In File `user_manage.h`
```c++
#include "BP_tree.h"
#include "fixed_string.h"
#include "linked_hash_map.h"
#include "parameter_table.h"
#include "tile_storage.h"
#include "train.h"
#include "user.h"
#include "utility.h"

class TrainManage;

class LoginPool {
public:
    LoginPool() = default;

    ~LoginPool() = default;

    void Login(const User& user);

    void Logout(const UserName& userName);

    bool Contains(const FixedString<20>& name); // Tell whether a user has logged in

    bool Contains(const std::string& name); // Tell whether a user has logged in

    const User& GetData(const FixedString<20>& name);

    const User& GetData(const std::string& name);

    void ModifyProfile(const User& user);

    void Clear();

    bool Empty();

private:
    LinkedHashMap<UserName, User, FixedStringHash1> loginUserMap_;
};

class UserManage {
public:
    UserManage() = default;

    ~UserManage() = default;

    void AddUser(ParameterTable& input);

    void Login(ParameterTable& input);

    void Logout(ParameterTable& input);

    void Query(ParameterTable& input);

    void Modify(ParameterTable& input);

    bool Logged(const std::string& name);

    long AddOrder(const std::string& name, Ticket& ticket, long timeStamp, TrainManage& trainManage);

    //void RollBack(long time);

    void Clear();

    const User& GetUser(const std::string& name);

private:
    void Adduser_(User& user, long timeStamp);

    LoginPool loginPool_;

    BPTree<HashPair, long> userIndex_ = BPTree<HashPair, long>("user_index");
    TileStorage<User>      userData_  = TileStorage<User>("user_data");
};
```

## In File `train.h`

```c++
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
    bool operator!=(const Date& rhs) const;
    Date& operator+=(int rhs);
    Date operator+(int rhs);
    Date& operator-=(int rhs);
    Date operator-(int rhs);
    
    friend std::ostream& operator<<(std::ostream& os, const Date& date);
    
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
    bool operator!=(const Time& rhs) const;
    Time& operator+=(const Time& rhs);
    Time operator-(const Time& rhs);
    Time operator+(const Time& rhs);
    Time& operator+=(int rhs);
    Time operator+(int rhs);
    
    friend std::ostream& operator<<(std::ostream& os, const Time& time);
    
    int minute;
};

struct Train {
    TrainID trainID;
    Station stations[101];
    long ticketData = -1;
    int  stationNum;
    int  seatNum;
    int  prefixPriceSum[101];
    Time departureTime[101];
    Time arrivalTime[101];
    Date startDate, endDate;
    char type;
    bool released = false; // Indicate whether the train is released or not
};

struct Journey {
    TrainID trainID;
    Station startStation;
    Station endStation;
    Date startDate;
    Time startTime;
    Date endDate;
    Time endTime;
    int price;
    int seat;
    
    friend std::ostream& operator<<(std::ostream& os, const Journey& journey);
};

enum class TicketState {
    refunded = -1,
    pending = 0,
    bought = 1,
};

struct Ticket {
    TrainID trainID;
    Station startStation;
    Station endStation;
    Date startDate;
    Time startTime;
    Date endDate;
    Time endTime;
    long trainPosition;
    long ticketPosition;
    int index;
    int price;
    int seatNum;
    int from, to;
    TicketState state; // 1 for bought, 0 for queuing, -1 for refunded
    long last = -1; // the last query
    long queue = -1; // the next queuing order
    friend std::ostream& operator<<(std::ostream& os, const Ticket& ticket);
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
#include "utility.h"
#include "user_manage.h"

using StationPair = Pair<long, long>;

class TrainManage {
    public:
    TrainManage() = default;
    
    ~TrainManage() = default;
    
    void Add(ParameterTable& input);
    
    void Delete(ParameterTable& input);
    
    void Release(ParameterTable& input);
    
    void QueryTrain(ParameterTable& input);
    
    void QueryTicket(ParameterTable& input);
    
    void QueryTransfer(ParameterTable& input);
    
    void TryBuy(ParameterTable& input, UserManage& userManage);
    
    long AddOrder(Ticket& ticket, long timeStamp);
    
    void QueryOrder(ParameterTable& input, UserManage& userManage);
    
    void Refund(ParameterTable& input, UserManage& userManage);
    
    //void RollBack(long timeStamp);
    
    void Clear();
    
    private:
    BPTree<HashPair, long>        trainIndex_     = BPTree<HashPair, long>("train_index");
    TileStorage<Train>            trainData_      = TileStorage<Train>("train_data");
    TileStorage<TrainTicketCount> ticketData_     = TileStorage<TrainTicketCount>("ticket_data");
    BPTree<HashPair, StationPair> stationIndex_   = BPTree<HashPair, StationPair>("station_index");
    TileStorage<Ticket>           userTicketData_ = TileStorage<Ticket>("user_ticket_data");
};
```
