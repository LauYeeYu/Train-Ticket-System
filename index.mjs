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
            }`
})

router.post('/', async ctx => {
    // Logical stuff
})

app.listen(50089)
