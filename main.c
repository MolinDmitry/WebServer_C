#include <stdio.h>
#include <stdint.h>


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>


// Необходимо, чтобы линковка происходила с DLL-библиотекой 
// Для работы с сокетами
// также для успешной компиляции необходимо добавить аргумент компилятора -lws2_32 самым последним (в файле tasks.json)
#pragma comment(lib, "libws2_32.a")
#include "queryparser.h"
#include "stringlib.h"
#include "controllers.h"


const char SERV_PORT[] = "8000";

//========================================================================================

void printStringVector(StringVectorTypeDef strVect){
    for(size_t i = 0; i < strVect.size; i++){
        printf("%s\n", strVect.strings[i]);
    }
}


//========================================================================================

int main(int argc, char* argv[]){
    printf("Demo Web-server\n");
    // TCHAR buffer[MAX_PATH];
	// GetCurrentDirectory(sizeof(buffer),buffer);
    // printf("%s", buffer);
    

    // char testString1[] = "GET /demoserver/exec?command=com&param=value HTTP/1.1";
    // char testString2[] = "GET /testpage HTTP/1.1";
    // char testString3[] = "GET /testpage/ HTTP/1.1";
    // char testString4[] = "  abc def 123  ";
    // char testString5[] = "exec?command=com&param=value";
    // char testString6[] = "command=com&param=value";
    // char testString7[] = "param=value";
    // char testQuery1[] = "GET /demoserver/exec?command=com&param=value HTTP/1.1\r\nSecond row\r\nThird row\r\n";
    // char testQuery2[] = "GET /testpage HTTP/1.1\r\nSecond row\r\nThird row\r\n";

    // char* queryString = getQueryString(testQuery2);
    // printf("query string: %s\n", queryString);
    // QueryStructTypeDef query = parseQuery(queryString);
    // free(queryString);
    // printf("path: %s\n",query.pathString);
    // for(size_t i = 0; i < query.numQueryParameters; i++){
    //     printf("parameter %d name: %s\n", i, query.parameters[i].parameterNameString);
    //     printf("parameter %d value: %s\n", i, query.parameters[i].valueString);
    // }
    // deleteQueryStruct(&query);


    // StringVectorTypeDef strVect = splitString(testString4, ' ');
    // printStringVector(strVect);
    // deleteStringVector(&strVect);
    // strVect = splitString(testString5, '?');
    // printStringVector(strVect);
    // deleteStringVector(&strVect);
    // strVect = splitString(testString6, '&');
    // printStringVector(strVect);
    // deleteStringVector(&strVect);
    // strVect = splitString(testString7, '=');
    // printStringVector(strVect);
    // deleteStringVector(&strVect);

    // QueryStructTypeDef query = parseQuery(testString5);
    // printf("%s\n",query.pathString);
    // for(size_t i = 0; i < query.numQueryParameters; i++){
    //     printf("%s\n",query.parameters[i].parameterNameString);
    //     printf("%s\n",query.parameters[i].valueString);
    // }
    // deleteQueryStruct(&query);


    //return 0;

    // служебная структура для хранения информации
    // о реализации Windows Sockets
    WSADATA wsaData;

    // старт использования библиотеки сокетов процессом
    // (подгружается Ws2_32.dll)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Если произошла ошибка подгрузки библиотеки
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return result;
    }

    struct addrinfo* addr = NULL; // структура, хранящая информацию
    // об IP-адресе  слущающего сокета

    // Шаблон для инициализации структуры адреса
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    // AF_INET определяет, что используется сеть для работы с сокетом
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; // Задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP; // Используем протокол TCP
    // Сокет биндится на адрес, чтобы принимать входящие соединения
    hints.ai_flags = AI_PASSIVE;

    // Инициализируем структуру, хранящую адрес сокета - addr.
    // HTTP-сервер будет висеть на 8000-м порту локалхоста
    result = getaddrinfo("127.0.0.1", SERV_PORT, &hints, &addr);

    // Если инициализация структуры адреса завершилась с ошибкой,
    // выведем сообщением об этом и завершим выполнение программы 
    if (result != 0) {
        printf("getaddrinfo failed: %d\n", result);
        WSACleanup(); // выгрузка библиотеки Ws2_32.dll
        return 1;
    }

    // Создание сокета
    int listen_socket = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);
    // Если создание сокета завершилось с ошибкой, выводим сообщение,
    // освобождаем память, выделенную под структуру addr,
    // выгружаем dll-библиотеку и закрываем программу
    if (listen_socket == INVALID_SOCKET) {
        printf("Error at socket: %d\n", result);
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    // Привязываем сокет к IP-адресу
    result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);

    // Если привязать адрес к сокету не удалось, то выводим сообщение
    // об ошибке, освобождаем память, выделенную под структуру addr.
    // и закрываем открытый сокет.
    // Выгружаем DLL-библиотеку из памяти и закрываем программу.
    if (result == SOCKET_ERROR) {
        printf("bind failed with error: %d\n",  WSAGetLastError());
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    // Инициализируем слушающий сокет
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: WSAGetLastError()\n");
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    
    uint8_t finishFlag = 0;
    while(!finishFlag){
        printf("\n\n<==========================================================================================>\n");
        printf("Listening port %s\n", SERV_PORT);
        // Принимаем входящие соединения
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(listen_socket);
            WSACleanup();
            return 1;
        }

        const size_t max_client_buffer_size = 1024;
        char buf[max_client_buffer_size];

        result = recv(client_socket, buf, max_client_buffer_size, 0);

        char* response = NULL; // сторка ответа 
       
        if (result == SOCKET_ERROR) {
            // ошибка получения данных
            printf("recv failed: %d\n");
            closesocket(client_socket);
        } else if (result == 0) {
            // соединение закрыто клиентом
             printf("connection closed...\n");
        } else if (result > 0) {
            // Мы знаем фактический размер полученных данных, поэтому ставим метку конца строки
            // В буфере запроса.
            buf[result] = '\0';

            // печатаем запрос
            // for(size_t i = 0; i < result; i++){
            //     printf("%c", buf[i]);
            // }

            uint8_t contentType = 0;
            // анализ запроса
            char* queryString = getQueryString(buf);
            printf("Query is received: %s", queryString);
            QueryStructTypeDef query = parseQuery(queryString);
            free(queryString);
            printf("Path: %s\n", query.pathString);
            for(size_t i = 0; i < query.numQueryParameters; i++){
                printf("parameter %d name: %s\n", i, query.parameters[i].parameterNameString);
                printf("parameter %d value: %s\n", i, query.parameters[i].valueString);
            }
            
            char* response_body = NULL;
            if (!strcmp(query.pathString, "/testpage")){
                // отвечаем на запрос с путем "/testpage"
                testpageController(&client_socket);
                
            } else if (!strcmp(query.pathString, "/demoserver/exec")){
                execController(&client_socket, &query);
                // contentType = 1;
                // response_body = appendStr(response_body, "{\"param1\":0,\"param2\":1}\n");
                // sendAnswer(&client_socket, response_body, contentType, "200 OK");
            } else if (!strcmp(query.pathString, "/favicon.ico")){
                faviconController(&client_socket);
                // contentType = 0;
                // response_body = appendStr(response_body, "\n");
                // sendAnswer(&client_socket, response_body, contentType, "404 Not found");
            } else{
                // ответ на неизвестный запрос
                defaultController(&client_socket);
                // contentType = 0;
                // response_body = appendStr(response_body, "<html><body><h2>Error 404. Page is not found</h2></body</html>\n");
                // sendAnswer(&client_socket, response_body, contentType, "404 Not found");
            }
            deleteQueryStruct(&query);
            // Закрываем соединение к клиентом
            closesocket(client_socket);    
        }
    }

    // Убираем за собой
    closesocket(listen_socket);
    freeaddrinfo(addr);
    WSACleanup();

    system("pause"); 
    return 0;
}