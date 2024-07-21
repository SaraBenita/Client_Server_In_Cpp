#define _CRT_SECURE_NO_WARNINGS

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include "ServerHeader.h"

#define TIME_PORT	27015


void main()
{
	WSAData wsaData;

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	clock_t start, end;
	bool measurementStarted = false;

	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		checkIfMeasureStopAfter3Minuts(start, end, measurementStarted); //For 13

		// Process request based on the received message
		if (strcmp(recvBuff, "1") == 0)
			GetTime(sendBuff);
		if (strcmp(recvBuff, "2") == 0)
			GetTimeWithoutDate(sendBuff);
		if (strcmp(recvBuff, "3") == 0)
			GetTimeSinceEpoch(sendBuff);
		if (strcmp(recvBuff, "4") == 0)
			GetClientToServerDelayEstimation(sendBuff);
		if (strcmp(recvBuff, "5") == 0)
			sprintf(sendBuff, "Type 5 message.");
		if (strcmp(recvBuff, "6") == 0)
			GetTimeWithoutDateOrSeconds(sendBuff);
		if (strcmp(recvBuff, "7") == 0)
			GetYear(sendBuff);
		if (strcmp(recvBuff, "8") == 0)
			GetMonthAndDay(sendBuff);
		if (strcmp(recvBuff, "9") == 0)
			GetSecondsSinceBeginningOfMonth(sendBuff);
		if (strcmp(recvBuff, "10") == 0)
			GetWeekOfYear(sendBuff);
		if (strcmp(recvBuff, "11") == 0)
			GetDaylightSavings(sendBuff);
		if (containsCityCode(recvBuff))
			GetTimeWithoutDateInCity(recvBuff[0], sendBuff);
		if (strcmp(recvBuff, "13") == 0)
			MeasureTimeLap(sendBuff, start, end, measurementStarted);


		// Sends the answer to the client, using the client address gathered
		// by recvfrom. 
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}


void GetTime(char* sendBuff) 
{
	// Get the current time.
	time_t timer;
	struct tm* tm_info;
	time(&timer);
	tm_info = localtime(&timer);

	// Format the time into the desired string.
	sprintf(sendBuff, "%04d-%02d-%02d %02d:%02d:%02d",
		tm_info->tm_year + 1900, // Year since 1900, so adding 1900 gives current year
		tm_info->tm_mon + 1,     // Month starts from 0, so adding 1 for correct month
		tm_info->tm_mday,        // Day of the month
		tm_info->tm_hour,        // Hour
		tm_info->tm_min,         // Minute
		tm_info->tm_sec);        // Second
}

