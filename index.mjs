import koa from 'koa'
import Router from '@koa/router'
import bodyparser from 'koa-body'
import { createInterface } from 'readline'
import { spawn } from 'child_process'

const app = new koa()
const router = new Router()
app.use(bodyparser()).use(router.routes()).use(router.allowedMethods())
const escape = str => str
    .replace(/</g, '&lt')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')

const proc = spawn('./train-ticket-system')


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


router.get('/', async ctx => {
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

router.post('/', async ctx => {
    // Logical stuff
})

app.listen(8080)
