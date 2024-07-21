#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <iostream>
#include "ClientHeader.h"

#define TIME_PORT	27015


void main()
{
	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.
	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called server. 

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");  //LocalHost- כתובת לשלך. במקום לשלוח לשרת אחר שולחים לשלך מעין לופ וזה מחזיר תשובה מהמחשב שלך.שולחים לשרת והשרת נמצא על המחשב שלי
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.
	
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	int choice;
	char cityLetter;
	bool flag = true;
	
	while (flag)
	{
		choice = checkValidInputFromMenu();

		if (choice == 14)
			flag = false;
		
		if (flag == true)
		{
			sendAndRecvMessageFromServer(connSocket, server, choice, flag, sendBuff, recvBuff, bytesSent, bytesRecv);
			printheServerResponseToTheClientquestion(choice, bytesRecv, recvBuff);
		}

		else { 
			// option 14
			// Closing connections and Winsock.
			cout << "Time Client: Closing Connection.\n";
			closesocket(connSocket);

			system("pause");
		}
	}
}


void printMenu()
{
	cout << "From the following menu, Choose whiche message you would like to send to the server?" << endl;
	cout << "1. Return the time in year, month, day, hour, minute, and second format." << endl;
	cout << "2. Return the time in hour, minute, and second format." << endl;
	cout << "3. Return the time in the format of seconds starting from 1.1.1970." << endl;
	cout << "4. Return an estimate of the delay between the client and the server." << endl;
	cout << "5. Return RoundTripTime (RTT) measurement." << endl;
	cout << "6. Return the time in hour and minute format." << endl;
	cout << "7. Return the year only." << endl;
	cout << "8. Return the month and day." << endl;
	cout << "9. Return the number of seconds that have passed since the beginning of the current month." << endl;
	cout << "10. Return the week number from the beginning of the year." << endl;
	cout << "11. Return 1 if summer time is set or 0 for winter time." << endl;
	cout << "12. Return the time in hour, minute and second format in different cities of the world." << endl;
	cout << "13. Return the elapsed time between the first MeasureTimeLap request received from the client and the second MeasureTimeLap request." << endl;
	cout << "14. Exit." << endl;
}

int checkValidInputFromMenu()
{
	int choice;
	bool validInput = false;
	
	printMenu();
	do {	
		cin >> choice;
		cin.clear(); // Clear any error flags

		// Clear the input buffer
		while (cin.get() != '\n') continue;

		// Check if the input is valid
		if (choice >= 1 && choice <= 14) {
			validInput = true;
		}
		else {
			cout << "Invalid input. Please try again" << endl;
			printMenu();
		}
	} while (!validInput);

	return choice;
}

void menuCitys()
{
	cout << "Please select a city: " << endl;
	cout << "A. Doha (Qatar)" << endl;
	cout << "B. Prague (Czech Republic)" << endl;
	cout << "C. New York (USA)" << endl;
	cout << "D. Berlin (Germany)" << endl;
	cout << "E. Other (Get UTC)" << endl;
}

char selectACityForOption12() {
	char cityLetter;
	bool validInput = false;

	do {
		cout << "Please select a city:" << endl;
		cout << "A. Doha (Qatar)" << endl;
		cout << "B. Prague (Czech Republic)" << endl;
		cout << "C. New York (USA)" << endl;
		cout << "D. Berlin (Germany)" << endl;
		cout << "E. Other (Get UTC)" << endl;

		cin >> cityLetter;
		cin.clear(); // Clear any error flags

		// Clear the input buffer
		while (cin.get() != '\n') continue;

		// Convert the input to uppercase
		cityLetter = toupper(cityLetter);

		// Check if the input is valid
		if (cityLetter == 'A' || cityLetter == 'B' || cityLetter == 'C' || cityLetter == 'D' || cityLetter == 'E') {
			validInput = true;
		}
		else {
			cout << "Invalid input. Please enter A, B, C, D or E." << endl;
		}
	} while (!validInput);

	return cityLetter;
}

void sendAndRecvMessageFromServer(SOCKET connSocket, sockaddr_in server, int choice, bool& flag,  char* sendBuff, char* recvBuff, int& bytesSent, int& bytesRecv)
{
	char cityLetter;

	if (choice == 1)
		strcpy(sendBuff, "1");
	if (choice == 2)
		strcpy(sendBuff, "2");
	if (choice == 3)
		strcpy(sendBuff, "3");
	if (choice == 4)
	{
		clientToServerDelayEstimation(connSocket, server, bytesSent, bytesSent);
		return;
	}
	if (choice == 5)
	{
		measureRTTClientToServer(connSocket, server, bytesSent, bytesSent);
		return;
	}
	if (choice == 6)
		strcpy(sendBuff, "6");
	if (choice == 7)
		strcpy(sendBuff, "7");
	if (choice == 8)
		strcpy(sendBuff, "8");
	if (choice == 9)
		strcpy(sendBuff, "9");
	if (choice == 10)
		strcpy(sendBuff, "10");
	if (choice == 11)
		strcpy(sendBuff, "11");
	if (choice == 12)
	{
		cityLetter = selectACityForOption12();
		sendBuff[0] = cityLetter;
		sendBuff[1] = '\0'; // Null-terminate the string
	}
	if (choice == 13)
		strcpy(sendBuff, "13");

	sendAMessageToTheServer(connSocket, server, sendBuff, bytesSent);
	recvAMessageFromTheServer(connSocket, recvBuff, bytesRecv);
}

