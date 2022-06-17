# Train Ticket System 火車票系統
This is an assignment of SJTU ACM class, a train ticket system. For more
information about this assignment, click [here](https://github.com/ACMClassCourse-2021/TicketSystem).

此爲上海交通大學 ACM 班程式設計大作業之一，一個書店管理系統。如需獲取更多關於此大作業的内容，點此檢視[詳情](https://github.com/ACMClassCourse-2021/TicketSystem)。

## Overview 概覽
This program is a storage-based train ticket system. For the supported
feature, please view the support feature part. For the technical detail,
please see the documents in the `docs/` directory.

此程式爲一個基於儲存空間的火車票系統。支援的功能請參閱支援功能部分。技術詳細請參閱 `docs/` 目錄下的文件。

The system features ticket purchasing and refunding, querying travel
plans, and train data management, with an fantastic performance.

此系統支援票價購買與退票，查詢旅行計劃，以及火車資料管理，具有非常好的性能。

## Contributors 參與者

Team name 組名: 陆零叁贰壹

- [relyt871](https://github.com/relyt871) 姜天乐
  - B plus tree

    B+ 樹

  - the file IO (including the Least Recently Used cache)

    檔案輸入輸出（包括最近最少使用快取）

  - garbage collection on file

    檔案垃圾回收

  - rollback

    回滾

- [LauYeeYu](https://github.com/LauYeeYu) 刘祎禹
  - the logical part of the system

    邏輯部分

  - documents

    文檔

  - Pretty Print

    命令行交互擴展

## Progress 進度

- Correctness Test & Pressure Test: *Done* 2022-06-16 commit
  
  正確性測試及壓力測試: 於 2022-06-16 **完成**

  Commit:
  [ffc06eadb86205890788e26e6ac211ac6af810f9](https://github.com/LauYeeYu/Train-Ticket-System/commit/ffc06eadb86205890788e26e6ac211ac6af810f9)

- Rollback Test: *Done* 2022-06-16

  回滾測試: 於 2022-06-16 **完成**

  Commit:
  [0af8b98e0d93c0153b3070542938d9731f4e87b0](https://github.com/LauYeeYu/Train-Ticket-System/commit/0af8b98e0d93c0153b3070542938d9731f4e87b0)

- Bonus: *Partially Done*
  - Cache: Done (Least Recently Used Algorithm)

    快取: **完成**（ LRU 算法）

  - File Garbage Collection: Done

    檔案垃圾回收: **完成**

  - Pretty Print: **Done**

    命令行交互擴展: **完成**

  - Graphic User Interface: *In Progress*

    圖形界面: **尚在進行中**

## 系统说明

> 以下说明中 “整数” 均为非负整数

### 用户相关

账户数据涉及以下内容：

- `username`：用户的账户名，作为用户的**唯一**标识符，为由字母开头，由字母、数字和下划线组成的字符串，长度不超过 20。
- `password`：由可见字符组成的字符串，长度不低于 1，不超过 30。
- `name`：用户真实姓名，由 2 至 5 个汉字组成的字符串。
- `mailAddr`：同一般邮箱地址格式，且仅含数字、大小写字母，`@`和`.`，长度不超过 30（**无需检验邮箱格式的合法性**）。
- `privilege`：所处用户组优先级，为 0~10 中的一个整数。

### 车次相关

#### 车次数据涉及以下属性：

- `trainID`：车次的**唯一**标识符，由字母开头，字母、数字和下划线组成的字符串，长度不超过 20。

- `stationNum`：车次经过的车站数量，一个不低于 2 且不超过 100 的整数。

- `stations`：车次经过的所有车站名，共 `stationNum` 项，每个站名由汉字组成，不超过 10 个汉字。
- `seatNum`：该车次的座位数，一个不超过 $10^5$ 的整数。

- `prices`：每两站之间的票价，共 `(stationNum - 1)` 项，第 `i` 项表示第 `i` 站到第 `(i+1)` 站的票价，为一个不超过 $10^5$ 的整数。
- `startTime`：列车每日的发车时间。
  - 时间格式为 `hh:mm` ，例如 `23:51`
- `travelTimes`：每两站之间行车所用的时间，共 `(stationNum - 1)` 项。
  - 单位：分钟
  - 每一项是一个不超过 $10^4$ 的整数
- `stopoverTimes`：除始发站和终点站之外，列车在每一站停留的时间，共 `(stationNum - 2)` 项。
  - 单位：分钟
  - 每一项为一个不超过 $10^4$ 的整数
- `saleDate`：车次的售卖时间区间。
  - 由 2 项时间表示，每一项均为 2021 年 6 月至 8 月的某一日期
  - 日期格式为 `mm-dd`，例如：`06-01`、`08-31`
- `type`：列车类型，一个大写字母。

#### 具体说明：

- 在时间区间（`saleDate`）内，该车次每日发一班车

- 关于 `travelTimes` 和 `stopoverTimes`：保证每一辆车从始发站到终点站用时不超过 72 小时


## Support Feature 支援功能

#### 指令列表

##### [N] `add_user`

- 参数列表
  - `-c -u -p -n -m -g`
- 返回值
  - 注册成功：`0`
  - 注册失败：`-1`
- 权限要求
  - `-c` 已登录，且新用户的权限低于 `-c` 的权限

- 说明
  - 当前用户 `-c`（`<cur_username>`），创建一个账户名为 `-u`（`<username>`），密码为 `-p`（`<password>`），用户真实姓名为 `-n`（`<name>`），用户邮箱为 `-m`（`<mailAddr>`），权限为 `-g`（`<privilege>`）的用户
  - 特别地，创建第一个用户时，新用户权限为 `10`，不受上述权限规则约束。具体来讲，当创建第一个用户时，忽略 `-c` 和 `-g` 参数，并认为新用户优先级为 `10`
  - 如果 `<username>` 已经存在则注册失败


##### [F] `login`

- 参数列表

  `-u -p`

- 说明

  用户 `-u` (`<username>`) 尝试用 `-p`(`<password>`) 登录。
  若登录成功，当前用户列表增加此用户。第一次进入系统时，当前用户列表为空。
  若用户不存在或密码错误或者用户已经登录，则登陆失败。
- 返回值

  登录成功：`0`

  登录失败：`-1`

##### [F] `logout`

- 参数列表

  `-u`

- 说明

  `-u`(`<username>`) 用户退出登录。若退出成功，将此用户移出当前用户列表。

- 返回值

  登出成功：`0`

  登出失败：`-1`

##### [SF] `query_profile`

- 参数列表

  `-c -u`

- 说明

  用户 `-c`(`<cur_username>`) 查询 `-u`(`<username>`) 的用户信息。

  权限要求：`-c` 已登录，且「`-c` 的权限大于 `-u` 的权限，或是 `-c` 和 `-u` 相同」。

- 返回值

  查询成功：一行字符串，依次为被查询用户的 `<username>`, `<name>`, `<mailAddr>` 和 `<privilege>`，用一个空格隔开。

  查询失败：`-1`

##### [F] `modify_profile`

- 参数列表

  `-c -u (-p) (-n) (-m) (-g)`

- 说明

  用户 `-c` (`<cur_username>`) 修改 `-u` (`<username>`) 的用户信息。修改参数同注册参数，且均可以省略。

  权限要求：`-c` 已登录，且「`-c` 的权限大于 `-u` 的权限，或是 `-c` 和 `-u` 相同」，且 `-g` 需低于 `-c` 的权限。

- 返回值

  修改成功：返回被修改用户的信息，格式同 `query_profile`。

  修改失败：`-1`

##### [N] `add_train`

- 参数列表

  `-i -n -m -s -p -x -t -o -d -y`

- 说明

  添加 `<trainID>` 为 `-i`，`<stationNum>` 为 `-n`，`<seatNum>` 为 `-m`，`<stations>` 为 `-s`，`<prices>` 为 `-p`，`<startTime>` 为 `-x`，`<travelTimes>` 为 `-t`，`<stopoverTimes>` 为 `-o`，`<saleDate>` 为 `-d`，`<type>` 为 `-y` 的车次。
  由于 `-s`、`-p`、`-t`、`-o` 和 `-d` 由多个值组成，输入时两个值之间以 `|` 隔开（仍是一个不含空格的字符串）。

  输入保证火车的座位数大于0,站的数量不少于2不多于100，且如果火车只有两站 `-o` 后的参数用下划线代替（见举例2）,且火车不会经过同一个站两次。
  如果`<trainID>`已经存在则添加失败。

- 返回值

  添加成功：`0`

  添加失败：`-1`

- 举例1：

  `>[666] add_train -i HAPPY_TRAINA -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G`

  `[666] 0`
- 举例2：

  `>[667] add_train -i HAPPY_TRAINB -n 2 -m 1000 -s 上院|下院 -p 114 -x 19:19 -t 600 -o _ -d 06-01|08-17 -y G`

  `[667] 0`

##### [N] `delete_train`

- 参数列表

  `-i`

- 说明

  删除 `<trainID>` 为 `-i` 的车次，删除车次必须保证未发布。

- 返回值

  删除成功：`0`

  删除失败：`-1`

##### [N] `release_train`

- 参数列表

  `-i`

- 说明

  将车次 `-i` (`<trainID>`) 发布。
  发布前的车次，不可发售车票，无法被 `query_ticket` 和 `query_transfer` 操作所查询到；发布后的车次不可被删除。

- 返回值

  发布成功：`0`

  发布失败：`-1`

- 举例：

  `>[668] release_train -i HAPPY_TRAIN `

  `[668] 0`

##### [N] `query_train`

- 参数列表

  `-i -d`

- 说明

  查询在日期 `-d` 发车的，车次 `-i` (`<trainID>`) 的情况，`-d` 的格式为 `mm-dd`。

- 返回值

  查询成功：输出共 `(<stationNum> + 1)` 行。

  第一行为 `<trainID> <type>`。

  接下来 `<stationNum>` 行，第 `i` 行为 `<stations[i]> <ARRIVING_TIME> -> <LEAVING_TIME> <PRICE> <SEAT>`，其中 `<ARRIVING_TIME>` 和 `<LEAVING_TIME>` 为列车到达本站和离开本站的绝对时间，格式为 `mm-dd hr:mi`。`<PRICE>` 为从始发站乘坐至该站的累计票价，`<SEAT>` 为从该站到下一站的剩余票数。对于始发站的到达时间和终点站的出发时间，所有数字均用 `x` 代替；终点站的剩余票数用 `x` 代替。如果车辆还未 `release` 则认为所有票都没有被卖出去。

  查询失败：`-1`

- 举例

  （上接添加列车的例子）

  `>[669] query_train -d 07-01 -i HAPPY_TRAIN`

  `[669] HAPPY_TRAIN G`

  `上院 xx-xx xx:xx -> 07-01 19:19 0 1000`

  `中院 07-02 05:19 -> 07-02 05:24 114 1000`

  `下院 07-02 15:24 -> xx-xx xx:xx 628 x`

##### [SF] `query_ticket`

- 参数列表

  `-s -t -d (-p time)`

- 说明

  查询日期为 `-d` 时从 `-s` 出发，并到达 `-t` 的车票。请注意：这里的日期是列车从 `-s` 出发的日期，不是从列车始发站出发的日期。

  `-p` 的值为 `time` 和 `cost` 中的一个，若为 `time` 表示输出按照该车次所需时间从小到大排序，否则按照票价从低到高排序。如果按照时间排序车次所需时间相同，则把 `<trainID>` 作为第二关键字进行排序，按照票价排序；同理若出现车次票价相同，则同样把 `<trainID>` 作为第二关键字进行排序。

- 返回值

  第一行输出一个整数，表示符合要求的车次数量。

  接下来每一行输出一个符合要求的车次，按要求排序。格式为
  `<trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <SEAT>`，其中出发时间、到达时间格式同
  `query_train`，`<FROM>` 和 `<TO>` 为出发站和到达站，`<PRICE>` 为累计价格，`<SEAT>`
  为最多能购买的票数。

- 样例

  （上接查询列车的例子）

  `>[670] query_ticket -s 中院 -t 下院 -d 08-17`

  `[670] 1`

  `HAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000`

##### [N] `query_transfer`

参数列表及其意义同`query_ticket`。

- 说明

  在恰好换乘一次（换乘同一辆车不算恰好换乘一次）的情况下查询符合条件的车次，仅输出最优解。
  最优解的定义如下:
  * 若`(-p time)` 则 总时间作为第一关键字，总价格作为第二关键字，第一辆车的 `Train ID` 作为第三关键字，第二辆车 `Train ID` 作为第四关键字。
  * 若`(-p cost)` 则 总价格作为第一关键字，总时间作为第二关键字，第一辆车的 `Train ID` 作为第三关键字，第二辆车 `Train ID` 作为第四关键字。

  保证任意两种方案关键字均不同。
  请注意：这里的日期是列车从 `-s` 出发的日期，不是从列车始发站出发的日期。

- 返回值

  查询失败（没有符合要求的车次）：`0`

  查询成功：输出2行，换乘中搭乘的两个车次，格式同 `query_ticket`。

##### [SF] `buy_ticket`

- 参数列表
  `-u -i -d -n -f -t (-q false)`

- 说明

  用户 `-u` (`<username>`) 购买：车次 `-i` (`<trainID>`)，日期为 `-d`，从站 `-f` 到站 `-t` 的车票 `-n` 张。

  `-q` 可选 `false` 或 `true`，若为 `true`，表明在**余票不足**的情况下愿意接受候补购票，当有余票时**立即**视为此用户购买了车票，且保证购买的车票的数量大于0。请注意：这里的日期是列车从 `-f` 出发的日期，不是从列车始发站出发的日期。

  权限要求：`-u` 已登录，且购买的车次必须已经被 `release`。

- 返回值

  购买成功：一个整数，表示订单总价。

  加入候补：`queue`

  购票失败：`-1`

- 候补队列解释：
  对于余票不足且需候补购票的订单，将加入候补队列。当有订单出现退单的时候，车次的剩余车票会增加，你需要按照候补队列的下单先后顺序满足候补队列中的订单。

  ```text
  假设当前车次有100个座位，为了简单假设只有两个站，且接下来的描述均允许候补购票。
  1 buy 50
  2 buy 30
  3 buy 60
  4 buy 10
  5 buy 20
  经过五次购票订单后订单1，2，4被满足，车次还剩下10个座位，候补队列内剩下订单3和订单5。
  6 refund (2 buy 30)
  当2号订单被退票，车次内还剩下40个座位，但是候补队列中订单3需要60个座位，因此无法满足。 
  继续向后寻找能够满足的订单5，因此订单5通过候补成功购票，订单5从候补队列中被删除，并且该车次只剩下了20个座位。
  虽然订单有先后顺序，但不代表早的订单一定先完成，只是拥有更高的优先级，且订单为最小单位，不能先把部分票卖给该订单，只能完全完成一个订单。
  ```
##### [F] `query_order`

- 参数列表

  `-u`

- 说明

  查询用户 `-u` (`<username>`) 的所有订单信息，按照交易时间顺序从新到旧排序（候补订单即使补票成功，交易时间也以下单时刻为准）。

  权限要求：`-u` 已登录。

- 返回值

  查询成功：第一行输出一个整数，表示订单数量。

  接下来每一行表示一个订单，格式为
  `[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>`，其中
  `<NUM>` 为购票数量，`<STATUS>`
  表示该订单的状态，可能的值为：`success`（购票已成功）、`pending`（位于候补购票队列中）和`refunded`（已经退票）。

  查询失败：`-1`

- 样例

  `>[673] query_order -u Lappland`

  `[673] 1`

  `[pending] HAPPY_TRAIN 上院 08-17 05:24 -> 下院 08-17 15:24 628 500`

##### [N] `refund_ticket`

- 参数列表

  `-u (-n 1)`

- 说明

  用户 `-u` (`<username>`) 退订从新到旧（即 `query_order` 的返回顺序）第 `-n` 个 (1-base) 订单。

  权限要求：`-u` 已登录。

- 返回值

  退票成功：`0`

  退票失败：`-1`

##### [R] `rollback` (NEED to enable rollback feature)

- 参数列表

  `-t`

- 说明

  将所有用户下线，并将数据库回滚到时间戳为`-t`时刻的状态。

  仅在回滚测试点中出现。

- 返回值

  回滚成功：`0`

  回滚失败：`-1`

##### [R] `clean`

- 参数列表

  无

- 说明

  清除所有数据。

  保证不会在 `rollback` 指令的测试中出现。

- 返回值

  `0`

##### [R] `exit`

- 参数列表

  无

- 说明

  退出程序，所有在线用户均下线。

- 返回值

  `bye`

## How to Build 建構方法
### CLI only 僅命令行

CMake parameters:
- `-DROLLBACK=1`: enable rollback feature 啓用回滚功能
- `-DPRETTY_PRINT=1`: enable pretty print 啓用美化输出

Please type the following command to build the executable file:

請輸入以下命令建構標可執行檔：

```text
cmake . <CMakeParameters> && make <MakeParameters>
```

### CLI and GUI 命令行和 GUI
Please follow the steps in the CLI only to build the executable file (`train-ticket-system`).

請遵照前一部分僅命令行的建構方法來建構可執行檔 (`train-ticket-system`)。

Please make sure that you have installed the following packages:

請確保您的裝置上具有以下包：

- `node`
- `yarn`
