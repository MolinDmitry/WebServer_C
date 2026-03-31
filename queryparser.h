#include <stdio.h>
#include <stdint.h>

// определение структур
typedef struct{
    char* parameterNameString;
    char* valueString;
} QueryParameterTypeDef;

typedef struct{
    char* pathString;
    QueryParameterTypeDef* parameters;
    size_t numQueryParameters;
} QueryStructTypeDef;




// Прототипы функций
char* getQueryString(char* query);
QueryStructTypeDef parseQuery(char* stringLine);
//std::vector<std::string> splitString(char* buf, size_t length);


//=============================================================================================
/**
 * @brief Возвращает первую строку из запроса (строка с адресом и параметрами
 * @param query - строка с содержанием запроса
 * @return - первая строка запроса
 */
char* getQueryString(char* query){
    size_t firstStringLength = 0;
    while((query[firstStringLength] != '\n') && (query[firstStringLength])){
        firstStringLength++;
    }
    if (query[firstStringLength+1]) 
        firstStringLength++;
    char* queryString = malloc(firstStringLength+1); // выделяем память под стоку (плюс нулевой символ в конце)
    for(size_t i = 0; i < firstStringLength; i++){
        queryString[i] = query[i];
    }
    queryString[firstStringLength] = '\0';
    return queryString;
}


//=============================================================================================
QueryStructTypeDef parseQuery(char* queryString){
    QueryStructTypeDef outObject;
    // делим запрос по пробелам
    // проводим подсчёт подстрок
    size_t numSpaceGroups = 0;
    size_t numSubStrings = 0;
    uint8_t spaceInPrevSymbolFlag = 0; // флаг пробела в предыдущем символе
    uint8_t startSpaceFlag = 0; // флаг наличия пробела в начале строки
    uint8_t endSpaceFlag = 0; // флаг наличия пробела в конце строки
    if (queryString[0] == ' ') startSpaceFlag = 1;
    size_t queryStringLength = strlen(queryString);
    if (queryStringLength){
        if (queryString[queryStringLength-1] == ' '){
            endSpaceFlag = 1;
        }
    }    
    // cчитаем количество пробельных групп в строке
    for(size_t i = 0; i < queryStringLength; i++){
        if (queryString[i] == " "){
            if (!spaceInPrevSymbolFlag){
                numSpaceGroups++;
            }
            spaceInPrevSymbolFlag = 1;
        }
        else{
            spaceInPrevSymbolFlag = 0;
        }        
    }
    // вычисляем количество подстрок
    numSubStrings = numSpaceGroups + 1 - startSpaceFlag - endSpaceFlag;

    char* stringVector = calloc(numSubStrings, sizeof(char*));
    size_t subStrLength = 0;
    uint8_t startSubStringFlag = 0;
    size_t subStringCounter = 0;
    for(size_t i = 0; i < queryStringLength; i++){
        if (queryString[i] != " "){
            if (!startSubStringFlag){
                startSubStringFlag = 1;
                subStrLength++;
                subStringCounter++;
            }
        }
    }
                        
    

    

    // выделяем path и параметры запроса


}

//=============================================================================================
