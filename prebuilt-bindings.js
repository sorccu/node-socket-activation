const prebuild = require('prebuilt-bindings')

module.exports = prebuild({
  context: __dirname,
  bindings: ({
    darwin: [{
      name: 'socket_activation'
    }]
  })[process.platform] || []
})
