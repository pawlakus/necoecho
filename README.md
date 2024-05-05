# TCP Echo Server

This is a simple TCP echo server implemented using the
[`neco`](https://github.com/tidwall/neco.git) library, which simplifies
concurrent I/O and network programming. The server echoes back every string
received from connected clients.

## Features

- coroutines for handling of multiple clients (no fork or thread for every
  client)
- Automatically disconnects stale clients after a hardcoded timeout period
  (5 seconds)
- Protects against clients sending excessive data by enforcing a maximum data
  limit
- Every network call use `neco_read_dl()` and `neco_write_dl()` (with deadlines)

## Usage

Can be used as a simple TCP target for your Load Balancer TCP checks.

You must specify listening interface and port number. There is no default value:

```
$ ./necoecho 0.0.0.0 9000
listening at 0.0.0.0:9000
```

## Compilation

Preparation:

```
# Debian12/Ubuntu
apt-get -y install build-essential git
git clone --recursive https://github.com/pawlakus/necoecho.git
```

MacOS/darwin:

```
make
```

Alpine Linux 3.18, muslc statically linked
```
export CPPFLAGS='-D_GNU_SOURCE -static'
make && strip necoecho
```

Ubuntu 20.04
```
export CPPFLAGS='-Wl,--no-as-needed -ldl'
make && strip necoecho
```

Ubuntu 22.04
```
make && strip necoecho
```

Debian 12
```
make && strip necoecho
```
