# General Overview of the Assignments Repository

## Overview
This repository contains two programming assignments focusing on **network communication** and **web server development**. These tasks were implemented to demonstrate proficiency in socket programming, handling client-server communication, and basic web server functionality.

## Assignments in This Repository

### 1. UDP Client-Server for Time Services
This assignment implements a UDP-based client-server application that provides various time-related services. The client sends requests to the server for different time formats and information, including current time, time since the epoch, time zone-based responses for different cities, and more. The server processes these requests and sends the corresponding response.

#### Key Technologies Used:
- UDP Sockets
- Time calculations using `GetTickCount()` and `localtime()`
- Client-server communication via **blocking sockets**

### 2. HTTP Web Server
This project implements a basic HTTP server that adheres to HTTP/1.1 specifications. The server supports multiple HTTP methods (GET, POST, PUT, DELETE) and handles client requests concurrently using **non-blocking sockets**. The server is designed to respond to requests with dynamic language-specific content, depending on the query parameter `lang`.

#### Key Technologies Used:
- Non-blocking sockets for concurrent connections
- HTTP/1.1 Protocol
- Language-specific content delivery (Hebrew, English, French)

## What Can Be Found in This Repository
- **Source Code**: The full implementation of the UDP client-server and HTTP web server.
- **README Files**: Detailed explanations for each assignment with setup and execution instructions.
- 
Feel free to explore the respective folders for each assignment and dive into the source code to see how each task was implemented.

## How to Use
1. Clone the repository.
2. Navigate to each assignment directory.
3. Follow the respective README.md for instructions on running and testing the applications.

## Tools and Technologies
- Languages: C++
- Networking: UDP sockets, non-blocking sockets.
- Protocols: HTTP/1.1, Custom UDP protocol for time services.
- Testing: Wireshark, Postman for testing requests and responses.
- 
