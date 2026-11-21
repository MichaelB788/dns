# DNS Resolver

A client written in C that accepts one hostname as a command-line argument,
sends an A-record query in the Internet class to 8.8.8.8:53 over an IPv4 UDP
socket, and displays decoded A and CNAME results.

# Prerequisites

Any Linux distribution should suffice, e.g., Debian/Ubuntu, Fedora, or Arch.

The project assumes the following packages are installed:
- CMake (version 3.5+)
- Makefile
- GCC

You can install them through your distribution's package manager like so:

Debian / Ubuntu:

```bash
sudo apt install cmake make gcc
```

Fedora:

```bash
sudo dnf install cmake make gcc
```

Arch:

```bash
sudo pacman -S cmake make gcc
```

# Building

Configure the project in `Release` mode like so:

```bash
cmake --preset Release
```

Then, build/rebuild the project using the following:

```bash
cmake --build build
```

You can then find the project executable `client` under `build/Release/`

# Running

Simply provide the target hostname as an argument to `client`. For example:

```bash
./client www.example.com
```
