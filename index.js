switch (process.platform) {
case 'darwin':
  module.exports = require('bindings')('socket_activation.node')
  break
default:
  module.exports = require('./src/systemd')
  break
}
