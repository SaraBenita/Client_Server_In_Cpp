#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <sstream>
#include <time.h>
#include <fstream>
#include <iomanip>

enum METHOD {
	SEND_GET,
	SEND_POST,
	SEND_OPTIONS,
	SEND_HEAD,
	SEND_PUT,
	SEND_DELETE,
	SEND_TRACE,
	SEND_UNKNOW
};

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	string buffer;
	METHOD method;
	time_t timeLastByte;
};

const string ROOT_PATH = "C://temp/"; // default working directory
const int TIMEOUT_SECONDS = 120;
const int PORT = 80;
const int MAX_SOCKETS = 60;
const int MAX_LENGTH_BUFFER = 2000;
const int EMPTY = 0;
const int LISTEN = 1;
const int RECEIVE = 2;
const int IDLE = 3;
const int SEND = 4;
bool addSocket(SOCKET id, int what, SocketState* sockets, int& socketsCount);
void removeSocket(int index, SocketState* sockets, int& socketsCount);
void acceptConnection(int index, SocketState* sockets, int& socketsCount);
void receiveMessage(int index, SocketState* sockets, int& socketsCount);
void sendMessage(int index, SocketState* sockets);