void GetTimeWithoutDate(char* sendBuff)
{
	// Get the current time
	time_t now = time(0);
	tm* ltm = localtime(&now);

	// Format the time without the date
	sprintf(sendBuff, "%02d:%02d:%02d",
		ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
}

void GetTimeSinceEpoch(char* sendBuff) {
	// Get the current time since epoch.
	time_t now;
	time(&now);

	sprintf(sendBuff, "%ld", now);
}

void GetClientToServerDelayEstimation(char* sendBuff) 
{
	DWORD serverTickCount = GetTickCount();

	// Convert tick count to string and store it in sendBuff
	sprintf(sendBuff, "%lu", serverTickCount);
}

void GetClientToServerDelayEstimationTest(char* sendBuff) {

	char timeStr[80];

	DWORD serverTickCount = GetTickCount();
	createTime(timeStr);
	sprintf(sendBuff, "%s, %lu", timeStr, serverTickCount);
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

void GetTimeWithoutDateOrSeconds(char* sendBuff) {

	time_t timer;
	struct tm* tm_info;
	time(&timer);
	tm_info = localtime(&timer);

	sprintf(sendBuff, "%02d:%02d", tm_info->tm_hour, tm_info->tm_min);
}

void GetYear(char* sendBuff) {
	time_t timer;
	struct tm* tm_info;
	time(&timer);
	tm_info = localtime(&timer);

	sprintf(sendBuff, "%04d", tm_info->tm_year + 1900);
}

void GetMonthAndDay(char* sendBuff) 
{
	time_t timer;
	struct tm* tm_info;
	time(&timer);
	tm_info = localtime(&timer);

	sprintf(sendBuff, "%02d-%02d", tm_info->tm_mon + 1, tm_info->tm_mday);
}

void GetSecondsSinceBeginningOfMonth(char* sendBuff)
{
	time_t currentTime;
	time(&currentTime);
	struct tm* localTime = localtime(&currentTime);

	// Get the beginning of the current month.
	struct tm beginningOfMonth = *localTime;
	beginningOfMonth.tm_mday = 1;
	beginningOfMonth.tm_hour = 0;
	beginningOfMonth.tm_min = 0;
	beginningOfMonth.tm_sec = 0;
	mktime(&beginningOfMonth);

	// Calculate the difference in seconds.
	double secondsSinceBeginningOfMonth = difftime(currentTime, mktime(&beginningOfMonth));

	sprintf(sendBuff, "%.0f", secondsSinceBeginningOfMonth);
}

void GetWeekOfYear(char* sendBuff) 
{
	time_t timer;
	struct tm* tm_info;
	time(&timer);
	tm_info = localtime(&timer);

	sprintf(sendBuff, "%d", tm_info->tm_yday / 7 + 1);
}

void GetDaylightSavings(char* sendBuff) 
{
	time_t currentTime;
	struct tm* localTime;
	time(&currentTime);
	localTime = localtime(&currentTime);
	int daylightSavings = localTime->tm_isdst;

	sprintf(sendBuff, "%d", daylightSavings);
}

bool containsCityCode(char* recvBuff) {

	for (int i = 0; recvBuff[i] != '\0'; i++) {
		// Check if the character is one of 'A', 'B', 'C', or 'D'
		if (recvBuff[i] == 'A' || recvBuff[i] == 'B' || recvBuff[i] == 'C' || recvBuff[i] == 'D' || recvBuff[i] == 'E') {
			return true; // Found a city code
		}
	}
	return false;
}

void GetTimeWithoutDateInCity(char recvBuff, char* sendBuff)
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	switch (recvBuff) {
	case 'A':
		timeinfo->tm_hour = (timeinfo->tm_hour + 3) % 24; // Doha (UTC+3)
		break;
	case 'B':
		timeinfo->tm_hour = (timeinfo->tm_hour + 1) % 24; // Prague (UTC+1)
		break;
	case 'C':
		timeinfo->tm_hour = (timeinfo->tm_hour - 4 + 24) % 24; // New York (UTC-4)
		break;
	case 'D':
		timeinfo->tm_hour = (timeinfo->tm_hour + 2) % 24; // Berlin (UTC+2)
		break;
	default:
		strftime(buffer, sizeof(buffer), "Coordinated Universal Time (UTC): %H:%M:%S", timeinfo);
		strcpy(sendBuff, buffer);
		return; // Exit function if input is invalid
	}

	// Format time
	strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);

	// Append city name to the time
	strcat(buffer, " (");
	switch (recvBuff) {
	case 'A':
		strcat(buffer, "Doha, Qatar");
		break;
	case 'B':
		strcat(buffer, "Prague, Czech Republic");
		break;
	case 'C':
		strcat(buffer, "New York, USA");
		break;
	case 'D':
		strcat(buffer, "Berlin, Germany");
		break;
	}
	strcat(buffer, ")");

	strcpy(sendBuff, buffer);
}

void MeasureTimeLap(char* sendBuff, clock_t& start, clock_t& end, bool& measurementStarted)
{
	float endTime = 0, startTime = 0;
	
	if (measurementStarted == false) { // first time
		start = clock();
		sprintf(sendBuff, "The measurement was started for the first time: (if sec request is above 3 min you will get 0 value!)");
		measurementStarted = true;
	}
	else 
	{
		startTime = start / CLOCKS_PER_SEC;
		end = clock();
		endTime = end / CLOCKS_PER_SEC;
		sprintf(sendBuff, "time measured between the two requests is : %g seconds", (endTime - startTime));
		measurementStarted = false;
	}
}

void checkIfMeasureStopAfter3Minuts(clock_t& start, clock_t& end, bool& measurementStarted)
{
	end = clock();
	float sResult = start / CLOCKS_PER_SEC;
	float eResult = end / CLOCKS_PER_SEC;
	if ((eResult - sResult) > 180)
		measurementStarted = false;
}