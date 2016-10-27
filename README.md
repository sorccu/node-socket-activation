# node-socket-activation

[![npm](https://img.shields.io/npm/v/socket-activation.svg)](https://www.npmjs.com/package/socket-activation)

**node-socket-activation** is a [socket activation](http://0pointer.de/blog/projects/socket-activation.html) library for [Node.js](https://nodejs.org/).

## Supported systems

| Service management framework    | OS                      | Supported                |
|---------------------------------|-------------------------|--------------------------|
| [launchd][launchd-sockets]      | macOS 10.9 or later     | :thumbsup: :bowtie:      |
| [systemd][systemd-socket]       | Compatible Linux distro | :thumbsdown: :persevere: |

[launchd-sockets]: https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html#//apple_ref/doc/uid/10000172i-SW7-SW4
[systemd-socket]: https://www.freedesktop.org/software/systemd/man/systemd.socket.html

## Usage

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
    <dict>
        <key>Label</key>
        <string>app.job</string>
        <key>ProgramArguments</key>
        <array>
            <string>/usr/local/bin/node</string>
            <string>ABSOLUTE_PATH_TO_YOUR_JS_FILE</string>
        </array>
        <key>RunAtLoad</key>
        <true/>
        <key>Sockets</key>
        <dict>
            <key>app</key>
            <array>
                <dict>
                    <key>SockFamily</key>
                    <string>IPv4</string>
                    <key>SockServiceName</key>
                    <string>3000</string>
                </dict>
            </array>
        </dict>
        <key>StandardErrorPath</key>
        <string>/tmp/app.job.err</string>
        <key>StandardOutPath</key>
        <string>/tmp/app.job.out</string>
    </dict>
</plist>
```

```javascript
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
```

## API

### `.collect(String name) → [Number]`

Returns the list of fds the system has managed for us. This operation consumes the list and can therefore only be performed once per launch. Subsequent calls will throw an `Error`.

This method throws an `Error` if:

* The process was not launched by the service management framework.
    - Error `.code` is `'ESRCH'`
* There is no matching socket.
    - Error `.code` is `'ENOENT'`
* The fd list has already been collected previously.
    - Error `.code` is `'EALREADY'`
* An internal system error occured.

## License

See [LICENSE](LICENSE).
