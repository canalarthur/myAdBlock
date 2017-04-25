#include <stdio.h>
#include <stdlib.h>
#include "../include/string-utils.h"

//retourne l'occurence d'un caracter dans un char*
int strchrIndex(char c, char* array)
{
    const char *ptr = strchr(array, c);
    if(ptr) 
	{
       return ptr - array;
    }
    return -1;
}

//retourne l'occurence d'un char* dans un char*
int strstrIndex(char* str, char* array)
{
    const char *ptr = strstr(array, str);
    if(ptr) 
	{
        return ptr - array;
    }
    return -1;
}

//extrait une chaine de caractere entre deux index
char* substring(char* buff,int index, int indexLast)
{
	char* subbuff=malloc(sizeof(char)*(indexLast-index+1));
	memcpy(subbuff, &buff[index], indexLast-index );
	subbuff[indexLast-index] = '\0';
    return subbuff;
}

//Remplace toChange par replacement dans target.
void str_replace(char *target, const char *toChange, const char *replacement)
{
    char buffer[1024*50] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t toChange_len = strlen(toChange);
    size_t repl_len = strlen(replacement);
    
    while (1)
		{
        const char *p = strstr(tmp, toChange);
        
        // On a passe la derniere occurrence de toChange, on copie la fin.
        if (p == NULL) 
		{
            strcpy(insert_point, tmp);
            break;
        }
        
        // on copie la partie avant toChange
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        
        // on copie le remplacement
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;
        
        tmp = p + toChange_len;
    }
    strcpy(target, buffer);
}

int countOccurences(char* strInitial,char* strRecherche)
{
     char* token = strtok(strInitial, strRecherche);
     int count=0;
 
     while( token != NULL )
     {
        count++;
		printf("Before the occurrence #%d : %s\n",count, token );

		token = strtok(NULL, strRecherche);
     }
     return count;
 }

 //Pareil que replace_str
char* replaceSTR(char* strInitial, char* strRecherche, char* strRemplace)
{
	int nbOccurence=countOccurences(strInitial,strRecherche);
	int longueurResult=strlen(strInitial)-nbOccurence*strlen(strRecherche)+nbOccurence*strlen(strRemplace);
	char* result = malloc(longueurResult* sizeof(char));

	char *token;
	int count=0;

	token = strtok(strInitial, strRecherche);

	while( token != NULL )
	{
		count++;
		printf("Before the occurrence #%d : %s\n",count, token );

		strcat(result,token);
		strcat(result,strRemplace);

		token = strtok(NULL, strRecherche);
	}
	return result;
 }
 


