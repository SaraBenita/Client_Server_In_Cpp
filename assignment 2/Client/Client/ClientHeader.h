#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <iostream>

void printMenu();
int checkValidInputFromMenu();
char selectACityForOption12();
void sendAndRecvMessageFromServer(SOCKET connSocket, sockaddr_in server, int choice, bool& flag, char* sendBuff, char* recvBuff, int& bytesSent, int& bytesRecv);
void printheServerResponseToTheClientquestion(int choice, int bytesRecv, char recvBuff[255]);
void clientToServerDelayEstimation(SOCKET connSocket, sockaddr_in server, int& bytesSent, int& bytesRecv);
void sendAMessageToTheServer(SOCKET& connSocket, sockaddr_in& server, char* sendBuff, int& bytesSent);
void recvAMessageFromTheServer(SOCKET connSocket, char* recvBuff, int& bytesRecv);
void measureRTTClientToServer(SOCKET connSocket, sockaddr_in server, int& bytesSent, int& bytesRecv);
void createTime(char* timeStr);