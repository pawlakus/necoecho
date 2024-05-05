# TCP Echo Server

This is a simple TCP echo server implemented using the
[`neco`](https://github.com/tidwall/neco.git) library, which simplifies
concurrent I/O and network programming. The server echoes back every string
received from connected clients.

## Features

- event-driven architecture for efficient handling of multiple clients (no fork
  or thread for every client)
- Automatically disconnects stale clients after a specified timeout period
  (default: 5 seconds)
- Protects against clients sending excessive data by enforcing a maximum data
  limit
- Optimized for low memory usage to handle a large number of concurrent
  connections
- Every `read()` and `write()` with deadlines

## Usage

Can be used as a simple TCP target for your Load Balancer TCP checks.

You must specify listening interface and port number. There is no default value:

```
$ ./echo 0.0.0.0 9000
listening at 0.0.0.0:9000
```
