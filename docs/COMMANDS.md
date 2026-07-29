# IRC Command Guide

This document describes the IRC commands supported by the ft_irc server.

- Commands are case-insensitive.
- Prefix the final parameter with `:` when it contains spaces.
- Before registration is complete, only `PASS`, `NICK`, `USER`, and `QUIT` are available.

[Back to the English README](../README.md)

## 1. Feature Overview

### 1.1. Connection and Registration

- **PASS** — Provide the connection password before registration
- **NICK** — Set or change a nickname
- **USER** — Set the username and real name during registration

### 1.2. Messaging

- **PRIVMSG** — Send a message to a user or channel
- **NOTICE** — Send a notice to a user or channel without automatic error replies

### 1.3. Channel Operations

- **JOIN** — Join one or more channels
- **PART** — Leave one or more channels
- **TOPIC** — View or change a channel topic
- **INVITE** — Invite a user to a channel
- **KICK** — Remove a user from a channel

### 1.4. Channel Modes (MODE)

| Mode | Parameter | Description                               |
| ---- | --------- | ----------------------------------------- |
| `i`  | None      | Enable or disable invite-only mode        |
| `t`  | None      | Restrict TOPIC changes to channel operators |
| `k`  | `<key>`   | Set or remove the channel key (password)  |
| `o`  | `<nick>`  | Grant or revoke channel operator privileges |
| `l`  | `<limit>` | Set or remove the channel user limit      |

### 1.5. Server Utilities

- **PING** — Check that the connection is active
- **QUIT** — Disconnect from the server

## 2. Feature Details

### 2.1. Connection and Registration

#### PASS

Provides the server connection password before registration.

```text
PASS <server_password>
```

#### NICK

Sets or changes a nickname.

```text
NICK <nickname>
```

#### USER

Sets the username and real name during registration.

```text
USER <username> <mode> <unused> :<realname>
```

### 2.2. Messaging

#### PRIVMSG

Sends a message to a user or channel. Up to four comma-separated targets may be specified at once.

```text
PRIVMSG <nickname> :<message>
PRIVMSG #<channel> :<message>
PRIVMSG <target1>,<target2> :<message>
```

#### NOTICE

Sends a notice to a user or channel. The server does not automatically return errors for an invalid target or missing message.

```text
NOTICE <nickname> :<message>
NOTICE #<channel> :<message>
```

### 2.3. Channel Operations

#### JOIN

Joins one or more channels. If channels require keys, provide the keys in the same order as the channel names. `JOIN 0` leaves every channel currently joined.

```text
JOIN #<channel>
JOIN #<channel> <key>
JOIN #<channel1>,#<channel2> <key1>,<key2>
JOIN 0
```

#### PART

Leaves one or more channels and optionally includes a part message.

```text
PART #<channel>
PART #<channel1>,#<channel2>
PART #<channel> :<message>
```

#### TOPIC

Views or changes a channel topic. Passing an empty topic removes the existing topic.

```text
TOPIC #<channel>
TOPIC #<channel> :<topic>
TOPIC #<channel> :
```

#### INVITE

Invites a user to a channel.

```text
INVITE <nickname> #<channel>
```

#### KICK

Removes a user from a channel. A single channel can target multiple users, or equal numbers of channels and users can be paired in order.

```text
KICK #<channel> <nickname>
KICK #<channel> <nickname> :<reason>
KICK #<channel> <nick1>,<nick2> :<reason>
KICK #<channel1>,#<channel2> <nick1>,<nick2> :<reason>
```

### 2.4. Channel Modes (MODE)

Pass only a channel name to view its current modes. Only a channel operator can change channel modes.

```text
MODE #<channel>
MODE #<channel> +i
MODE #<channel> -i
MODE #<channel> +t
MODE #<channel> -t
MODE #<channel> +k <key>
MODE #<channel> -k <key>
MODE #<channel> +o <nickname>
MODE #<channel> -o <nickname>
MODE #<channel> +l <limit>
MODE #<channel> -l
```

Multiple modes can be changed in one command. Supply parameters in the same order as their corresponding modes.

```text
MODE #<channel> +it
MODE #<channel> +kol <key> <nickname> <limit>
```

### 2.5. Server Utilities

#### PING

Checks the connection. The server responds with a `PONG` containing the supplied token.

```text
PING <token>
```

#### QUIT

Closes the server connection. The reason is sent to users who share a channel with the departing user.

```text
QUIT
QUIT :<reason>
```
