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
//extrait une chaine de caractere entre deux index
char* substring(char* buff,int index, int indexLast){
     char* subbuff=malloc(sizeof(char)*(indexLast-index+1));
     memcpy(subbuff, &buff[index], indexLast-index );
     subbuff[indexLast-index] = '\0';
     return subbuff;
}
//verifie si le deuxieme argument verifier l'expression reguliere du second argument
int matchSpecialExpression(char* strRegex, char*str){
   int testIfRegex=0;
   for(int i=0;i<strlen(strRegex);i++){
        if(strRegex[i]=='*'){
           testIfRegex=1;
           if(matchSpecialExpression(substring(strRegex,0,i),str) ==0 || matchSpecialExpression(substring(strRegex,i+1,strlen(strRegex)),str)==0 ){
                //printf("before * = %s strRegex = %s\n",substring(strRegex,i+1,strlen(strRegex)), strRegex);   
                return 0;
           }
        }
   }
  if(testIfRegex){
     return 1;
   }
  //printf("strRegex = %s, requestTerms = %s \n",strRegex,str);   
   if(strstr(str,strRegex)){
       return 1;
   }else{
       return 0;
   }
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
    //on ouvre le fichier en mode lecture
    fp = fopen(fullPathAdList, "r");

    if (fp == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s, chemin: %s",fileName,fullPathAdList);
        return 0;
    }
    //printf("Request Term: %s\n",requestTerm);
    while ((read = getline(&line, &len, fp)) != -1) {
	line=substring(line,0,strlen(line)-1);
        if (strchrIndex('!',line)!=0) // les lignes commencants par ! sont des commentaires ->  a ignorer
	{   
            if(strchrIndex('@',line)==0)line=substring(line,1,strlen(line));
            if(strchrIndex('@',line)==0)line=substring(line,1,strlen(line));
            if(strchrIndex('|',line)==0)line=substring(line,1,strlen(line));
            if(strchrIndex('|',line)==0)line=substring(line,1,strlen(line));
            if(matchSpecialExpression(line,requestTerm) ||strstr(line, requestTerm) )// soit le terme demandé, est dans easyList )
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
	
        if(argc==3){
           //testRapide
           printf("match regex = %d",matchSpecialExpression(argv[1],argv[2]));
        }else if(argc==2){
	    isItPubRequest(argv[1]);
        }else {
            printf("USAGE: ./finderPub \"PUB A CHERCHER\"\n");
        }
        return 0;
}



