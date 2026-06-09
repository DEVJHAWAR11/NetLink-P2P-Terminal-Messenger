# NetLink — P2P Terminal Messenger

NetLink is a terminal-based messaging system written in pure C++ where two computers can send and receive messages to each other in real-time over a TCP connection. It relies entirely on standard system headers, utilizing raw TCP sockets and POSIX threads to achieve full-duplex communication without blocking.

## Architecture

```text
SERVER                                     CLIENT
------                                     ------
socket()                                   socket()
bind() to port 8080
listen()
                                           connect() to server IP:8080
accept() ←-------- TCP connection --------→
spawn receive_thread                       spawn receive_thread

[MAIN THREAD]                              [MAIN THREAD]
loop: read input → send()                  loop: read input → send()

[RECEIVE THREAD]                           [RECEIVE THREAD]
loop: recv() → print                       loop: recv() → print
```

## How to Compile and Run

1. Open your Linux terminal (or WSL).
2. Compile the project using `make`:
   ```bash
   make
   ```
3. Run the server first in one terminal:
   ```bash
   ./server
   ```
4. Run the client in another terminal (connecting to localhost):
   ```bash
   ./client 127.0.0.1
   ```

*Note: You can connect over a real network by putting the server's IP instead of 127.0.0.1.*

## The 7 Socket Functions Used

- `socket()` — Creates a socket endpoint for communication (like getting a phone).
- `bind()` — Attaches the socket to a specific port (like registering your phone number).
- `listen()` — Waits for incoming connections (like turning the ringer on).
- `accept()` — Accepts one client connection and creates a dedicated socket for them.
- `connect()` — Client dials the server to establish a connection.
- `send()` — Sends data through the connection (like talking).
- `recv()` — Receives data from the connection (like listening).

## What I Learned

Building this project taught me how TCP connections are established at the lowest level, how the 3-way handshake happens automatically inside `connect()`, and how data travels between machines. Most importantly, I learned how to use POSIX threads to solve the blocking I/O problem, allowing both peers to send and receive messages simultaneously without freezing the program.
