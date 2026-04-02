#include <stdint.h>
#include <stdio.h>


const uint8_t CONTENT_TYPE_HTML = 0;
const uint8_t CONTENT_TYPE_JSON = 1;


void testpageController(SOCKET* clientSocket);




//============================================================================================
void testpageController(SOCKET* clientSocket){

    uint8_t contentType = 0;
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
    sendAnswer(clientSocket, response_body, contentType, "200 OK");

}

//============================================================================================
