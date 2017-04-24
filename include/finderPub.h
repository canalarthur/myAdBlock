#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string-utils.h"

static const char* DIRECTORY_LIST="bannerLists/";

//verifie si le deuxieme argument verifier l'expression reguliere du second argument
int matchSpecialExpression(char* strRegex, char*str);

int isItPubRequestForFile(char* fileName, char* requestTerm);

int isItPubRequest(char* requestTerm);




