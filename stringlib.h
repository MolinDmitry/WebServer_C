

char* appendStr(char* targetStr, const char* appendStr);
void sendAnswer(SOCKET* clientSocket, char* response_body, uint8_t contentType, char* statusString);

//========================================================================================
char* appendStr(char* targetStr, const char* appendStr){
        char* resultString = NULL;
        if (appendStr  == NULL)
        return targetStr;
        size_t appendStringLength = strlen(appendStr);
        size_t targetStringLength = 0;
        if (targetStr == NULL) 
            resultString = calloc(appendStringLength+1, sizeof(char));
        else{
            targetStringLength = strlen(targetStr);
            resultString = (char*)realloc(targetStr, (targetStringLength + appendStringLength + 1)*sizeof(char));
        }
        for(size_t i = 0; i < appendStringLength; i++){
            resultString[targetStringLength + i] = appendStr[i];
        }
        resultString[targetStringLength + appendStringLength] = '\0';
        return resultString;              
}


//========================================================================================
void sendAnswer(SOCKET* clientSocket, char* response_body, uint8_t contentType, char* statusString){
    char* response = NULL;
    // Формируем весь ответ вместе с заголовками
    response = appendStr(response, "HTTP/1.1 ");
    response = appendStr(response, statusString);
    response = appendStr(response, "\r\n");
    response = appendStr(response, "Version: HTTP/1.1\r\n");
    response = appendStr(response, "Access-Control-Allow-Origin: http://localhost:8000\r\n");
    response = appendStr(response, "Access-Control-Allow-Methods: GET, POST\r\n");
    response = appendStr(response, "Access-Control-Allow-Headers: Content-Type\r\n");
    if (!contentType)
        response =  appendStr(response, "Content-Type: text/html; charset=utf-8\r\n");
    else
        response =  appendStr(response, "Content-Type: application/json; charset=utf-8\r\n");
    response =  appendStr(response, "Content-Length: ");
    char contentLengthStr[100] = "";
    snprintf(contentLengthStr, sizeof(contentLengthStr), "%d", strlen(response_body));
    response =  appendStr(response, contentLengthStr);
    response =  appendStr(response, "\r\n\r\n");
    response =  appendStr(response, response_body);

    // Отправляем ответ клиенту с помощью функции send
    int result = send(*clientSocket, response, strlen(response), 0);
    if (result == SOCKET_ERROR) {
        // произошла ошибка при отправке данных
        printf("send failed: %d\n", WSAGetLastError());
    }
    free(response);
    free(response_body);
    response = NULL;
    response_body = NULL;
}

