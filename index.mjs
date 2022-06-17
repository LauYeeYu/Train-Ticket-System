import koa from 'koa'
import Router from '@koa/router'
import bodyparser from 'koa-body'
import { createInterface } from 'readline'
import { spawn } from 'child_process'
import { v4 as uuid } from 'uuid'

const app = new koa()
const router = new Router()
app.use(bodyparser()).use(router.routes()).use(router.allowedMethods())
const escape = str => str
    .replace(/</g, '&lt')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')

const proc = spawn('./train-ticket-system')

let userMap = new Map()
let privilegeMap = new Map()

const s = createInterface({ input: proc.stdout }) // binding the stdout of subprocess to a readline interface
const queue = []
const buffer = []

let timeStamp = 0

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

const dismissTimeStamp = raw => {
    return raw.split(' ').slice(1).join(' ')
}

let getProfile = async user => {
    ++timeStamp
   putline(`[${timeStamp}] query_profile -c ${user} -u ${user}`)
    return dismissTimeStamp(await getline())
}

// terminating the system
const terminateCode = uuid()
console.log(`Your terminate code is ${terminateCode}.`)
console.log(`Please visit /${terminateCode} to terminate the system.`)
router.get(`/${terminateCode}`, async ctx => {
    putline('[0] exit')
    if ((await getline()).split(' ')[1] === 'bye') {
        ctx.body = `
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
</head>
<body>
    <main>
    <h1>The system has been terminated, please also terminate the JavaScript service.</h1>
    </main>
</body>
</html>`
    }
})

router.get('/', async ctx => {
    if (!ctx.cookies.get('sessionID')) {
        ctx.body = `
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
</head>
<body>
    <main>
    Not logged in.
    </main>
</body>
</html>`
        return
    }

    ctx.body = `
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
</head>
<body>
    <main>
    logged in as ${ctx.cookies.get('username')}
    </main>
</body>
</html>`
})

router.post('/login', async ctx => {
    ++timeStamp
    putline(`[${timeStamp}] login -u ${ctx.request.body.username} -p ${ctx.request.body.password}`)
    const msg = dismissTimeStamp(await getline())
    if (userMap.has(ctx.request.body.username)) {
        if (msg === "Login failed: the user has already logged in.") {
            ctx.cookies.set('sessionID', userMap[ctx.request.body.username])
            ctx.cookies.set('username', ctx.request.body.username)
            ctx.redirect('/')
        } else {
            ctx.body = `
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
</head>
<body>
    <main>
    ${msg}
    </main>
</body>
</html>`
        }
    } else {
        if (msg === "Login successfully.") {
            userMap[ctx.request.body.username] = uuid()
            ctx.cookies.set('sessionID', userMap[ctx.request.body.username])
            ctx.cookies.set('username', ctx.request.body.username)
            // example: username name email@example.com 10(privilege)
            privilegeMap[ctx.request.body.username] = (await getProfile(ctx.query.user)).split(' ')[3]
            ctx.redirect('/')
        } else {
            ctx.body = `
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
</head>
<body>
    <main>
    ${msg}
    </main>
</body>
</html>`
        }
    }
})

router.get('/logout', async ctx => {
    ctx.cookies.set('sessionID', '', { expires: new Date(0) })
    ctx.cookies.set('username', '', { expires: new Date(0) })
    ctx.redirect('/')
})

router.get('/query-plan', ctx => {
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
            <p>Ticket</p>
            <form action="/query-ticket" method="post">
                <p>Destination: <input name="destination"></p>
                <p>Departure: <input name="departure"></p>
                <p>Date: <input name="month">-<input name="date"></p>
                <select name="sort" id="modify">
                    <option value="time">-time by default-</option>
                    <option value="time">time</option>
                    <option value="cost">cost</option>
                </select>
                <button type="submit">Search</button>
            </form>
            <p>Transfer</p>
            <form action="/query-transfer" method="post">
                <p>Destination: <input name="destination"></p>
                <p>Departure: <input name="departure"></p>
                <p>Date: <input name="month">-<input name="date"></p>
                <select name="sort" id="modify">
                    <option value="time">-time by default-</option>
                    <option value="time">time</option>
                    <option value="cost">cost</option>
                </select>
                <button type="submit">Search</button>
            </form>
        </main>
    </body>
</html>`
})

router.post('/query-ticket', async ctx => {
    ++timeStamp
    putline(`[${timeStamp}] query_ticket -t ${ctx.request.body.destination} -s ${ctx.request.body.departure} -d ${ctx.request.body.month}-${ctx.request.body.date} -p ${ctx.request.body.sort}`)
    const num = (await getline()).split(' ')[1]

    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
            `

    for (let i = 0; i < num; ++i) {
        const [ trainID, from, leaving, arrow, to, arrival, price, seats ] = (await getline()).split(' ')
        if (seats === '0' || seats === '1') {
            ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seat</p>
    <p>${from} -> ${to}</p>
    <p>${leaving} ${arrival}</p>    
</div>`
        } else {
            ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seats</p>
    <p>${from} -> ${to}</p>
    <p>${leaving} ${arrival}</p>
</div>`
        }
    }
    ctx.body += `
        </main>
    </body>
</html>`
})

