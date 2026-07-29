*This project has been created as part of the 42 curriculum by ahrelee, hoysong, jinyjeon.*

![image.png](https://github.com/hoysong/ft_irc/blob/main/image.png)

# Description

ft_irc is a lightweight IRC (Internet Relay Chat) server written in C++98 as part of the 42 common core curriculum. The goal of the project is to build a fully functional IRC server from scratch, capable of handling multiple simultaneous client connections using non-blocking I/O and a single `poll()` system call.

The server implements a hybrid subset of the IRC protocol based on both **RFC 1459** and **RFC 2812**, supporting essential features such as user authentication, nickname and username registration, channel management, private messaging, and operator privileges. It is designed to work with real-world IRC clients such as **irssi**, **HexChat**, or **WeeChat**, as well as basic tools like **netcat** (`nc`) or **telnet**.

No server-to-server communication is implemented. The project focuses exclusively on the client-server relationship as defined in the RFC specifications.

# Features

## Connection & Registration
- **PASS** — Set a connection password before registration
- **NICK** — Set or change a user's nickname
- **USER** — Set the username and realname during registration

## Messaging
- **PRIVMSG** — Send a message to a user or a channel
- **NOTICE** — Send a notice to a user or a channel (no automatic replies)

## Channel Operations
- **JOIN** — Join one or more channels
- **PART** — Leave one or more channels
- **TOPIC** — View or change the topic of a channel
- **INVITE** — Invite a user to a channel
- **KICK** — Remove a user from a channel

## Channel Modes (MODE)
The following channel modes are supported via the `MODE` command:

| Mode | Parameter | Description                            |
| ---- | --------- | -------------------------------------- |
| `i`  | None      | Set/remove invite-only channel         |
| `t`  | None      | Restrict TOPIC changes to operators    |
| `k`  | `<key>`   | Set/remove a channel key (password)    |
| `o`  | `<nick>`  | Give/take channel operator privilege   |
| `l`  | `mit>`    | Set/remove a user limit on the channel |

## Server Utilities
- **PING** — Keep the connection alive
- **QUIT** — Disconnect from the server

## Technical Constraints
- Written in **C++98**
- Handles multiple clients simultaneously without hanging
- All I/O operations are **non-blocking**
- Uses a **single `poll()`** for all I/O multiplexing
- Communication via **TCP/IP (IPv4)**
- No forking

# Instructions

## Prerequisites

- A C++ compiler with C++98 support (e.g., `c++`, `g++`, `clang++`)
- GNU Make

## Building

Clone the repository and compile:

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
| `make re`     | Run `fclean` followed by `make`                          |

## Running the Server

```bash
./ircserv <port> <password>
```

- `<port>` — The port number on which the server listens for incoming connections
- `<password>` — The connection password that clients must provide

**Example:**

```bash
./ircserv 6667 mypassword
```

## Connecting with an IRC Client (irssi)

```
irssi
/connect 127.0.0.1 6667 mypassword
```

## Connecting with Netcat

```bash
nc -C 127.0.0.1 6667
```

Then authenticate manually:

```
PASS mypassword
NICK mynick
USER myuser 0 * :My Real Name
```

> **Note:** The `-C` flag is used because IRC messages are terminated with `\r\n` as defined in the RFC standards.

# Resources

## References

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol Documentation](https://modern.ircdocs.horse)
- [IRC/2 Numeric List](https://www.alien.net.au/irc/irc2numerics.html)

## AI Usage

AI tools were used during the development of this project in the following areas:

- **RFC interpretation** — Clarifying ambiguous sections of RFC 1459 and RFC 2812, particularly around edge cases in command parsing and expected server behavior
- **Error code and response mapping** — Identifying the correct numeric replies for each command and organizing per-command error/response tables
- **README authoring** — Assisting with the structure and writing of this document
- **Code review ideation** — Generating suggestions for code review and identifying potential issues in protocol handling logic
- **Repetitive refactoring** — Delegating mechanical refactoring tasks such as updating all call sites after function signature changes
---
