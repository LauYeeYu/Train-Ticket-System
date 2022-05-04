# Interaction

## `add_user`

参数：

`-c -u -p -n -m -g`

行为：(`UserManage::Add`)

1. 检查 `-c` 是否已经登录，如未登录，输出 `-1` 并退出函数。

2. 检查 `-u` 是否已经存在，如存在，输出 `-1` 并退出函数。

3. 检查剩余参数是否符合对应的要求，如不符合，输出 `-1` 并退出函数。

4. 输出 `0` 并退出函数。

## `login`

参数：

`-u -p`

行为：(`UserManage::Login`)

1. 检查用户是否已经登录，如已经登录，输出 `-1` 并退出函数。

2. 检查是否存在用户，如不存在，输出 `-1` 并退出函数。

3. 从 `user_index` 拉取用户信息，匹配密码，如密码不匹配，输出 `-1` 并退出函数。

4. 使账户登录，输出 `0` 并退出函数。

## `logout`

参数：

`-u`

行为：(`UserManage::Logout`)

1. 检查是否登录，如未登录，输出 `-1` 并退出函数。

2. 登出用户，从登录池移除用户，输出 `0` 并退出函数。

## `query_profile`

参数：

`-c -u`

行为：(`UserManage::Query`)

1. 检查 `-c` 是否登录，如未登录，输出 `-1` 并退出函数。

2. 检查 `-u` 是否登录（登录的用户在内存中有记录，减少响应时间），如未登录，检查是否存在，如不存在，输出 `-1` 并退出函数，如存在，从文件中拉取数据。

3. 检查 `-c` 权限是否不小于 `-u`，如不是，输出 `-1` 并退出函数。

4. 依次输出 `<username>`, `<name>`, `<mailAdress>` 和 `privilege`，以空格分隔，退出函数。

## `modify_profile`

参数：

`-c -u (-p) (-n) (-m) (-g)`

行为：(`TrainManage::Modify`)

1. 检查 `-c` 是否登录，如未登录，输出 `-1` 并退出函数。

2. 检查 `-u` 是否存在，并从文件中拉取信息。（Future: 尝试从登录池中检查，需要将对应的位置也载入登录池）

3. 如 `-g` 对应不为空，检查 `-c` 权限是否大于 `-u`，否则检查 `-c` 权限是否不小于 `-u`，如不是，输出 `-1` 并退出函数。

4. 依次新的输出 `<username>`, `<name>`, `<mailAdress>` 和 `privilege`，以空格分隔，退出函数。 

## `add_train`

参数：

`-i -n -m -p -x -t -o -d -y`

行为：(`TrainManage::Add`)

1. 检查 `<trainID>` 是否存在，如存在，输出 `-1` 并退出函数。

2. 数据经过预处理后写入索引文件（始发站车次索引表、终点站车次索引表、车次信息索引表、车次数据表、车次购票表）中。

3. 输出 `0` 并退出函数。

## `release_train`

参数：

`-i`

行为：(`TrainManage::Release`)

1. 检查是否存在该 `<trainID>`，如不存在，输出 `-1` 并退出函数。

2. 将对应的车次设定为 `released`，输出 `0` 并退出函数。

## `query_train`

参数：

`-i -d`

行为：(`TrainManage::QueryTrain`)

1. 检查是否存在该车次，如不存在，输出 `-1` 并退出函数。

2. 检查在对应日期是否有此车次，如不存在，输出 `-1` 并退出函数。

3. 输出 `<trainID> <type>`

4. 输出 `<stationNum>` 行，每行内容为 `<station> <arrivalTime> -> <departureTime> <price> <seat>`，其中 `<arrivalTime>` 和 `<departureTime>` 为列车到达本站和离开本站的绝对时间，格式为 `mm-dd hr:mi`。`<price>` 为从始发站乘坐至该站的累计票价， `<seat>` 为从该站到下一站的剩余票数。对于始发站的到达时间和终点站的出发时间，所有数字均用 `x` 代替；终点站的剩余票数用`x` 代替。如果车辆还未释出则认为所有票都没有被卖出去。

## `query_ticket`

参数：

`-s -t -d (-p time)`

行为：(`TrainManage::QueryTicket`)

1. 从始发站车次索引表中读取含有对应始发站，且终点站为所输入终点站、出发日期符合车次的车次信息。

