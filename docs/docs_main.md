# Train Ticket System

## Oerview 概览

### Main Functions 主要功能

- 本次作业要求实现一个火车票订票系统，该系统向用户提供购票业务相关功能，包括车票查询、购票、订单操作等，以及向管理员提供后台管理功能。

- 系统需将用户数据、购票数据、车次数据进行本地存储，并对其进行高效操作。

### Basic Components 基本组件

*注：如需查看代码结构，请参阅[代码结构文档](docs_code_structure.md)。*

- [用户数据储存](#用户数据储存)

- [车次数据储存](#车次数据储存)

- [登录信息](#登录信息)

- [信息表](#信息表)

- 交互

### Main Data Structure 主要数据结构

- 外部存储
  - B+ 树（用以建立数据的索引及储存数据）

    具体参见 [B+ 树文档](docs_bpt.md)

  - 平铺储存结构（用以储存大块数据）

    具体参见 [平铺储存结构文档](docs_tile_storage.md)

- 内存 (memory)

  - `LinkedHashMap`

  - `Vector`

### Bouns

计划完成的 bonus:

- 缓存，具体参见 B+ 树文件结构的技术细节中缓存部分。

- B+ 树空间回收：将无需使用的空间回收。

- 前端 (JavaScript 实现)

## 用户数据储存

需要储存的数据：

- `username`: 用户的账户名，作为用户的唯一标识符，为由字母开头，由字母、数字和下划线组成的字符串，长度不超过 20。

- `password`: 由可见字符组成的字符串，长度不低于 1，不超过 30。

- `name`: 用户真实姓名，由 2 至 5 个汉字组成的字符串。

- `mailAddress`：同一般邮箱地址格式，且仅含数字、大小写字母，`@` 和 `.`，长度不超过 30（无需检验邮箱格式的合法性）。

- `privilege`：所处用户组优先级，为 0~10 中的一个整数。

储存结构

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

struct User {
    FixedString<20> username;
    FixedString<30> password;
    FixedString<20> name; // Actually a UTF-8 string
    FixedString<30> mailAdress;
    long            orderInfo = -1; // for ``query_order''
    int             privilege = 0;
};
```

## 车次数据储存

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
    Time& operator+=(const Time& rhs);
    bool operator-(const Time& rhs);
    Time operator+(const Time& rhs);
    Time& operator+=(int rhs);
    Time operator+(int rhs);
    int ToInt(); // the minute counting
    friend std::ostream<<(std::ostream& os, const Time& time);

    int day, hour, time;
};

struct Train {
    FixedString<20> trainID;
    FixedString<40> stations;
    long place = -1;
    int stationNum;
    int seatNum[100];
    int prefixPriceSum[100];
    Time departureTime[100];
    Time arrivalTime[100];
    Date startDate, endDate;
    char Type;
    bool status = false; // Indicate whether the train is released or not
};
```

## 登录信息
```c++
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

```

## 信息表

- 用户信息索引表（B+ 树）: `user_index`

- 用户数据表（平铺储存结构）: `user_data`

- 用户购票表（平铺储存结构）: `user_ticket_data`

- 始发站车次索引表（B+ 树）: `start_station_index`

- 终点站车次索引表（B+ 树）: `terminal_station_index`

- 车次信息索引表（B+ 树）: `train_index`

- 车次数据表（平铺储存结构）: `train_data`

- 车次购票表（平铺储存结构）: `ticket_data`

## 交互

请参阅[交互部分的文档](docs_interaction.md)。
