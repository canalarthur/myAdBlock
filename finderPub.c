#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* DIRECTORY_LIST="bannerLists/";

int strchrIndex(char c, char* array)
{
    const char *ptr = strchr(array, c);
    if(ptr) {
       return ptr - array;
    }
    return -1;
}
int isItPubRequestForFile(char* fileName, char* requestTerm){

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    //attention, attention les trois blocs qui suivent servent a concatener une chaine de caractere en c
    char* fullPathAdList = malloc(strlen(DIRECTORY_LIST)+1+strlen(fileName)); 
    strcpy(fullPathAdList, DIRECTORY_LIST); /* copier le directory */
    strcat(fullPathAdList, fileName); /* coptier le nom du fichier */
    fp = fopen(fullPathAdList, "r");

    if (fp == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s, chemin: %s",fileName,fullPathAdList);
        return 0;
    }
   //printf("Request Term: %s\n",requestTerm);
    while ((read = getline(&line, &len, fp)) != -1) {
	if (strchrIndex('!',line)!=0) // les lignes commencants par ! sont des commentaires ->  a ignorer
	{   
            if (strstr(line, requestTerm) )// soit le terme demandé, est dans easyList )
            {
                printf("Pub repéré : %s is in \"%s\" \n", requestTerm, line);
	        return 1;
            }
       }
    }

    fclose(fp);
    if (line)
        free(line);
    printf("Pas de pub reperé %s pour le mot clé %s\n",fileName,requestTerm);
    return 0; 
}

int isItPubRequest(char* requestTerm){   
    return isItPubRequestForFile("easylist.txt",requestTerm);	
}


int main(int argc,char** argv){
	if(argc==2){
	    isItPubRequest(argv[1]);
        }else {
            printf("USAGE: ./finderPub \"PUB A CHERCHER\"\n");
        }
        return 0;
}