router.post('/query-transfer', async ctx => {
    ++timeStamp
    putline(`[${timeStamp}] query_transfer -t ${ctx.request.body.destination} -s ${ctx.request.body.departure} -d ${ctx.request.body.month}-${ctx.request.body.date} -p ${ctx.request.body.sort}`)
    const num = (await getline()).split(' ')[1]

    if (num === '0') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
        No possible plan
        </main>
    </body>
</html>`
    } else {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
            `

        for (let i = 0; i < 2; ++i) {
            const [trainID, from, leaving, arrow, to, arrival, price, seats] = (await getline()).split(' ')
            if (seats === '0' || seats === '1') {
                ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seat</p>
    <p>${from} -> ${to}</p>
    <p>${leaving} ${arrival}</p>    
</div>`
            } else {
                ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seats</p>
    <p>${from} -> ${to}</p>
    <p>${leaving} ${arrival}</p>
</div>`
            }
        }
        ctx.body += `
        </main>
    </body>
</html>`
    }
})

router.get('/profile', async ctx => {
    // example: username name email@example.com 10(privilege)
    const msg = await getProfile(ctx.cookies.get('username'))
    if (msg.split(' ')[1] === 'failed:') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
        <p>You are not logged in</p>
        </main>
    </body>
</html>`

    }
    const [ username, name, email, privilege ] = msg.split(' ')
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
            <p>Ticket</p>
            <form action="/profile" method="post">
                <p>username: <input name="username value="${username}"></p>
                <p>name: <input name="name" value="${name}"></p>
                <p>mail address: <input name="mailAddress" value="${email}"></p>
                <p>privilege: <input name="privilege" value="${privilege}"></p>
                <button type="submit">Search</button>
        </main>
    </body>
</html>`
})

router.post('/profile', async ctx => {
    putline(`[${timeStamp}] modify_profile -c ${ctx.request.body.username} -u ${ctx.request.body.username} -n ${ctx.request.body.name} -m ${ctx.request.body.mailAddress} -g ${ctx.request.body.privilege}`)
    const msg = await getline()
    if (msg.split(' ')[1] === 'failed:') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>`
    } else {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet" href="styles.css">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>
</html>`
    }
})

router.post('/query-train', async ctx => {
    ++timeStamp
    putline(`[${timeStamp}] query-train -u ${ctx.request.body.trainID} -t ${ctx.request.body.month}-${ctx.request.body.day}`)
    const msg = dismissTimeStamp(await getline())
    if (msg.split(' ')[1] === 'failed') {
        ctx.body = `
            <html>
            <head>
                <meta charset="UTF-8">
                <link rel="stylesheet" href="styles.css">
                <meta http-equiv="X-UA-Compatible">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Ticket System</title>
            </head>
            <body>
                <main>
                ${msg}
                </main>
            </body>
        </html>`
        return
    }
    const num = msg.split(' ')[5]
    ctx.body = `
        <html>
        <head>
            <meta charset="UTF-8">
            <link rel="stylesheet" href="styles.css">
            <meta http-equiv="X-UA-Compatible">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Ticket System</title>
        </head>
        <body>
            <main>
            ${msg}
                <table border="1" class="centre">
                    <tr>
                        <th>station</th>
                        <th>arrival date</th>
                        <th>arrival time</th>
                        <th>departure date</th>
                        <th>departure time</th>
                        <th>total price</th>
                        <th>remained seats</th>
                    </tr>`
    for (let i = 0; i < num; i++) {
        const [ station, arrivalDate, arrivalTime, arrow, departureDate, departureTime, price, seats ] = (await getline()).split(' ')
        ctx.body += `
        <tr>
            <td>${station}</td>
            <td>${arrivalDate}</td>
            <td>${arrivalTime}</td>
            <td>${departureDate}</td>
            <td>${departureTime}</td>
            <td>${price}</td>
            <td>${seats}</td>
        <tr>
        `
    }
    ctx.body += `
            </main>
        </body>
        </html>
    `
})

