# Graphic User Interface

## 技术栈

### 后端部分

基于原先的后端，稍作修改（如输出更完整的信息等），编译为二进制文件。

### 前端部分

- 图形界面控制： `html` + `css` + `JavaScript`(前端 JavaScript 作为动效控制)

- 服务端控制： `nodejs`

### 前后端链接

通过 `nodejs` 建立一个子进程，运行后端二进制，将前端的 `post` 请求格式化为输入数据，并针对输出数据进行对应页面的变化、跳转。

具体方式如下：
1. 前端通过链接跳转 (get 请求)，跳转到对应页面。
2. 在操作页面上，将输入作为 `post` 请求，进入 `post` 对应页面，输入到后端，等待后端输出后，将输出数据返回给前端，进行对应的页面的渲染。

## 图形界面逻辑

### 主页面

主页面为火车信息页面，有各个区块指向各个操作的页面链接。

- `add_train` (`/add-train`)
- `delete_train` (`/delete-train`)
- `release_train` (`/release-train`)
- `query_train` (`/query-train`)
- `query_ticket` (`/query-ticket`)
- `query_transfer` (`/query-transfer`)
- `buy_ticket` (`/buy-ticket`)
- `query_order` (`/query-order`)
- `refund_ticket` (`/refund-ticket`)
- `rollback` (`/rollback`)
- `clean` (`/clean`)

主页面左上角为用户信息相关页面。

### 用户页面

用户页面中，有各个区块指向各个用户操作的页面链接。

- `login` (`/login`)
- `logout` (`/logout`)
- `add_user` (`/add-user`)
- `query_profile` (`/query-profile`)
- `modify_profile` (`/modify-profile`)

左上角为首页链接。

### 操作界面

每个操作界面非常相似，都是针对需要的输入信息设计的若干输入框。

点击提交按钮之后就将进行对应的 `post` 请求。

具体操作逻辑见[前后端链接部分](#前后端链接)。

### 服务端 JavaScript 格式

- import 部分
```javascript
import koa from 'koa'
import Router from '@koa/router'
import bodyparser from 'koa-body'
import { createInterface } from 'readline'
import { spawn } from 'child_process'
import request from 'koa/lib/request'
```

- 建立服务
```javascript
const app = new koa()
const router = new Router()
app.use(bodyparser()).use(router.routes()).use(router.allowedMethods())
```

- 转义输入字符
```javascript
const escape = str => str
    .replace(/</g, '&lt')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
```
- 建立子进程
```javascript
const proc = spawn('./train-ticket-system')
```

- 将子进程的输入输出内容与 JavaScript 输入输出链接 (注意: 子进程的输出内容是后端的输入内容, 反之亦然)
```javascript
const s = createInterface({ input: proc.stdout }) // 建立读取子进程输出的接口
const queue = []

// when a read stream has found something to read put it into the queue 
s.on('line', line => {
    if (queue.length === 0) buffer.push(line) 
    else queue.shift()(line)
})
// put a line
const putline = line => proc.stdin.write(line + '\n')
// get a line
const getline = () => {
    if (buffer.length === 0) return new Promise(resolve => queue.push(resolve))
    else return buffer.shift()
}
```

- Get 请求
```javascript
router.get('/* link */', async ctx => {
    ctx.body = `
    <html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
        /* main body part */
        </main>
    </body>
    </html>
    `
})
```

- Post 请求
 ```javascript
router.post('/* post link */', async ctx => {
    // Logical stuff
})
```

- 端口占用
```javascript
app.listen(8080)
```

