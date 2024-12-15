
### **README for Assignment 3: HTTP Web Server**
# HTTP Web Server

## Overview
This project implements a basic HTTP web server that adheres to the HTTP/1.1 specification (RFC 2616). The server supports multiple HTTP methods and processes concurrent client connections without using blocking sockets or multithreading.

## Features
### Supported HTTP Methods
1. **OPTIONS**: Lists supported HTTP methods.
2. **GET**: Handles query strings for language-based responses (`lang=he/en/fr`).
3. **HEAD**: Returns headers without the body.
4. **POST**: Accepts and logs string data from the client.
5. **PUT, DELETE, TRACE**: Basic implementation to handle these methods.

### GET Request Language Support
- Reads the `lang` parameter in the query string and returns the page in the corresponding language.
- Supports Hebrew (`he`), English (`en`), and French (`fr`). Defaults to English for unsupported languages.

### Concurrency
- Uses **non-blocking sockets** to manage multiple connections simultaneously.
- Enforces a two-minute timeout for idle connections.

### Error Handling
- Returns appropriate HTTP status codes (e.g., 400 for bad requests, 404 for missing pages).
- Implements robust validation for headers and request formats.

### Logging
- Logs POST request payloads and other key events to the server console.

### Testing
- Verified using **Wireshark** for packet analysis.
- Tested with Telnet, Chrome, and other tools to ensure compliance with HTTP standards.

## Technologies and Concepts
- **Non-Blocking Sockets**: Ensures responsiveness and efficient connection handling without multithreading.
- **HTTP/1.1 Protocol**: Implements request parsing and response generation following RFC 2616.
- **Socket Programming**: Handled connection setup, data parsing, and response generation.
- **Error Handling**: Ensured graceful degradation for malformed or unsupported requests.
- **Dynamic Language Support**: Query string parsing to deliver content in the requested language.

## Challenges Solved
- Implemented **concurrent connections** without threads by using non-blocking sockets.
- Handled **idle connections** by adding a timeout logic to close stale connections.
- Dynamically responded to **language-specific GET requests**.

## How to Run
1. Start the server: ./webserver

## Example GET Request:
curl "http://localhost:8080/index.html?lang=he"

##Example POST Request:
curl -X POST -d "Hello, server!" http://localhost:8080

## Testing Tools
- Wireshark: Captured HTTP traffic to verify request-response cycles.
- Browsers: Used Chrome and Firefox to test language-specific responses.

## Files
- webserver.cpp: Implements the HTTP server, request handling, and response generation.
- index.html: Sample HTML file for testing language-based responses.