void printheServerResponseToTheClientquestion(int choice, int bytesRecv, char recvBuff[255])
{
	if (choice == 1)
		cout << "Time in Year, month, day, hour, minute and second:(1) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 2)
		cout << "Time in hour, minute and second format:(2) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 3)
		cout << "Time in seconds starting from 1.1.1970:(3) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 6)
		cout << "Time in hour and minute format:(6) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 7)
		cout << "Return the year only:(7) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 8)
		cout << "Return the month and day:(8) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 9)
		cout << "The number of seconds that have passed since the beginning of the current month:(9) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 10)
		cout << "Return the week number from the beginning of the year:(10) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 11)
		cout << "Return 1 if summer time is set or 0 for winter time:(11) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 12)
		cout << "Time in hour, minute and second format in different cities:(12) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	if (choice == 13)
	{
		cout << "Return the elapsed time between the first MeasureTimeLap request received from the client and the" << endl;
		cout << "second Measure Time Lap request:(13) " << bytesRecv << "/" << strlen(recvBuff) << " bytes of \"" << recvBuff << "\" message.\n" << endl;
	}
}

void clientToServerDelayEstimationTest(SOCKET connSocket, sockaddr_in server, int& bytesSent, int& bytesRecv)
{
	int i;
	static double total = 0;
	double current = 0, last = 0;
	char sendBuff[255] = "4", recvBuff[255] = "";

	for (i = 0; i < 100; i++)
		sendAMessageToTheServer(connSocket, server, sendBuff, bytesSent);

	for (i = 0; i < 100; i++)
	{
		recvAMessageFromTheServer(connSocket, recvBuff, bytesRecv);
		current = atoll(recvBuff);
		if (last != 0)
			total += current - last;
		last = current;
	}

	if (total != 0)
		cout << "Estimated client-server delay: " << total / (100 - 1) << " Miliseconds." << endl << endl;
	else
		cout << "Esimation calculation failed.." << endl;
	total = 0;
}

void clientToServerDelayEstimation(SOCKET connSocket, sockaddr_in server, int& bytesSent, int& bytesRecv)
{
	int i;
	static double total = 0;
	double current = 0, last = 0;
	char sendBuff[255] = "4", recvBuff[255] = "";
	char timeStr[80];

	for (i = 0; i < 100; i++)
		sendAMessageToTheServer(connSocket, server, sendBuff, bytesSent);

	for (i = 0; i < 100; i++)
	{
		recvAMessageFromTheServer(connSocket, recvBuff, bytesRecv);
		current = atoll(recvBuff);
		if (last != 0)
			total += current - last;
		last = current;

		createTime(timeStr);
		// Print received message and modified time
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message at " << timeStr << endl;
	}

	if (total != 0)
		cout << "Estimated client-server delay: " << total / (100 - 1) << " Miliseconds." << endl << endl;
	else
		cout << "Esimation calculation failed.." << endl;
	total = 0;
}

void createTime(char* timeStr)
{
	// Get current time in the local time zone
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	// Format and print time
	snprintf(timeStr, 80, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void sendAMessageToTheServer(SOCKET& connSocket, sockaddr_in& server, char* sendBuff, int& bytesSent)
{
	//int bytesSent;

	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}
	cout << "Client Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
}

void recvAMessageFromTheServer(SOCKET connSocket, char* recvBuff, int& bytesRecv)
{
	//int bytesRecv;

	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}
	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
}

void measureRTTClientToServer(SOCKET connSocket, sockaddr_in server, int& bytesSent, int& bytesRecv)
{
	static double total = 0;
	int i;
	char sendBuff[255] = "5", recvBuff[255] = "";
	long double sendTime, recieveTime;
	char timeStr[80];

	for (i = 0; i < 100; i++) {
		sendAMessageToTheServer(connSocket, server, sendBuff, bytesSent);
		sendTime = GetTickCount();
		recvAMessageFromTheServer(connSocket, recvBuff, bytesRecv);

		createTime(timeStr);
		// Print received message and modified time
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message at " << timeStr << endl;

		recieveTime = GetTickCount();
		total += (recieveTime - sendTime);
	}

	if (total != 0)
		cout << "Estimated RTT: " << total / 100 << " Miliseconds." << endl << endl;
	else
		cout << "Estimation calculation failed." << endl;
	total = 0;
}