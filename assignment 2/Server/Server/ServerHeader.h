#define _CRT_SECURE_NO_WARNINGS

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <iostream>

void GetTime(char* sendBuff);
void GetTimeWithoutDate(char* sendBuff);
void GetTimeSinceEpoch(char* sendBuff);
void GetClientToServerDelayEstimation(char* sendBuff);
void GetTimeWithoutDateOrSeconds(char* sendBuff);
void GetYear(char* sendBuff);
void GetMonthAndDay(char* sendBuff);
void GetSecondsSinceBeginningOfMonth(char* sendBuff);
void GetWeekOfYear(char* sendBuff);
void GetDaylightSavings(char* sendBuff);
void GetTimeWithoutDateInCity(char recvBuff, char* sendBuff);
bool containsCityCode(char* recvBuff);
void MeasureTimeLap(char* sendBuff, clock_t& start, clock_t& end, bool& measurementStarted);
void checkIfMeasureStopAfter3Minuts(clock_t& start, clock_t& end, bool& measurementStarted);
void createTime(char* timeStr);