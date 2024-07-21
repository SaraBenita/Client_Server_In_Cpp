#include "Server.h"
#include "MethodImp.h"



void main()
{
	struct SocketState sockets[MAX_SOCKETS] = { 0 };
	int socketsCount = 0;

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}


	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == listenSocket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}


	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(PORT);
	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}


	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}
	addSocket(listenSocket, LISTEN, sockets, socketsCount);

	// Accept connections and handles them one by one.
	while (true)
	{
		
		// Check for timeouts
		clock_t currentTime = clock();
		for (int i = 1; i < socketsCount; i++)
		{
			if (sockets[i].recv != EMPTY && sockets[i].timeLastByte != -1)
			{
				double elapsedTime = double(currentTime - sockets[i].timeLastByte) / CLOCKS_PER_SEC;
				if (elapsedTime > TIMEOUT_SECONDS)
				{
					cout << "Time Server: Closing connection due to timeout for socket " << sockets[i].id << endl;
					closesocket(sockets[i].id);
					removeSocket(i, sockets, socketsCount);
				}
			}
		}
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			cout << "Time Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(i, sockets, socketsCount);
					break;

				case RECEIVE:
					receiveMessage(i, sockets, socketsCount);
					break;
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				switch (sockets[i].send)
				{
				case SEND:
					sendMessage(i, sockets);
					break;
				}
			}
		}
		
	}
	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
}

bool addSocket(SOCKET id, int what,SocketState *sockets,int& socketsCount)
{
	//
	// Set the socket to be in non-blocking mode.
	//
	unsigned long flag = 1;
	if (ioctlsocket(id, FIONBIO, &flag) != 0)
	{
		cout << "Time Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	}
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].timeLastByte = -1;
			socketsCount++;
			return (true);
		}
	}
	return (false);
}

void removeSocket(int index, SocketState* sockets, int& socketsCount)
{
	sockets[index].recv = EMPTY;
	sockets[index].send = EMPTY;
	socketsCount--;
}

void acceptConnection(int index, SocketState* sockets, int& socketsCount)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Time Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;


	if (addSocket(msgSocket, RECEIVE, sockets, socketsCount) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void receiveMessage(int index, SocketState* sockets, int& socketsCount)
{
	SOCKET msgSocket = sockets[index].id;
	char newReqBuffer[MAX_LENGTH_BUFFER];
	int bytesRecv = recv(msgSocket, newReqBuffer, MAX_LENGTH_BUFFER - 1, 0);
	
	sockets[index].timeLastByte = clock();

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index, sockets, socketsCount);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index, sockets, socketsCount);
		return;
	}
	else
	{
		newReqBuffer[bytesRecv] = '\0';
		sockets[index].buffer = sockets[index].buffer + newReqBuffer;
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << newReqBuffer << "\" message.\n";
		size_t endMethod = sockets[index].buffer.find(" ");
		if (sockets[index].buffer.size() > 0)
		{
			if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "GET", 3) == 0)
			{
				initMethodType(SEND_GET, 3, index, sockets);
				return;
			}
			else if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "POST", 4) == 0)
			{
				initMethodType(SEND_POST, 4, index, sockets);
				return;
			}
			else if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "PUT", 3) == 0)
			{
				initMethodType(SEND_PUT, 3, index, sockets);
				return;
			}
			else if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "DELETE", 6) == 0)
			{
				initMethodType(SEND_DELETE, 6, index, sockets);
				return;
			}
			else if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "HEAD", 4) == 0)
			{
				initMethodType(SEND_HEAD, 4, index, sockets);
				return;
			}
			else if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "TRACE", 5) == 0)
			{
				initMethodType(SEND_TRACE, 5, index, sockets);
				return;
			}
			else if (strncmp(sockets[index].buffer.substr(0, endMethod).c_str(), "OPTIONS", 7) == 0)
			{
				initMethodType(SEND_OPTIONS, 7, index, sockets);
				return;
			}
			else 
			{
				initMethodType(SEND_UNKNOW, sockets[index].buffer.substr(0, endMethod).length(), index, sockets);
			}

		}
	}

}

void sendMessage(int index, SocketState* sockets)
{
	int bytesSent = 0;
	string sendBuff;
	SOCKET msgSocket = sockets[index].id;


	if (sockets[index].method == SEND_GET)
	{
		handleGetRequest(sockets[index].buffer, sendBuff, SEND_GET);
	}
	else if (sockets[index].method == SEND_POST)
	{
		handlePostRequest(sockets[index].buffer, sendBuff);
	}
	else if (sockets[index].method == SEND_OPTIONS)
	{
		handleOptionsRequest(sockets[index].buffer, sendBuff);
	}
	else if (sockets[index].method == SEND_HEAD)
	{
		handleGetRequest(sockets[index].buffer, sendBuff, SEND_HEAD);
	}
	else if (sockets[index].method == SEND_PUT)
	{
		handlePutRequst(sockets[index].buffer, sendBuff);
	}
	else if (sockets[index].method == SEND_DELETE)
	{
		handleDeleteRequest(sockets[index].buffer, sendBuff);
	}
	else if (sockets[index].method == SEND_TRACE)
	{
		handleTraceRequest(sockets[index].buffer, sendBuff);
	}
	else
	{
		handleUnKnownMethod(sockets[index].buffer, sendBuff);
	}

	bytesSent = send(msgSocket, sendBuff.c_str(), (int)strlen(sendBuff.c_str()), 0);

	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff.c_str()) << " bytes of \"" << sendBuff << "\" message.\n";

	if (sockets[index].buffer.length() > 0)
	{
		sockets[index].send = SEND;
	}
	else
	{
		sockets[index].send = IDLE;
	}

}





