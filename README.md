*This project was created by hoysong as part of the 42 common core curriculum.*

[English](./README.md) | [한국어](./README.ko.md)

![ft_irc](./image.png)

# 1. Project Overview

ft_irc is a lightweight IRC (Internet Relay Chat) server written in C++98 as part of the 42 common core curriculum. The goal of the project is to build a fully functional IRC server from scratch that handles multiple simultaneous client connections through an `epoll`-based event loop.

The server implements a hybrid subset of the IRC protocol based on **RFC 1459** and **RFC 2812**. It supports essential features such as user authentication, nickname and username registration, channel management, private messaging, and operator privileges. **irssi** is the reference client, while basic tools such as **netcat** (`nc`) and **telnet** can also connect to the server.

Server-to-server communication is not implemented. The project focuses exclusively on the client-server relationship defined by the RFC specifications.

# 2. Features

See the [IRC Command Guide](./docs/COMMANDS.md) for a complete feature overview and detailed usage of each command.

# 3. Technical Characteristics and Constraints

- Written in **C++98**
- Handles multiple clients simultaneously
- Uses a **single `epoll` instance** for all I/O multiplexing
- Communicates over **TCP/IP (IPv4)**
- Does not fork
- Uses **irssi** as the reference client implementation; interaction with other IRC clients is not guaranteed

# 4. Usage

## 4.1. Prerequisites

### 4.1.1. Build and Runtime Environment

- A **Linux** environment with `epoll` support
- A C++ compiler with C++98 support (e.g. `c++`, `g++`, or `clang++`)
- GNU Make

### 4.1.2. Connection and Testing Tools

- **irssi** — Required to connect and verify behavior with the reference client
- **netcat** (`nc`) — Optional tool for sending raw IRC commands

## 4.2. Building

Clone the repository and compile the project:

```bash
git clone <repository-url>
cd ft_irc
make
```

Other Make targets:

| Target        | Description                                              |
| ------------- | -------------------------------------------------------- |
| `make`        | Compile the project and produce the `ircserv` executable |
| `make clean`  | Remove object files                                      |
| `make fclean` | Remove object files and the executable                   |
| `make re`     | Run `fclean`, then run `make`                            |

## 4.3. Running the Server

```bash
./ircserv <port> <password>
```

- `<port>` — Port on which the server listens for incoming client connections
- `<password>` — Connection password that clients must provide

**Example:**

```bash
./ircserv 6667 mypassword
```

## 4.4. Connecting with the IRC Client (irssi)

### 4.4.1. Method 1

1. Start irssi:

```bash
irssi
```

2. Enter the following command in irssi:

```irssi
/connect 127.0.0.1 6667 mypassword
```

### 4.4.2. Method 2

Start irssi with connection options:

```bash
irssi -c 127.0.0.1 -p 6667 -n mynick --password="mypasswd"
```

## 4.5. Connecting with Netcat

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

## 4.6. Notes for Trying the Project

- Commands listed in the [Feature Overview](./docs/COMMANDS.md#1-feature-overview) are case-insensitive.

- Commands other than registration commands are rejected until the [Connection and Registration](./docs/COMMANDS.md#21-connection-and-registration) process is complete.

- [netcat](#45-connecting-with-netcat) and [irssi](#44-connecting-with-the-irc-client-irssi) are different programs. irssi provides its own client-side commands, which may not be available in netcat.

# 5. Resources

## 5.1. References

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol Documentation](https://modern.ircdocs.horse)
- [IRC/2 Numeric List](https://www.alien.net.au/irc/irc2numerics.html)

## 5.2. AI Usage

AI tools were used during the development of this project in the following areas:

- **RFC interpretation** — Clarifying ambiguous sections of RFC 1459 and RFC 2812, particularly edge cases in command parsing and expected server behavior
- **Error code and response mapping** — Identifying the correct numeric replies for each command and organizing per-command error and response tables
- **README authoring** — Assisting with the structure and writing of this document
- **Code review ideation** — Generating code review suggestions and identifying potential issues in protocol-handling logic
- **Repetitive refactoring** — Handling mechanical refactoring tasks such as updating all call sites after function signature changes

---
