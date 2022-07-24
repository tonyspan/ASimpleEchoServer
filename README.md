# A Simple Echo Server [![License](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/tonyspan/ASimpleEchoServer/blob/master/LICENSE)

This project is a simple library/interface for cross-platform socket programming. A server will echo/broadcast back to all connected clients the received message. To make things simpler, all clients receive a list of running servers as command line arguments and should choose to which to connect. If the connection is drooped with a server, the client(s) will connect to another.

## Notes
* Tested on Ubuntu 20.04 and Windows 10 (Visual Studio 2019)
* Some unnoticed memory leaks possibly
* Not sure if any race conditions occur