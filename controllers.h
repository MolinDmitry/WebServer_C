#include <stdint.h>
#include <stdio.h>


const uint8_t CONTENT_TYPE_HTML = 0;
const uint8_t CONTENT_TYPE_JSON = 1;


void testpageController(SOCKET* clientSocket);
void execController(SOCKET* client_socket, QueryStructTypeDef* query);
void faviconController(SOCKET* client_socket);
void defaultController(SOCKET* client_socket);




//============================================================================================
void testpageController(SOCKET* clientSocket){
    char* response_body = NULL;                            
    char fileReadingBuffer[256];                
    FILE* fp = fopen("..\\pages\\testpage.html", "r");
    if (fp == NULL){
        printf("Error during file open operation!\n");
        response_body = appendStr(response_body, "<html><body><h2>Error 404. Page is not found</h2></body</html>\n");
    }
    else{
        while(fgets(fileReadingBuffer, 256, fp) != NULL){
            response_body = appendStr(response_body, fileReadingBuffer);
        }
        fclose(fp);
    }                
    sendAnswer(clientSocket, response_body, CONTENT_TYPE_HTML, "200 OK");
    if (response_body != NULL) free(response_body);
}

//============================================================================================
void execController(SOCKET* client_socket, QueryStructTypeDef* query){
    //char* response_body = NULL;    
    char buf[1024];    
    static size_t paramValue1 = 0;
    static size_t paramValue2 = 0;
    //snprintf(buf, 1024, "{\"param1\":%d,\"param2\":%d}\n", paramValue1, paramValue2);
    //response_body = appendStr(response_body, buf);
    //sendAnswer(client_socket, response_body, CONTENT_TYPE_JSON, "200 OK");

    // ============== Начало теста для поиска утечек памяти =============================================
    char response[4096];
    char response_body[1024];
    snprintf(response_body, 1024, "{\"param1\":%d,\"param2\":%d}\n", paramValue1, paramValue2);
    size_t response_body_length = strlen(response_body);
    snprintf(response, 4096, "HTTP/1.1 200 OK \r\n\
Version: HTTP/1.1\r\n\
Access-Control-Allow-Origin: http://localhost:8000\r\n\
Access-Control-Allow-Methods: GET, POST\r\n\
Access-Control-Allow-Headers: Content-Type\r\n\
Content-Type: application/json; charset=utf-8\r\n\
Content-Length: %d\r\n\r\n\
{\"param1\":%d,\"param2\":%d}\n\
", response_body_length, paramValue1, paramValue2);

    // Отправляем ответ клиенту с помощью функции send
    int result = send(*client_socket, response, strlen(response), 0);
    if (result == SOCKET_ERROR) {
        // произошла ошибка при отправке данных
        printf("send failed: %d\n", WSAGetLastError());
    }
    // ============== Конец теста для поиска утечек памяти =============================================
    



    paramValue1++;
    //if (response_body != NULL) free(response_body);
}

//============================================================================================
void faviconController(SOCKET* client_socket){
    char* response_body = NULL;
    response_body = appendStr(response_body, "\n");
    sendAnswer(client_socket, response_body, CONTENT_TYPE_HTML, "404 Not found");
    if (response_body != NULL) free(response_body);
}

//============================================================================================
void defaultController(SOCKET* client_socket){
    char* response_body = NULL;
    response_body = appendStr(response_body, "<html><body><h2>Error 404. Page is not found</h2></body</html>\n");
    sendAnswer(client_socket, response_body, CONTENT_TYPE_HTML, "404 Not found");
    if (response_body != NULL) free(response_body);
}