const net = require('net')

const sockets = require('socket-activation')

// Set up defaults for when the app is run directly without socket activation.
let listeners = [
  (server, callback) => {
    server.listen(3000, '127.0.0.1', callback)
  }
]

// Check if socket activation is used.
try {
  listeners = sockets.collect('app').map(fd => (server, callback) => {
    server.listen({ fd }, callback)
  })
  console.log('Using socket activation')
} catch (err) {
  if (err.code !== 'ESRCH') {
    throw err
  }
  console.log('Not using socket activation')
}

for (const listener of listeners) {
  const server = net.createServer(conn => {
    conn.end('hello!\n')
  })

  server.on('error', err => {
    throw err
  })

  listener(server, () => {
    const addr = server.address()
    console.log('Listening on', addr)
  })
}
