import koa from 'koa'
import Router from '@koa/router'
import bodyparser from 'koa-body'
import { createInterface } from 'readline'
import { spawn } from 'child_process'
import { v4 as uuid } from 'uuid'
import serve from "koa-static";

const app = new koa()
const router = new Router()

app.use(serve('assets'))

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
<link rel="stylesheet">
<meta http-equiv="X-UA-Compatible">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
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
    if (!ctx.cookies.get('sessionID') || userMap[ctx.cookies.get('username')] === undefined) {
        ctx.body = `
<html>
<head>
<meta charset="UTF-8">
<link rel="stylesheet">
<meta http-equiv="X-UA-Compatible">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
</head>
<body>
    <main><header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
          <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-secondary">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
        </ul>
        <form class="col-12 col-lg-auto mb-3 mb-lg-0 me-lg-3" role="search">
          <input type="search" class="form-control form-control-dark text-white bg-dark" placeholder="Search..." aria-label="Search">
        </form>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
    </main>
    <script>
    const loginButton = document.getElementById('login')
    loginButton.addEventListener('click', () => { location.href = '/login' })
</script>
</body>
</html>`
        return
    }

    ctx.body = `
<html>
<head>
<meta charset="UTF-8">
<link rel="stylesheet">
<meta http-equiv="X-UA-Compatible">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
<style>
    .login-form {
        width: 50%;
    }

    #loginbutton {
        margin-top: 40px;
    }
</style>
</head>
<body>
    <main><header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
  <div class="login-form position-absolute top-50 start-50 translate-middle">
  <form action="/buy" method="post">
        <p><input name="trainID" class="h3 mb-3 fw-normal" placeholder="trainID"></p>
        <p><input name="startStation" class="h3 mb-3 fw-normal" placeholder="From where"></p>
        <p><input name="endStation" class="h3 mb-3 fw-normal" placeholder="To where"></p>
        <p><input name="number" class="h3 mb-3 fw-normal" placeholder="seat number"></p>
        <p><input name="month" class="h3 mb-3 fw-normal" placeholder="mm">-<input name="day" class="h3 mb-3 fw-normal" placeholder="dd"></p>
        <div class="form-check">
            <input class="form-check-input" type="checkbox" value="" name="queue" checked="checked">
            <label class="form-check-label" for="flexCheckChecked">Accept waiting until the order is available if there is no enough seats.</label>
        </div>
        
    <div id="loginbutton">
      <button type="submit" class="w-100 btn btn-lg btn-primary">Buy</button>
      </div>
    </form>
  </div>
    </main>
</body>
</html>
<html>`
})

