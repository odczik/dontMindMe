const express = require('express')
const app = express()
const path = require('path');
const port = 3000
let count = 0

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '/index.html'))
})
app.get('/count', (req, res) => {
    console.log(count)
    res.send({number: count})
})
app.post('/count', (req, res) => {
    count++
    res.sendStatus(200)
})

app.listen(port, () => {
    console.log(`Example app listening on port ${port}`)
})