router.get('/styles.css', ctx => {
    ctx.body = `
    /* reset */
            * { margin: 0; padding: 0; box-sizing: border-box; }
            img { border: none; }
            table { border-collapse: collapse; }
            a {
                text-decoration: none;
                margin: 0 16px;
            }
            a:hover, a:active, a:focus { text-decoration: underline; }
            ol, ul { margin-left: 2em; }
            body {
                padding: 16px;
                padding-bottom: 64px;
                min-height: 100vh;
                text-align: center;
                background-color: #fcfffc;
                color: #222;
                border-top: 4px solid #6186b6;
            }
            main {
                padding-top: 64px;
                text-align: start;
                display: inline-block;
                max-width: 1080px;
            }
            blockquote { padding-left: 2rem; position: relative; }
            blockquote::before {
                content: '“';
                position: absolute;
                font-size: 3rem;
                line-height: 3.125rem;
                font-weight: 400;
                right: calc(100% - 1.75rem);
                top: 0;
                color: rgba(0, 0, 255, .5);
            }
            /* typography */
            h1, h2, h3, h4, h5, h6, main, .subtitle1, .subtitle2, .body2, caption, button, .button, .overline {
                -moz-osx-font-smoothing: grayscale;
                -webkit-font-smoothing: antialiased;
                font-family: Roboto, sans-serif;
                text-decoration: inherit;
                text-transform: inherit;
            }
            h1 {
                font-size: 4.25rem;
                line-height: 5rem;
                padding: 2rem 0;
                font-weight: 200;
                letter-spacing: -0.015625em;
                position: relative;
            }
            h2 {
                font-size: 3.25rem;
                line-height: 3.25rem;
                padding: 1.25rem 0;
                font-weight: 200;
                letter-spacing: -0.0083333333em;
            }
            h3 {
                font-size: 2.75rem;
                line-height: 3.125rem;
                padding: 1rem 0;
                font-weight: 200;
                letter-spacing: normal;
            }
            h4 {
                font-size: 2.125rem;
                line-height: 2.5rem;
                padding: 0.75rem 0;
                font-weight: 400;
                letter-spacing: 0.0073529412em;
            }
            h5 {
                font-size: 1.5rem;
                line-height: 2rem;
                padding: 0.5rem 0;
                font-weight: 400;
                letter-spacing: normal;
            }
            h6 {
                font-size: 1.25rem;
                line-height: 2rem;
                font-weight: 500;
                letter-spacing: 0.0125em;
            }
            .subtitle1 {
                font-size: 1rem;
                line-height: 1.75rem;
                font-weight: 400;
                letter-spacing: 0.009375em;
            }
            .subtitle2 {
                font-size: 0.875rem;
                line-height: 1.375rem;
                font-weight: 500;
                letter-spacing: 0.0071428571em;
            }
            main {
                font-size: 1rem;
                line-height: 1.5rem;
                font-weight: 400;
                letter-spacing: 0.03125em;
            }
            p, li { padding: 0.25rem 0; }
            .body2 {
                font-size: 0.875rem;
                line-height: 1.25rem;
                font-weight: 400;
                letter-spacing: 0.0178571429em;
            }
            caption {
                font-size: 0.75rem;
                line-height: 1.25rem;
                font-weight: 400;
                letter-spacing: 0.0333333333em;
            }
            button, .button {
                font-size: 0.875rem;
                line-height: 2.25rem;
                font-weight: 500;
                letter-spacing: 0.0892857143em;
                text-transform: uppercase;
            }
            .overline {
                font-size: 0.75rem;
                line-height: 2rem;
                font-weight: 500;
                letter-spacing: 0.1666666667em;
                text-transform: uppercase;
            }
            .centre {
                text-align: center;
            }
            @media (prefers-color-scheme: dark) {
            body {
                background-color: #121212;
                color: rgba(255, 255, 255, .8)
            }
            blockquote::before { color: rgba(127, 127, 255, .6); }
            a { color: #3391ff; }
            a:visited { color: #cc91ff; }
            }
            @media (min-width: 1500px) {
                main {
                    display: block;
                    margin-left: 194px;
                }
            }
            .top-app-bar {
                background-color: #4f5d73;
            }
            `
})

app.listen(50089)
