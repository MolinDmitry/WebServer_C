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

typedef struct{
    char** strings;
    size_t size;
} StringVectorTypeDef;




// Прототипы функций
StringVectorTypeDef splitString(char* str, char delim);
char* copyString(char* str);
void deleteStringVector(StringVectorTypeDef* strVect);
void deleteQueryStruct(QueryStructTypeDef* queryStruct);
char* getQueryString(char* query);
QueryStructTypeDef parseQuery(char* stringLine);


//=============================================================================================
/**
 * @brief Возвращает структуру с массивом указателей на строки, выделенные из str по разделителю delim
 * @param str - разделяемая строка
 * @param delim - символ разделителя
 * @return структура типа  StringVectorTypeDef
 */
StringVectorTypeDef splitString(char* str, char delim){
    StringVectorTypeDef stringVector;
    stringVector.strings = NULL;
    stringVector.size = 0;
    size_t strLength = strlen(str);
    if (strLength == 0) return stringVector;
    uint8_t startSubStringFlag = 0;
    size_t subStrLength = 0;
    char* currentSubString = NULL;
    for(size_t i = 0; i < strLength; i++){
        if (str[i] != delim){            
            if (!startSubStringFlag){
                startSubStringFlag = 1;
                stringVector.size++;                  
            }
            subStrLength++;
            currentSubString = realloc(currentSubString, subStrLength);
            currentSubString[subStrLength-1] = str[i];
            if (i == (strLength-1)){
                    currentSubString = realloc(currentSubString, subStrLength+1);
                    currentSubString[subStrLength] = '\0'; // завершаем строку нулевым символом
                    stringVector.strings = realloc(stringVector.strings, stringVector.size*sizeof(char*));
                    stringVector.strings[stringVector.size - 1] = currentSubString;
                }
        }
        else{
            if (startSubStringFlag){
                startSubStringFlag = 0;
                currentSubString = realloc(currentSubString, subStrLength+1);
                currentSubString[subStrLength] = '\0'; // завершаем строку нулевым символом
                stringVector.strings = realloc(stringVector.strings, stringVector.size*sizeof(char*));
                stringVector.strings[stringVector.size - 1] = currentSubString;
                currentSubString = NULL;
                subStrLength = 0;                
            }
        }
    }
    return stringVector;
}
//=============================================================================================
/**
 * @brief Возвращает копию строки с нулевым символом в конце
 * @param str - копируемая строка
 * @return указатель на копию строки
 *  */
char* copyString(char* str){
    size_t pathLen = strlen(str);
    // копируем строку в возвращаемый объект
    char* resultString = calloc(pathLen+1, sizeof(char));
    strncpy(resultString, str, pathLen);
    resultString[pathLen] = '\0';
    return resultString;    
}

//=============================================================================================
void deleteStringVector(StringVectorTypeDef* strVect){
    for(size_t i = 0; i < (*strVect).size; i++){
        if ((*strVect).strings[i] != NULL){
            free((*strVect).strings[i]);
            (*strVect).strings[i] = NULL;
        }            
    }
    (*strVect).size = 0;
}

//=============================================================================================
void deleteQueryStruct(QueryStructTypeDef* queryStruct){
    if ((*queryStruct).pathString != NULL){
        free((*queryStruct).pathString);
        (*queryStruct).pathString = NULL;
    }
        
    for(size_t i = 0; i < (*queryStruct).numQueryParameters; i++){
        if ((*queryStruct).parameters[i].parameterNameString != NULL){
            free((*queryStruct).parameters[i].parameterNameString);
            (*queryStruct).parameters[i].parameterNameString = NULL;
        }
            
        if ((*queryStruct).parameters[i].valueString != NULL){
            free((*queryStruct).parameters[i].valueString);
            (*queryStruct).parameters[i].valueString = NULL;
        }
            
    }
    (*queryStruct).numQueryParameters = 0;
}

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
    outObject.numQueryParameters = 0;
    outObject.pathString = NULL;
    outObject.parameters = NULL;

    size_t queryStringLength = strlen(queryString);
    if (queryStringLength == 0){
        return outObject;
    }
    // разделяем строку на подстроки по пробелам
    StringVectorTypeDef strVect = splitString(queryString, ' ');
    if (strVect.size < 3){
        printf("Error! Wrong query format\n");
        return outObject;
    }
    // выделяем path и параметры запроса
    StringVectorTypeDef strVect2 = splitString(strVect.strings[1], '?');
    if (strVect2.size == 0){
        printf("Error! Wrong path format\n");
        return outObject;
    }
    // запрос без параметров
    // копируем строку в возвращаемый объект
    outObject.pathString = copyString(strVect2.strings[0]);
    
    if (strVect2.size > 1){
        // запрос с параметрами 
        deleteStringVector(&strVect);
        strVect = splitString(strVect2.strings[1],'&');
        deleteStringVector(&strVect2);
        if (strVect.size == 0){
            printf("Error! Wrong parameters format\n");
            return outObject;
        }
        outObject.numQueryParameters = strVect.size;
        outObject.parameters = calloc(outObject.numQueryParameters, sizeof(QueryParameterTypeDef));
        for(size_t i = 0; i < outObject.numQueryParameters; i++){
            StringVectorTypeDef parameterStringVector = splitString(strVect.strings[i], '=');
            if (parameterStringVector.size < 2){
                printf("Error! Wrong \'%s\' parameter format\n", strVect.strings[i]);
                return outObject;
            }
            outObject.parameters[i].parameterNameString =  copyString(parameterStringVector.strings[0]);
            outObject.parameters[i].valueString =  copyString(parameterStringVector.strings[1]);
            deleteStringVector(&parameterStringVector);
        }
        deleteStringVector(&strVect);        
    } 
   return outObject;
}

//=============================================================================================
