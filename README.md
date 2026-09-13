*This project was created by hoysong as part of the 42 common core curriculum.*

[English](./README.md) | [한국어](./README.ko.md)

![ft_irc](./image.png)

# 1. Project Overview

ft_irc is a lightweight IRC (Internet Relay Chat) server written in C++98 as part of the 42 common core curriculum. The server handles multiple simultaneous client connections through a single `epoll`-based event loop.

It implements a hybrid subset of **RFC 1459** and **RFC 2812**, including user authentication, nickname and username registration, channel management, private messaging, and operator privileges. **irssi** is the reference client, while basic tools such as **netcat** (`nc`) and **telnet** can also connect to the server.

Server-to-server communication is not implemented. The project focuses exclusively on the client-server relationship defined by the RFC specifications.

# 2. Implementation Ownership

I independently designed and implemented the server in this repository. My work covers the event loop, socket and client lifecycle, receive buffering, IRC message parsing and dispatch, channel management, protocol command handlers, response generation, and integration bug fixes.

# 3. Request Processing Flow

```text
Client connection
    ↓
ListenSocket accepts the socket
    ↓
ClientManager creates and tracks the Client
    ↓
EpollManager registers and waits for socket events
    ↓
Client appends received bytes to its own buffer
    ↓
Client::popLine() extracts complete CRLF-delimited messages
    ↓
IRCServer parses and dispatches the command
    ↓
The command handler updates client/channel state and sends a response
```

A TCP receive call does not necessarily return one complete IRC message. Each `Client` therefore keeps its own receive buffer. The server processes only complete `\r\n`-terminated messages and retains incomplete data until the next receive event. The same loop can also extract several messages received together.

# 4. Component Responsibilities

| Component | Responsibility |
| --- | --- |
| `ListenSocket` | Creates, binds, listens on, and accepts the server socket |
| `EpollManager` | Owns the `epoll` instance and manages event registration, deletion, and waiting |
| `Client` | Stores connection and registration state, receive buffer, and joined-channel information |
| `ClientManager` | Creates, finds, and removes connected clients |
| `Channel` / `ChannelManager` | Manages channel membership, operators, invitations, topics, and channel modes |
| `IRCServer` | Runs the event loop, parses complete messages, dispatches commands, and coordinates managers |
| `Msg` | Builds IRC protocol responses and numeric replies |

# 5. Key Design Decisions

- **Single `epoll` event loop:** all listen and client sockets are handled without creating a process per connection.
- **Per-client receive buffers:** split and consecutive TCP receives are converted into complete IRC messages at the CRLF boundary.
- **Function-pointer dispatch map:** command strings such as `PASS`, `NICK`, `JOIN`, and `PRIVMSG` are mapped to `IRCServer` member handlers.
- **Separated state managers:** client and channel state are managed independently from socket event handling and message construction.

# 6. Features

See the [IRC Command Guide](./docs/COMMANDS.md) for a complete feature overview and detailed usage of each command.

# 7. Technical Characteristics and Constraints

- Written in **C++98**
- Handles multiple clients simultaneously
- Uses a **single `epoll` instance** for all I/O multiplexing
- Communicates over **TCP/IP (IPv4)**
- Does not fork
- Uses **irssi** as the reference client implementation; interaction with other IRC clients is not guaranteed

# 8. Usage

## 8.1. Prerequisites

### 8.1.1. Build and Runtime Environment

- A **Linux** environment with `epoll` support
- A C++ compiler with C++98 support (e.g. `c++`, `g++`, or `clang++`)
- GNU Make

### 8.1.2. Connection and Testing Tools

- **irssi** — Required to connect and verify behavior with the reference client
- **netcat** (`nc`) — Optional tool for sending raw IRC commands

## 8.2. Building

Clone the repository and compile the project:

```bash
git clone <repository-url>
cd ft_irc
make
```

Other Make targets:

| Target | Description |
| --- | --- |
| `make` | Compile the project and produce the `ircserv` executable |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and the executable |
| `make re` | Run `fclean`, then run `make` |

## 8.3. Running the Server

```bash
./ircserv <port> <password>
```

- `<port>` — Port on which the server listens for incoming client connections
- `<password>` — Connection password that clients must provide

**Example:**

```bash
./ircserv 6667 mypassword
```

## 8.4. Connecting with the IRC Client (irssi)

### 8.4.1. Method 1

1. Start irssi:

```bash
irssi
```

2. Enter the following command in irssi:

```irssi
/connect 127.0.0.1 6667 mypassword
```

### 8.4.2. Method 2

Start irssi with connection options:

```bash
irssi -c 127.0.0.1 -p 6667 -n mynick --password="mypasswd"
```

## 8.5. Connecting with Netcat

1. Start netcat in a terminal:

```bash
nc -C 127.0.0.1 6667
```

2. Authenticate manually:

```text
PASS mypassword
NICK mynick
USER myuser 0 * :My Real Name
```

> **Note:** The `-C` option is used because IRC messages end with `\r\n` as defined by the RFC specifications.

## 8.6. Notes for Trying the Project

- Commands listed in the [Feature Overview](./docs/COMMANDS.md#1-feature-overview) are case-insensitive.
- Commands other than registration commands are rejected until the [Connection and Registration](./docs/COMMANDS.md#21-connection-and-registration) process is complete.
- netcat and irssi are different programs. irssi provides its own client-side commands, which may not be available in netcat.

# 9. Resources

## 9.1. References

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol Documentation](https://modern.ircdocs.horse)
- [IRC/2 Numeric List](https://www.alien.net.au/irc/irc2numerics.html)

## 9.2. AI Usage

AI tools were used during the development of this project in the following areas:

- **RFC interpretation** — Clarifying ambiguous sections of RFC 1459 and RFC 2812, particularly edge cases in command parsing and expected server behavior
- **Error code and response mapping** — Identifying the correct numeric replies for each command and organizing per-command error and response tables
- **README authoring** — Assisting with the structure and writing of this document
- **Code review ideation** — Generating code review suggestions and identifying potential issues in protocol-handling logic
- **Repetitive refactoring** — Handling mechanical refactoring tasks such as updating all call sites after function signature changes

---
