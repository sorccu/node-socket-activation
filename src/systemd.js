const SD_LISTEN_FDS_START = 3

const collected = Object.create(null)

module.exports.collect = (name) => {
  // Only allow a specific name to be collected once to match launchd. Helps
  // prevent surprises when moving between systemd.
  if (collected[name]) {
    const err = new Error('The specified socket has already been activated')
    err.code = 'EALREADY'
    throw err
  }

  collected[name] = 1

  // Environment variables provided by systemd.
  const rawpid = process.env.LISTEN_PID
  const rawfds = process.env.LISTEN_FDS
  const rawfdnames = process.env.LISTEN_FDNAMES

  // Does it look like we're running under systemd?
  if (!rawpid || !rawfds) {
    const err = new Error('The calling process is not managed by systemd')
    err.code = 'ESRCH'
    throw err
  }

  const pid = +rawpid

  // Are the fds for us?
  if (pid !== process.pid) {
    throw new Error(`Cannot collect fds meant for pid ${pid} in pid ${process.pid}`)
  }

  const numfds = +rawfds
  const fdnames = rawfdnames.split(':')

  if (numfds !== fdnames.length) {
    throw new Error('Mismatching number of fd names and fds')
  }

  const fds = []

  fdnames.forEach((fdname, i) => {
    if (fdname === name) {
      fds.push(SD_LISTEN_FDS_START + i)
    }
  })

  if (fds.length === 0) {
    const err = new Error('The socket name specified does not exist')
    err.code = 'ENOENT'
    throw err
  }

  return fds
}
