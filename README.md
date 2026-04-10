# ft_irc

> A minimal IRC server written in C++98, compatible with the **irssi** client.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Build](#build)
- [Usage](#usage)
- [Connecting with irssi](#connecting-with-irssi)
- [Supported Commands](#supported-commands)
- [Code Structure](#code-structure)

---

## Overview

`ft_irc` is an IRC server implemented in C++98 as part of the 42 curriculum. It follows the IRC protocol (RFC 1459) and allows multiple clients to connect simultaneously, join channels, exchange messages, and manage channel modes.

The server uses a **non-blocking** model based on `poll()`: a single thread handles all connections.

---

## Features

- Simultaneous connections from multiple clients
- Password-based authentication (`PASS`)
- User registration (`NICK` / `USER`)
- Channel creation and management
- Private messaging between users
- Channel modes: `+i`, `+t`, `+k`, `+l`, `+o`
- Channel operator management
- Invitations (`INVITE`) and kicks (`KICK`)
- Clean disconnection handling
- Compatible with **irssi** and **WeeChat**

---

## Architecture

```
ft_irc/
├── src/
│   ├── main.cpp
│   ├── Server.cpp       — Networking, poll() loop, command dispatch
│   ├── Client.cpp       — Connected client state, I/O buffers
│   ├── Channel.cpp      — IRC channel management
│   └── Message.cpp      — IRC message parsing / serialization
├── include/
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Message.hpp
│   └── Replies.hpp      — IRC numeric reply codes (RPL_*, ERR_*)
└── Makefile
```

### Data Flow

```
TCP Client
    │
    ▼
poll() ──► POLLIN  ──► _receiveData()  ──► _processLines()  ──► _executeCommand()
       ──► POLLOUT ──► _sendData()
       ──► POLLERR / POLLHUP ──► _disconnectClient()
```

### IRC Message Parsing

A raw IRC message follows this format:

```
[:<prefix>] <COMMAND> [param1] [param2] [:<trailing>]
```

Example: `:alice!user@host PRIVMSG #general :Hello everyone`

The `Message` class handles parsing and reconstruction of these messages.

---

## Build

```bash
make
```

This produces the `ircserv` executable.

```bash
make clean    # remove object files
make fclean   # remove object files + executable
make re       # full rebuild
```

---

## Usage

```bash
./ircserv <port> <password>
```

| Argument     | Description                              |
|--------------|------------------------------------------|
| `<port>`     | Listening port (e.g. `6667`)             |
| `<password>` | Password required to connect             |

Example:

```bash
./ircserv 6667 mypassword
```

Graceful shutdown with `Ctrl+C` (SIGINT).

---

## Connecting with irssi

```bash
irssi
```

Once inside irssi:

```
/connect localhost 6667 mypassword
```

Then join a channel:

```
/join #general
```

---

## Supported Commands

### Registration

| Command | Syntax | Description |
|---------|--------|-------------|
| `PASS` | `PASS <password>` | Authenticate with server password |
| `NICK` | `NICK <nickname>` | Set or change nickname |
| `USER` | `USER <username> 0 * :<realname>` | Register the client |
| `QUIT` | `QUIT [:<reason>]` | Disconnect from the server |

### Channels

| Command | Syntax | Description |
|---------|--------|-------------|
| `JOIN` | `JOIN <#channel> [key]` | Join or create a channel |
| `PART` | `PART <#channel> [:<reason>]` | Leave a channel |
| `TOPIC` | `TOPIC <#channel> [:<topic>]` | Get or set the channel topic |
| `KICK` | `KICK <#channel> <nick> [:<reason>]` | Remove a member (operator only) |
| `INVITE` | `INVITE <nick> <#channel>` | Invite a user to a channel |
| `MODE` | `MODE <#channel> <modes> [args]` | Set channel modes |
| `WHO` | `WHO <#channel>` | List channel members |

### Messaging

| Command | Syntax | Description |
|---------|--------|-------------|
| `PRIVMSG` | `PRIVMSG <target> :<message>` | Send a message to a user or channel |
| `NOTICE` | `NOTICE <target> :<message>` | Send a notice (no automatic reply) |

### Channel Modes (`MODE`)

| Mode | Argument | Description |
|------|----------|-------------|
| `+i` / `-i` | — | Invite-only channel |
| `+t` / `-t` | — | Only operators can change the topic |
| `+k` / `-k` | `<key>` | Protect the channel with a password |
| `+l` / `-l` | `<limit>` | Limit the number of members |
| `+o` / `-o` | `<nick>` | Grant / revoke operator status |

---

## Code Structure

### `Server`

The heart of the server. Manages the listening socket, the `poll()` loop, client acceptance, data reception and sending, and dispatches incoming messages to the appropriate command handlers.

### `Client`

Represents a connected client. Maintains receive and send buffers, registration state (`PASS` + `NICK` + `USER` all set), and identity information (nickname, username, hostname).

### `Channel`

Represents an IRC channel. Manages the member list, operators, invited users, active modes, and the topic.

### `Message`

Handles parsing and serialization of IRC protocol messages. Extracts the prefix, command, and parameters (including the trailing parameter preceded by `:`).

### `Replies.hpp`

Centralizes the IRC numeric reply codes used in server responses (`001`, `353`, `433`, etc.).

