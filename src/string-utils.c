#include <stdio.h>
#include <stdlib.h>
#include "../include/string-utils.h"


//retourne l'occurence d'un caracter dans un char*
int strchrIndex(char c, char* array)
{
    const char *ptr = strchr(array, c);
    if(ptr) {
       return ptr - array;
    }
    return -1;
}

//extrait une chaine de caractere entre deux index
char* substring(char* buff,int index, int indexLast){
     char* subbuff=malloc(sizeof(char)*(indexLast-index+1));
     memcpy(subbuff, &buff[index], indexLast-index );
     subbuff[indexLast-index] = '\0';
    return subbuff;
}



void str_replace(char *target, const char *needle, const char *replacement)
{
    char buffer[1024*50] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);
    
    while (1) {
        const char *p = strstr(tmp, needle);
        
        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }
        
        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        
        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;
        
        // adjust pointers, move on
        tmp = p + needle_len;
    }
    
    // write altered string back to target
    strcpy(target, buffer);
}


int countOccurences(char* strInitial,char* strRecherche){
     char* token = strtok(strInitial, strRecherche);
     int count=0;
 
     while( token != NULL )
     {
        count++;
         printf("avant la %de occurence: %s\n",count, token );
 
         token = strtok(NULL, strRecherche);
     }
     return count;
 }

char* replaceSTR(char* strInitial, char* strRecherche, char* strRemplace){
 
     int nbOccurence=countOccurences(strInitial,strRecherche);
     int longueurResult=strlen(strInitial)-nbOccurence*strlen(strRecherche)+nbOccurence*strlen(strRemplace);
     char* result = malloc(longueurResult* sizeof(char));
 
     char *token;
     int count=0;
 
     token = strtok(strInitial, strRecherche);
 
     while( token != NULL )
     {
         count++;
         printf("avant la %de occurence: %s\n",count, token );
 
         strcat(result,token);
         strcat(result,strRemplace);
 
         token = strtok(NULL, strRecherche);
     }
     return result;
 }
 


