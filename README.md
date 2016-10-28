# node-socket-activation

[![npm](https://img.shields.io/npm/v/socket-activation.svg)](https://www.npmjs.com/package/socket-activation)

**node-socket-activation** is a [socket activation](http://0pointer.de/blog/projects/socket-activation.html) library for [Node.js](https://nodejs.org/).

## Supported systems

| Service management framework    | OS                      | Supported?          |
|---------------------------------|-------------------------|---------------------|
| [launchd][launchd-sockets]      | macOS 10.9 or later     | :thumbsup: :bowtie: |
| [systemd][systemd-socket]       | Compatible Linux distro | :thumbsup: :bowtie: |

[launchd-sockets]: https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html#//apple_ref/doc/uid/10000172i-SW7-SW4
[systemd-socket]: https://www.freedesktop.org/software/systemd/man/systemd.socket.html

## Usage

### How your app should look like

#### `app.js`

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

See the [examples](examples/) directory for more examples incl. supporting both socket activation and direct execution.

### Launchd configuration

Normally you'd put this in `~/Library/LaunchAgents`.

#### `org.example.app.plist`

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
    <dict>
        <key>Label</key>
        <string>org.example.app</string>
        <key>ProgramArguments</key>
        <array>
            <string>/usr/local/bin/node</string>
            <string>ABSOLUTE_PATH_TO_YOUR_JS_FILE</string>
        </array>
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
        <string>/tmp/org.example.app.err</string>
        <key>StandardOutPath</key>
        <string>/tmp/org.example.app.out</string>
    </dict>
</plist>
```

There's no need to set `RunAtLoad`, launchd will start your app on the first request.

Enable and start listening by running:

```sh
launchctl load ~/Library/LaunchAgents/org.example.app.plist
```

### Systemd configuration

Normally you'd put these in `/etc/systemd/system`.

#### `app.service`

```systemd
[Unit]
RefuseManualStart=true

[Service]
Restart=always
ExecStart=/usr/local/bin/node ABSOLUTE_PATH_TO_YOUR_JS_FILE
```

#### `app.socket`

```systemd
[Socket]
ListenStream=127.0.0.1:3000
NoDelay=true
FileDescriptorName=app

[Install]
WantedBy=sockets.target
```

Enable and start listening by running:

```sh
systemctl enable app.socket
systemctl start app.socket
```

There's no need to enable the service, systemd will start it on the first request.

## API

### `.collect(String name) → [Number]`

Returns the list of fds the system has managed for us. This operation consumes the list and can therefore only be performed once per launch. Subsequent calls *with the same `name`* throw an `Error`. It's your responsibility not to do that.

The `name` is case sensitive and must match the name of the socket. Under systemd, the setting is [FileDescriptorName=](https://www.freedesktop.org/software/systemd/man/systemd.socket.html#FileDescriptorName=). Under launchd, the `<key>...</key>` of the socket definition defines the name.

Note that the method returns an `Array` of fds. For example, there may be more than one fd if you select the "IPv4v6" SockFamily in launchd.plist which listens to both IPv4 and IPv6, or if several [ListenStream](https://www.freedesktop.org/software/systemd/man/systemd.socket.html#ListenStream=) are given in [systemd.socket](https://www.freedesktop.org/software/systemd/man/systemd.socket.html). Up to you to decide if you want to error out when given multiple fds or setup multiple listeners.

The API is the same for all supported service managers.

This method throws an `Error` if:

* The process was not launched by the service management framework (or with `launchd`, if there's no matching socket).
    - Error `.code` is `'ESRCH'`
* There is no matching socket.
    - Error `.code` is `'ENOENT'`
* The fd list has already been collected previously.
    - Error `.code` is `'EALREADY'`
* An internal system error occurred.

## License

See [LICENSE](LICENSE).
