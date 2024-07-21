#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"

string getResourceNameAndQueryFromReq(string& socketBuffer, string& queryString);
string getQueryStringValue(const string& queryStr);
void initMethodType(METHOD methodType, int sizeMethod, int index, SocketState* sockets);
void buildHeaderForRes(string& httpResponse, const string& httpStatus, const string& contentType);
void buildBodyForRes(string& httpResponse, const string& contentBody, bool appendSuffix);
void handleGetRequest(string& socketBuff, string& sendBuff, METHOD methodType);
void handlePostRequest(string& socketBuff, string& sendBuff);
void handlePutRequst(string& socketBuff, string& sendBuff);
void handleOptionsRequest(string& socketBuff, string& sendBuff);
void handleTraceRequest(string& socketBuff, string& sendBuff);
void handleDeleteRequest(string& socketBuff, string& sendBuff);
void handleUnKnownMethod(string& socketBuff, string& sendBuff);
bool checkIsResourceHtml(const string& fullResourcePath);
string saveBodyToFile(string& buffer, ofstream& outputFile, int content_length);
string makePostHTMLbody(const string& filePath, const string& body);
string getHeader(const string& socketBuffer, const string& targetKey);
void removeHeadersFromBuffer(string& socketBuffer);
void removeHeadersAndBodyFromBuffer(string& socketBuffer);