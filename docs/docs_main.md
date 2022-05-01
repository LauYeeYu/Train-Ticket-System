# Train Ticket System

## Oerview 概览

### Main Functions 主要功能

- 本次作业要求实现一个火车票订票系统，该系统向用户提供购票业务相关功能，包括车票查询、购票、订单操作等，以及向管理员提供后台管理功能。

- 系统需将用户数据、购票数据、车次数据进行本地存储，并对其进行高效操作。

### Basic Components 基本组件

- [用户数据储存](#用户数据储存)

- [车次数据储存](#车次数据储存)

- [登录信息](#登录信息)

- 交互

### Main Data Structure 主要数据结构

- B+ 树（用以建立数据的索引及储存数据）

  具体参见 [B+ 树文档](docs_bpt.md)

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
    char data[size + 1];

    char& operator[](long index);
    bool operator==(const FixedString& rhs);
    bool operator<(const FixedString& rhs);
};

struct User {
    FixedString<20> username;
    FixedString<30> password;
    FixedString<20> name; // Actually a UTF-8 string
    FixedString<30> mailAdress;
    int             privilege = 0;
};
```

比较结构（用于在 B+ 树中保持有序性）

```c++
class UserCompare {
    bool operator()(const User& lhs, const User& rhs);
};
```

## 车次数据储存

```c++
template<long size>
struct FixedString {
    char data[size + 1];

    char& operator[](long index);
    bool operator==(const FixedString& rhs);
    bool operator<(const FixedString& rhs);
};

struct Train {
    FixedString<20> trainID;
    FixedString<40> stations;
    int stationNum;
    int seatNum;
    int prices[100];
    int startHour;
    int startMinute;
    int travelTimes[100];
    int stopoverTimes[100];
    int saleMonth;
    int saleDay;
    char Type;
    bool status = false; // Indicate whether the train is released or not
};
```

## 登录信息
```c++
class LoginPool {
    bool Contains(FixedString<20> name); // Tell whether a user has logged in

    const User& getData(FixedString<20> name);

private:
    LinkedHashMap<FixedString<20>, User> loginUserMap_;
};

```

## 交互