2. 按照排序依据排序：如有 `-p time`，则按照时间顺序排序；否则按照手票价排序，如出现票价相同，依据 `<trainID》` 字典序排序。

3. 输出符合要求的车次数量。

4. 依据此前的排序输出 `<trainID> <from> <arrivalTime> -> <to> <departureTime> <price> <seat>`，其中 `<arrivalTime>` 和 `<departureTime>` 为列车到达本站和离开本站的绝对时间，格式为 `mm-dd hr:mi`。`<price>` 为累计票价， `<seat>` 为剩余票数。

## `query_transfer`

参数：

`-s -t -d (-p time)`

行为：(`TrainManage::QueryTransfer`)

1. 从始发站车次索引表中读取含有对应始发站且出发日期符合车次的车次信息。

2. 从终点站车次索引表中读取含有对应终点站的车次信息。

3. 针对两车的途径站进行比对，如有匹配，检查是否在时间上可行。

4. 按照排序依据排序：如有 `-p time`，则按照时间顺序（到达时间减去出发时间）排序；否则按照手票价排序，如出现票价相同，依据 `<trainID>` 字典序排序。

5. 输出符合要求的车次数量。

6. 依据此前的排序输出 `<trainID> <from> <arrivalTime> -> <to> <departureTime> <price> <seat>`，其中 `<arrivalTime>` 和 `<departureTime>` 为列车到达本站和离开本站的绝对时间，格式为 `mm-dd hr:mi`。`<price>` 为累计票价， `<seat>` 为剩余票数。

## `buy_ticket`

参数：

`-u -i -d -n -f -t (-q false)`

行为：(`TrainManage::BuyTicket`)

1. 检查用户是否登录，如未登录，输出 `-1` 并退出函数。

2. 检查车次是否存在，如不存在，输出 `-1` 并退出函数。

3. 检查车次是否被释出，如未释出，输出 `-1` 并退出函数。

4. 检查是否可以购票，如不可以，且拒绝候补，则直接输出 `-1` 并退出；如接受候补，从 `Usermanage::LastOrder`
获取上一次的信息，则打上 queue 标记，并将车次对应的候补链表尾部加入此节点，调用
 `Usermanage::Addorder`，输出 `queue` 并退出函数。如可以购票，则减少对应的票数，从
`Usermanage::LastOrder` 获取上一次的信息，将新的订单信息写入用户购票数据表，并调用 `Usermanage::Addorder`。

5. 输出价格并退出函数。

## `query_order`

参数：

`-u`

行为：(`TrainManage::QueryOrder`)

1. 检查用户是否登录，如未登录，输出 `-1` 并退出。

2. 从 `Usermanage::LastOrder` 开始不断将之前的火车信息获取到 `Vector` 中。

3. 按照格式 `[<status>] <trainID> <from> <arrivalTime> -> <to> <departureTime> <price> <num>`，其中
`status` 表示该订单的状态，可能的值为：`success`（购票已成功）、`pending`（位于候补购票队列中）和
`refunded`（已经退票）, `<arrivalTime>` 和 `<departureTime>` 为列车到达本站和离开本站的绝对时间，格式为
`mm-dd hr:mi`。`<price>` 为订单价格， `<num>` 为的剩余票数。

## `refund_ticket`

参数：

`-u (-n 1)`

行为：(`TrainManage::Refund`)

1. 检查用户是否登录，如未登录，则输出 `-1` 并退出。

2. 从 `Usermanage::LastOrder` 找到对应的订单，如不存在，则输出 `-1` 并退出。

3. 修改订单并修改对应订单之后所有订单，修改用户数据中的最新订单，修改用户索引。

4. 从候补队列中找寻可以购买车票的用户并购买车票，写入用户数据。

4. 退出函数。

## `rollback`

参数：

`-t`

行为：

1. 遍历用户索引表并读入暂存，并将用户索引表回滚。

2. 通过上述数据遍历用户数据表，将购票信息回滚。

3. 回滚用户数据表。

4. 遍历车次索引表，并读入暂存区，将车次索引表表回滚。

5. 回滚始发站车次索引表、终点站车次索引表。

5. 依据读入暂存的车次索引表遍历车次信息表，回滚购票信息表。

6. 依据读入暂存的车次索引表回滚车次信息表。

## `clean`

行为：

清除所有数据。

## `exit`

输出 `bye`，退出程序，下线所有用户。