router.get('/login', async ctx => {
    ctx.body = `
    <html>
<head>
<meta charset="UTF-8">
<link rel="stylesheet">
<meta http-equiv="X-UA-Compatible">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
<style>
    .login-form {
        width: 50%;
    }

    #loginbutton {
        margin-top: 40px;
    }
</style>
</head>
<body>
    <main><header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-secondary">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
        </ul>
        <form class="col-12 col-lg-auto mb-3 mb-lg-0 me-lg-3" role="search">
          <input type="search" class="form-control form-control-dark text-white bg-dark" placeholder="Search..." aria-label="Search">
        </form>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
  <div class="login-form position-absolute top-50 start-50 translate-middle">
    <form action="/login" method="post">
    <div class="mb-3">
    <label for="username" class="h3 mb-3 fw-normal">Username</label>
      <input class="form-control" name="username" type="text" placeholder="username" aria-label="default input example">
    </div class="mb-3">
    <div>
    <label for="password" class="h3 mb-3 fw-normal">Password</label>
      <input type="password" name="password" class="form-control" placeholder="password">
      </div>
    <div id="loginbutton">
      <button type="submit" class="w-100 btn btn-lg btn-primary">Login</button>
      </div>
    </form>
  </div>
    </main>
    <script>
    const loginButton = document.getElementById('login')
    const signupButton = document.getElementById('signup')
    loginButton.addEventListener('click', () => { location.href = '/login' })
    signupButton.addEventListener('click', () => { location.href = '/sign-up'})
</script>
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
    <link rel="stylesheet">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
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
    <link rel="stylesheet">
    <meta http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
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
<link rel="stylesheet">
<meta http-equiv="X-UA-Compatible">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
<style>
    .login-form {
        width: 50%;
    }

    #loginbutton {
        margin-top: 40px;
    }
</style>
</head>
<body>
    <main><header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <form class="col-12 col-lg-auto mb-3 mb-lg-0 me-lg-3" role="search">
          <input type="search" class="form-control form-control-dark text-white bg-dark" placeholder="Search..." aria-label="Search">
        </form>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
  <div class="container">
  
    <label for="username" class="h3 mb-3 fw-normal">Ticket</label>
            <form action="/query-ticket" method="post">
                <p>Destination: <input name="destination" class="h3 mb-3 fw-normal"></p>
                <p>Departure: <input name="departure" class="h3 mb-3 fw-normal"></p>
                <p>Date: <input name="month" class="h3 mb-3 fw-normal">-<input name="date" class="h3 mb-3 fw-normal"></p>
                <select name="sort" id="modify">
                    <option value="time">-time by default-</option>
                    <option value="time">time</option>
                    <option value="cost">cost</option>
                </select>
    <div id="loginbutton">
      <button type="submit" class="w-100 btn btn-lg btn-primary">Search</button>
      </div>
            </form>
            </div>
            <div class="container">

    <label for="username" class="h3 mb-3 fw-normal">Transfer</label>
                <form action="/query-transfer" method="post">
                <p>Destination: <input name="destination" class="h3 mb-3 fw-normal"></p>
                <p>Departure: <input name="departure" class="h3 mb-3 fw-normal"></p>
                <p>Date: <input name="month" class="h3 mb-3 fw-normal">-<input name="date" class="h3 mb-3 fw-normal"></p>
                <select name="sort" id="modify">
                    <option value="time">-time by default-</option>
                    <option value="time">time</option>
                    <option value="cost">cost</option>
                </select>
    <div id="loginbutton">
      <button type="submit" class="w-100 btn btn-lg btn-primary">Search</button>
      </div>
            </form>
            </div>
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
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>`

    for (let i = 0; i < num; ++i) {
        const [ trainID, from, leavingDate, leavingTime, arrow, to, arrivalDate, arrivalTime, price, seats ] = (await getline()).split(' ')
        if (seats === '0' || seats === '1') {
            ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seat</p>
    <p>${from} -> ${to}</p>
    <p>${leavingTime} ${arrivalTime}</p>
    <p>${leavingDate} ${arrivalDate}</p>    
</div>`
        } else {
            ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seats</p>
    <p>${from} -> ${to}</p>
    <p>${leavingTime} ${arrivalTime}</p>
    <p>${leavingDate} ${arrivalDate}</p> 
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
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
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
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
            `

        for (let i = 0; i < 2; ++i) {
            const [ trainID, from, leavingDate, leavingTime, arrow, to, arrivalDate, arrivalTime, price, seats ] = (await getline()).split(' ')
            if (seats === '0' || seats === '1') {
                ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seat</p>
    <p>${from} -> ${to}</p>
    <p>${leavingTime} ${arrivalTime}</p>
    <p>${leavingDate} ${arrivalDate}</p>    
</div>`
            } else {
                ctx.body += `
<div>
    <p>${trainID} ￥${price} remain ${seats} seats</p>
    <p>${from} -> ${to}</p>
    <p>${leavingTime} ${arrivalTime}</p>
    <p>${leavingDate} ${arrivalDate}</p> 
</div>`
            }
        }
        ctx.body += `
        </main>
    </body>
</html>`
    }
})

router.get('/unauthorized', async ctx => {
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
            <p>Unauthorized</p>
        </main>
    </body>`
})

router.get('/profile', async ctx => {
    // example: username name email@example.com 10(privilege)
    const msg = await getProfile(ctx.cookies.get('username'))
    if (msg.split(' ')[1] === 'failed:') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
    <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
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
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
            <form action="/profile" method="post">
                <p>username: ${username}</p>
                <p>name: <input name="name" value="${name}"></p>
                <p>mail address: <input name="mailAddress" value="${email}"></p>
                <p>privilege: <input name="privilege" value="${privilege}"></p>
                <button type="submit">modify</button>
        </main>
    </body>
</html>`
})

router.post('/profile', async ctx => {
    if (ctx.cookies.get('sessionID') !== userMap[ctx.cookies.get('username')]) {
        ctx.redirect('/unauthorized')
    }
    putline(`[${timeStamp}] modify_profile -c ${ctx.cookies.get('username')} -u ${ctx.cookies.get('username')} -n ${ctx.request.body.name} -m ${ctx.request.body.mailAddress} -g ${ctx.request.body.privilege}`)
    const msg = dismissTimeStamp(await getline())
    if (msg.split(' ')[1] === 'failed:') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>`
    }
    ctx.redirect('/profile')
})

router.get('/orders', async ctx => {
    if (ctx.cookies.get('sessionID') !== userMap[ctx.cookies.get('username')]) {
        ctx.redirect('/unauthorized')
    }
    putline(`[${timeStamp}] query_order -u ${ctx.cookies.get('username')}`)
    const line = await getline()
    if (line.split(' ')[2] === 'failed:') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>Query failed! You are not logged in!</p>
        </main>
    </body>
</html>`
        return
    }
    const num = line.split(' ')[1]

    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
<header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>`
    for (let i = 1; i <= num; ++i) {
        const [ statusRaw, trainID, from, leavingDate, leavingTime, arrow, to, arrivalDate, arrivalTime, price, num ] = (await getline()).split(' ')
        const status = statusRaw.slice(1, -1)
        if (num === '0' || num === '1') {
            ctx.body += `
<div>
    <p>${status} ${trainID} ${from} ${leavingDate} ${leavingTime} -> ${to} ${arrivalDate} ${arrivalTime} ￥${price} ${num} seat</p>
    <form action="/refund" method="post">
        <input type="hidden" value="${i}">
        <button type="submit" class="btn btn-lg btn-primary">Refund</button>
    </form>
</div>`
        } else {
            ctx.body += `
<div>
    <p>${status} ${trainID} ${from} ${leavingDate} ${leavingTime} -> ${to} ${arrivalDate} ${arrivalTime} ￥${price} ${num} seats</p>
    <form action="/refund" method="post">
        <input type="hidden" value="${i}">
        <button type="submit" class="btn btn-lg btn-primary">Refund</button>
    </form>
</div>`
        }
    }
    ctx.body += `
        </main>
    </body>
</html>`
})

router.post('/refund', async ctx => {
    if (ctx.cookies.get('sessionID') !== userMap[ctx.cookies.get('username')]) {
        ctx.redirect('/unauthorized')
    }
    const index = ctx.request.body.index
    putline(`[${timeStamp}] refund_ticket -u ${ctx.cookies.get('username')} -i ${index}`)
    const msg = dismissTimeStamp(await getline())
    if (msg.split(' ')[1] === 'failed:') {
        ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
        <p>${msg}</p>
        </main>
    </body>
</html>`
        return
    }
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
        <p>${msg}</p>
        </main>
    </body>
</html>`
})

router.get('/add-user', async ctx => {
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
          <button type="button" class="btn btn-outline-light me-2" id="login">Login</button>
        </div>
      </div>
    </div>
  </header>
            <p>Add User</p>
            <form action="/add-user" method="post">
                <p>username: <input name="username" class="h3 mb-3 fw-normal"></p>
                <p>name: <input name="name"  class="h3 mb-3 fw-normal"></p>
                <p>mail address: <input name="mailAddress" class="h3 mb-3 fw-normal"></p>
                <p>privilege: <input name="privilege" class="h3 mb-3 fw-normal"></p>
                <p>password: <input name="password" type="password" class="h3 mb-3 fw-normal"></p>
                <button type="submit" class="btn btn-outline-light me-2">Add</button>
        </main>
    </body>
</html>`
})

router.post('/add-user', async ctx => {
    if (ctx.cookies.get('sessionID') !== userMap[ctx.cookies.get('username')]) {
        ctx.redirect('/unauthorized')
    }
    putline(`[${timeStamp}] add_user -c ${ctx.request.body.username} -u ${ctx.request.body.username} -n ${ctx.request.body.name} -e ${ctx.request.body.email} -p ${ctx.request.body.privilege} -p ${ctx.request.body.password}`)
    const msg = dismissTimeStamp(await getline())
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>
</html>`
})

router.get('/modify', async ctx => {
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
            <p>Modify User</p>
            <form action="/modify" method="post">
                <p>username: <input name="username"></p>
                <p>password: <input name="password" type="password"></p>
                <p>mail address: <input name="mailAddress"></p>
                <p>privilege: <input name="privilege"></p>
                <button type="submit">Modify</button>
        </main>
    </body>
</html>`
})

router.post('/modify', async ctx => {
    if (ctx.cookies.get('sessionID') !== userMap[ctx.cookies.get('username')]) {
        ctx.redirect('/unauthorized')
    }
    if (ctx.request.body.privilege !== '') {
        putline(`[${timeStamp}] modify_user -c ${ctx.cookies.get('username')} -u ${ctx.request.body.username} -g ${ctx.request.body.privilege}`)
        const msg = dismissTimeStamp(await getline)
        if (msg.split(' ')[1] === 'failed:') {
            ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>
</html>`
            return
        }
    }
    if (ctx.request.body.mailAddress !== '') {
        putline(`[${timeStamp}] modify_user -c ${ctx.cookies.get('username')} -u ${ctx.request.body.username} -m ${ctx.request.body.mailAddress}`)
        const msg = dismissTimeStamp(await getline)
        if (msg.split(' ')[1] === 'failed:') {
            ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>
</html>`
            return
        }
    }
    if (ctx.request.body.password !== '') {
        putline(`[${timeStamp}] modify_user -c ${ctx.cookies.get('username')} -u ${ctx.request.body.username} -p ${ctx.request.body.password}`)
        const msg = dismissTimeStamp(await getline)
        if (msg.split(' ')[1] === 'failed:') {
            ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>
</html>`
            return
        }
    }
    if (ctx.request.body.name !== '') {
        putline(`[${timeStamp}] modify_user -c ${ctx.cookies.get('username')} -u ${ctx.request.body.username} -n ${ctx.request.body.name}`)
        const msg = dismissTimeStamp(await getline)
        if (msg.split(' ')[1] === 'failed:') {
            ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>${msg}</p>
        </main>
    </body>
</html>`
            return
        }
    }
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
        <p>Modify User Success</p>
        </main>
    </body>
</html>`
})

router.get('/add-train', async ctx => {
    //TODO
    ctx.body = `
<html>
    <head>
        <meta charset="UTF-8">
        <link rel="stylesheet">
        <meta http-equiv="X-UA-Compatible">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
    </head>
    <body>
        <main>
            <p>Add Train</p>
            <form action="/add-train" method="post">
                <p>train number: <input name="trainNumber"></p>
                <p>train name: <input name="trainName"></p>
                <p>start station: <input name="startStation"></p>
                <p>end station: <input name="endStation"></p>
                <p>start time: <input name="startTime"></p>
                <p>end time: <input name="endTime"></p>
                <p>price: <input name="price"></p>
                <button type="submit">Add</button>
        </main>
    </body>
</html>`
})

router.post('/query-train', async ctx => {
    ++timeStamp
    putline(`[${timeStamp}] query-train -u ${ctx.request.body.trainID} -t ${ctx.request.body.month}-${ctx.request.body.day}`)
    const msg = dismissTimeStamp(await getline())
    if (msg.split(' ')[1] === 'failed:') {
        ctx.body = `
            <html>
            <head>
                <meta charset="UTF-8">
                <link rel="stylesheet">
                <meta http-equiv="X-UA-Compatible">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
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
            <link rel="stylesheet">
            <meta http-equiv="X-UA-Compatible">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
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

router.post('/buy', async ctx => {
    if (ctx.cookies.get('sessionID') !== userMap[ctx.cookies.get('username')]) {
        ctx.redirect('/unauthorized')
    }
    ++timeStamp
    putline(`[${timeStamp}] buy_ticket -u ${ctx.cookies.get('username')} -i ${ctx.request.body.trainID} -f ${ctx.request.body.startStation} -t ${ctx.request.body.endStation} -d ${ctx.request.body.month}-${ctx.request.body.day} -n ${ctx.request.body.number} -q ${ctx.request.body.queue === 'checked'}`)
    const msg = dismissTimeStamp(await getline())

    ctx.body = `
<html>
<head>
<meta charset="UTF-8">
<link rel="stylesheet">
<meta http-equiv="X-UA-Compatible">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Ticket System</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-0evHe/X+R7YkIZDRvuzKMRqM+OrBnVFBL6DOitfPri4tjfHxaWutUpFmBp4vmVor" crossorigin="anonymous">
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0-beta1/dist/js/bootstrap.bundle.min.js" integrity="sha384-pprn3073KE6tl6bjs2QrFaJGz5/SUsLqktiwsUTF55Jfv3qYSDhgCecCxMW52nD2" crossorigin="anonymous"></script>
<style>
    .login-form {
        width: 50%;
    }

    #loginbutton {
        margin-top: 40px;
    }
</style>
</head>
<body>
    <main><header class="p-3 bg-dark text-white">
    <div class="container">
      <div class="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
        <a href="/" class="d-flex align-items-center mb-2 mb-lg-0 text-white text-decoration-none">
        <img class="bi me-2" width="40" height="32" role="img" aria-label="Bootstrap" src="/icon.svg"></img>
        </a>
        <ul class="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
          <li><a href="/" class="nav-link px-2 text-white">Home</a></li>
          <li><a href="/query-plan" class="nav-link px-2 text-white">Travel Plan</a></li>
          <li><a href="/profile" class="nav-link px-2 text-white">Profile</a></li>
          <li><a href="/orders" class="nav-link px-2 text-white">Orders</a></li>
        </ul>
        <div class="text-end">
        ${ctx.cookies.get('username')}
        </div>
      </div>
    </div>
  </header>
    <div class="container">
    <p>${msg}</p>
    </div>
    </main>
</body>
</html>
<html>`
return
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
