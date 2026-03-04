## TCP & UDP Client-Server Application

## How to Use

1. Start the server:

```bash
./server 12345
```

2. Start TCP clients (subscribers), for example:

```bash
./subscriber C1 127.0.0.1 12345
```

3. After the clients subscribe to topics, send messages using the UDP client:

```bash
python3 udp_client.py --source-port 1234 --input_file three_topics_payloads.json --mode random --delay 2000 127.0.0.1 12345
```

---

## Project Goal

The goal of this project is to implement a **client-server application** that manages messages using the **publish–subscribe model**.

The system consists of three components:

1. **Server**  
   A single server that manages communication between all clients and handles message publishing and subscriptions.

2. **TCP Clients (Subscribers)**  
   - Connect to the server  
   - Receive commands from the keyboard (`subscribe` / `unsubscribe`)  
   - Display messages received from the server

3. **UDP Clients (Publishers)**  
   Publish messages by sending them to the server using a predefined protocol.

---

## Application Workflow

The server opens **two sockets**:

- one **TCP socket**
- one **UDP socket**

Both are added to the **`pfds` structure**, together with the file descriptor used for reading input from `stdin`.  
This structure is used to monitor events.

The server then enters a loop where it processes information received from clients.

### Server Commands

The server can receive input from `stdin`. The only valid command is:

```
exit
```

When this command is issued:

- the server disconnects all connected clients
- sends an **exit message** to each client
- removes them from the database
- closes all sockets

---

### Possible Events

If the server is not shutting down, three main situations may occur:

#### 1. Data from a TCP client
The server receives the **client ID** and checks whether:

- the client is **new**, or
- the client is **reconnecting**

If the client reconnects, the server sends any messages that were received while the client was disconnected.

---

#### 2. Data from a UDP client
The server receives information about **topics**.

The message is stored in a structure of type `udp_message_t`, then converted into the message format that will be sent to subscribers.

If a subscriber is **offline** but has a topic with **SF = 1 (store-and-forward)**, the messages are stored in a **queue** until the client reconnects.

---

#### 3. Message from an already connected TCP client

If no data is received, it means the client disconnected:

- the connection is closed
- the client is removed from `pfds`

Otherwise, the server processes the command received from the client:

- `subscribe`
- `unsubscribe`

---

## Subscriber Client

The subscriber client behaves similarly to the server but is simpler:

- it only uses a **TCP socket**
- it processes input from:
  - the keyboard
  - the server

---

## Implemented TCP Protocol

To implement an efficient protocol, the following considerations were taken into account:

- **Nagle's algorithm was disabled** to avoid delays in message transmission.
- Only the necessary data is transmitted:
  - before sending a message, the **exact size of the data** is sent.
  - this prevents sending unnecessary data and avoids receiving corrupted messages.
- The message size is transmitted as a **`uint32_t`**, ensuring consistent size representation across platforms (4 bytes).
- Structures are sent using:

```
__attribute__((__packed__))
```

to prevent sending unnecessary padding bytes.

- Communication is efficient because **clients receive only the messages corresponding to the topics they subscribed to**.

---

## Observations

This assignment was very interesting. It was somewhat difficult at the beginning, but after reviewing the laboratory materials several times, the implementation became much clearer.

The project helped me better understand **socket programming and network communication**, and overall it was a very useful learning experience.
