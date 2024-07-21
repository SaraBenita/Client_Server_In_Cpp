#include "MethodImp.h"


void handleGetRequest(string& socketBuff, string& sendBuff, METHOD methodType)
{
    string queryStr, resourcePath, fullResourcePath, content_type, content_length;
    ifstream file;
    resourcePath = getResourceNameAndQueryFromReq(socketBuff, queryStr);
    fullResourcePath = ROOT_PATH + getQueryStringValue(queryStr) + resourcePath;
    removeHeadersFromBuffer(socketBuff);
    file.open(fullResourcePath);
    if (checkIsResourceHtml(fullResourcePath))
    {
        if (file.is_open() == false)
        {
            buildHeaderForRes(sendBuff, "404 - Not Found", "text/html");
            buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>404 Not Found</h1><p>The request failed - file not found.</p></body></html>", true);
        }
        else
        {
            stringstream bufferFileContent;
            bufferFileContent << file.rdbuf();
            string fileContent = bufferFileContent.str();
            if (methodType == SEND_HEAD)
            {
                buildHeaderForRes(sendBuff, "204 - No Content", "text/html");
                sendBuff += "\r\n";
            }
            else
            {
                buildHeaderForRes(sendBuff, "200 - OK", "text/html");
                buildBodyForRes(sendBuff, fileContent, true);
            }
            file.close();
        }
    }
    else
    {
        buildHeaderForRes(sendBuff, "400 - Bad Request", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>400 - Bad Request</h1><p>The request failed - server support only text/html files</p></body></html>", true);
    }
}
bool checkIsResourceHtml(const string& fullResourcePath)
{
    string extension = fullResourcePath.substr(fullResourcePath.find_last_of('.') + 1);
    return extension == "html";
}
bool fileExists(const string& path)
{
    std::ifstream file(path);
    return file.is_open();
}
void handlePostRequest(string& socketBuff, string& sendBuff)
{
    string queryStr, resourcePath, fullResourcePath;
    resourcePath = getResourceNameAndQueryFromReq(socketBuff, queryStr);
    fullResourcePath = ROOT_PATH + getQueryStringValue(queryStr) + resourcePath;
    string target_key = "Content-Length";
    string content_length = getHeader(socketBuff, target_key);

    // Check if the file already exists
    if (fileExists(fullResourcePath))
    {
        buildHeaderForRes(sendBuff, "400 - Bad Request", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>400 - Bad Request</h1><p>The request failed - file already exists</p></body></html>", true);
        removeHeadersAndBodyFromBuffer(socketBuff);
        return;
    }
    // Create and open the file
    ofstream file(fullResourcePath);
    if (!file.is_open())
    {
        buildHeaderForRes(sendBuff, "500 - Internal Server Error", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>500 - Internal Server Error</h1><p>The request failed - unable to open file for writing</p></body></html>", true);
        removeHeadersAndBodyFromBuffer(socketBuff);
        return;
    }

    // Write to the file
    string fileContent = saveBodyToFile(socketBuff, file, stoi(content_length));
    if (fileContent.empty()) {
        buildHeaderForRes(sendBuff, "400 - Bad Request", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>400 - Bad Request</h1><p>The request failed - post can't be without body</p></body></html>", true);
        removeHeadersFromBuffer(socketBuff);
        return;
    }

    buildHeaderForRes(sendBuff, "201 - Created", "text/html");
    string HTMLbody = makePostHTMLbody(fullResourcePath, fileContent);
    buildBodyForRes(sendBuff, HTMLbody, false);//need to undrestand 
}
void handlePutRequst(string& socketBuff, string& sendBuff)
{
    string queryStr, resourcePath, fullResourcePath;
    resourcePath = getResourceNameAndQueryFromReq(socketBuff, queryStr);
    fullResourcePath = ROOT_PATH + getQueryStringValue(queryStr) + resourcePath;
    string target_key = "Content-Length";
    string content_length = getHeader(socketBuff, target_key);

    string status;
    // Create and open the file
    ofstream file(fullResourcePath);
    if (fileExists(fullResourcePath))
        status = "204 - No Content";
    else
        status = "201 - Created";

    if (!file.is_open())
    {
        buildHeaderForRes(sendBuff, "500 - Internal Server Error", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>500 - Internal Server Error</h1><p>The request failed - unable to open file for writing</p></body></html>", true);
        removeHeadersAndBodyFromBuffer(socketBuff);
        return;
    }

    // Write to the file
    if (stoi(content_length) == 0)
    {
        removeHeadersFromBuffer(socketBuff); //remove headers
    }
    else
    {
        string fileContent = saveBodyToFile(socketBuff, file, stoi(content_length)); // remove body+headers
    }
    buildHeaderForRes(sendBuff, status, "text/plain");
    buildBodyForRes(sendBuff, "", false);
}
void handleOptionsRequest(string& socketBuff, string& sendBuff)
{
    removeHeadersFromBuffer(socketBuff);
    buildHeaderForRes(sendBuff, "200 - OK", "text/plain");
    // Add Allow line to response
    sendBuff += "Allow: GET, POST, OPTIONS, TRACE, HEAD, PUT, DELETE\r\n";
    // Add Content-Length to response
    sendBuff += "Content-Length: 0\r\n\r\n";
}
void handleTraceRequest(string& socketBuff, string& sendBuff)
{
    // no body in trace req
    size_t bodyPos = socketBuff.find("\r\n\r\n");
    bodyPos += 4; // Move past "\r\n\r\n"
    buildHeaderForRes(sendBuff, "200 - OK", "message/http");
    buildBodyForRes(sendBuff, "TRACE" + socketBuff.substr(0, bodyPos), false);//the socketBuff contain /r/n/r/n so false
    socketBuff = socketBuff.substr(bodyPos); // remove headers+body 
}
void handleUnKnownMethod(string& socketBuff, string& sendBuff)
{
    removeHeadersAndBodyFromBuffer(socketBuff);
    buildHeaderForRes(sendBuff, "405 - Method Not Allowed", "text/plain");
    sendBuff += "Server Allow: GET, POST, OPTIONS, TRACE, HEAD, PUT, DELETE\r\n";
    // Add Content-Length to response
    sendBuff += "Content-Length: 0\r\n\r\n";
}
void handleDeleteRequest(string& socketBuff, string& sendBuff)
{
    string queryStr, resourcePath, fullResourcePath;
    resourcePath = getResourceNameAndQueryFromReq(socketBuff, queryStr);
    fullResourcePath = ROOT_PATH + getQueryStringValue(queryStr) + resourcePath;
    removeHeadersFromBuffer(socketBuff);
    // Check if the file already exists
    if (!fileExists(fullResourcePath)) {
        buildHeaderForRes(sendBuff, "404 - Not Found", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>404 - Not Found</h1><p>Cant delete the file because the requested file does not exist</p></body></html>", true);
        return;
    }
    // Attempt to delete the file
    if (remove(fullResourcePath.c_str()) != 0)
    {
        buildHeaderForRes(sendBuff, "500 - Internal Server Error", "text/html");
        buildBodyForRes(sendBuff, "<html><head><title>Error!</title></head><body><h1>500 - Internal Server Error</h1><p>Unable to delete the file</p></body></html>", true);
        return;
    }

    // Successfully deleted the file
    buildHeaderForRes(sendBuff, "200 - OK", "text/html");
    buildBodyForRes(sendBuff, "<html><head><title>File Deleted</title></head><body><h1>200 - OK</h1><p>The file was successfully deleted</p></body></html>", true);
}
string makePostHTMLbody(const string& filePath, const string& body)
{
    std::ostringstream htmlBody;
    // Send HTML content to the stream
    htmlBody << "<!DOCTYPE html>\n";
    htmlBody << "<html>\n" << "<head>\n";
    htmlBody << "<title>File Created</title>\n";
    htmlBody << "</head>\n" << "<body>\n";
    htmlBody << "<h1>File Created Successfully</h1>\n";
    htmlBody << "<p>The file has been created successfully at <a>" << filePath << "</a>.</p>\n";
    htmlBody << "<p>The Content in the file is \"<a>" << body << " \"</a>.</p>\n";
    htmlBody << "</body>\n" << "</html>\n";

    return htmlBody.str();
}
string saveBodyToFile(string& buffer, ofstream& outputFile, int content_length) {

    size_t bodyPos = buffer.find("\r\n\r\n");
    string content;

    if (bodyPos != std::string::npos) {
        bodyPos += 4; // Move past "\r\n\r\n"
        buffer = buffer.substr(bodyPos); //remove the headers before the body from buffer
        content = buffer.substr(0, content_length);
        buffer = buffer.substr(content_length); //remover the body from the buffer 
        content.erase(std::remove(content.begin(), content.end(), '\r'), content.end()); // Remove all '\r' characters
        outputFile << content;
        outputFile.flush();
    }
    return content;
}
string getHeader(const string& socketBuffer, const string& targetKey) {
    size_t pos = socketBuffer.find("\r\n");
    size_t start = pos + 2; // Skip \r\n
    while ((pos = socketBuffer.find("\r\n", start)) != std::string::npos)
    {
        std::string header = socketBuffer.substr(start, pos - start);
        size_t separatorPos = header.find(": ");
        if (separatorPos != std::string::npos) {
            std::string key = header.substr(0, separatorPos);
            if (key == targetKey)
            {
                return header.substr(separatorPos + 2);
            }
        }
        start = pos + 2; // Move to the next header
    }
    return ""; // Return an empty string if the target key is not found
}
void buildHeaderForRes(string& httpResponse, const string& httpStatus, const string& contentType)
{
    ostringstream headerStream;
    // Get current time    
    time_t currentTime = std::time(nullptr);
    tm* gmtTime = std::gmtime(&currentTime);
    // Format time to string    
    headerStream << "HTTP/1.1 " << httpStatus << "\r\n" << "Server: WebHTTPServer\r\n"
        << "Content-Type: " << contentType << "\r\n" << "Date-Time: "
        << std::put_time(gmtTime, "%a, %d %b %Y %H:%M:%S GMT")
        << "\r\n";
    httpResponse = headerStream.str();
}
void buildBodyForRes(string& httpResponse, const string& contentBody, bool appendSuffix)
{
    ostringstream bodyStream;
    // Add Content-Length to header    
    bodyStream << "Content-Length: " << contentBody.length() << "\r\n\r\n" << contentBody;
    // Add suffix if required
    if (appendSuffix)
    {
        bodyStream << "\r\n\r\n";
    }
    httpResponse += bodyStream.str();
}
void initMethodType(METHOD methodType, int sizeMethod, int index, SocketState* sockets)
{
    sockets[index].send = SEND;
    sockets[index].method = methodType;
    sockets[index].buffer = sockets[index].buffer.substr(sizeMethod);
}
string getResourceNameAndQueryFromReq(string& socketBuffer, string& queryString)
{
    istringstream iss(socketBuffer);

    string resourceAndQuery, resourcePath;

    iss >> resourceAndQuery;
    size_t queryPos = resourceAndQuery.find('?');

    if (queryPos != string::npos)
    {
        resourcePath = resourceAndQuery.substr(0, queryPos); // Extract resource path
        queryString = resourceAndQuery.substr(queryPos + 1); // Extract query string
    }
    else
    {
        resourcePath = resourceAndQuery;
    }
    return resourcePath;
}
string getQueryStringValue(const string& queryStr)
{
    string value = "he";
    // Split query string into individual parameters
    istringstream iss(queryStr);
    string parameter;

    while (getline(iss, parameter, '&'))
    {
        // Spliting parameter to key and value pair
        size_t equal_pos = parameter.find('=');
        if (equal_pos != string::npos)
        {
            string key = parameter.substr(0, equal_pos);
            if (key == "lang")
            {
                value = parameter.substr(equal_pos + 1);
            }
        }
    }
    return "/" + value + "/";
}
void removeHeadersFromBuffer(string& socketBuffer)
{
    size_t bodyPos = socketBuffer.find("\r\n\r\n");
    bodyPos += 4; // Move past "\r\n\r\n"
    socketBuffer = socketBuffer.substr(bodyPos); //remove the headers from buffer
}
void removeHeadersAndBodyFromBuffer(string& socketBuffer)
{
    string target_key = "Content-Length";
    string content_length = getHeader(socketBuffer, target_key);
    size_t bodyPos = socketBuffer.find("\r\n\r\n");
    bodyPos += 4; // Move past "\r\n\r\n"
    bodyPos += stoi(content_length);
    socketBuffer = socketBuffer.substr(bodyPos); // remove headers+body 
}
