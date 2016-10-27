const net = require('net')

const sockets = require('socket-activation')

// There may be more than one fd, e.g. if you select the "IPv4v6"
// SockFamily which listens to both IPv4 and IPv6. Up to you to
// decide if you want to error out or setup multiple servers as
// follows.
for (const fd of sockets.collect('app')) {
  const server = net.createServer(conn => {
    conn.end('hello!\n')
  })

  server.on('error', err => {
    throw err
  })

  server.listen({ fd }, () => {
    const addr = server.address()
    console.log('Listening on', addr)
  })
}
