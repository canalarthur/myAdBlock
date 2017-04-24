#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/finderPub.h"

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

    printf("**********************************************\nPUB CHECKER\n**********************************************\n\n");
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    //attention, attention les trois blocs qui suivent servent a concatener une chaine de caractere en c
    char* fullPathAdList = malloc(strlen(DIRECTORY_LIST)+1+strlen(fileName)); 
    strcpy(fullPathAdList, DIRECTORY_LIST); /* copier le directory */
    strcat(fullPathAdList, fileName); /* coptier le nom du fichier */
    fullPathAdList[strlen(DIRECTORY_LIST)+strlen(fileName)]='\0';

    //on ouvre le fichier en mode lecture
    fp = fopen(fullPathAdList, "r");

    if (fp == NULL)
    {
        printf("Impossible d'ouvrir le fichier %s, chemin: %s",fileName,fullPathAdList);
        return 0;
    }
    
    printf("isItPubRequestForFile methode 3 \n\n");
    printf("File Search: %s\n",fullPathAdList);
    printf("Request Term: %s\n",requestTerm);
    
    while ((read = getline(&line, &len, fp)) != -1) {
        //str_replace(line,"\n","");
        //printf("isItPubRequestForFile methode :%s \n\n",line);

        if (strchrIndex('!',line)!=0) // les lignes commencants par ! sont des commentaires ->  a ignorer
        {
            if(strchrIndex('@',line)==0)line=substring(line,1,strlen(line));
            if(strchrIndex('@',line)==0)line=substring(line,1,strlen(line));
            if(strchrIndex('|',line)==0)line=substring(line,1,strlen(line));
            if(strchrIndex('|',line)==0)line=substring(line,1,strlen(line));
            
            if(matchSpecialExpression(line,requestTerm) || strstr(line, requestTerm))
            {
                printf("Pub repéré : %s is in \"%s\" \n", requestTerm, line);
	            return 1;
            }
           
       }
    }

    if(line){
        free(line);
    }
    fclose(fp);
    printf("Pas de pub reperé %s pour le mot clé %s\n",fileName,requestTerm);
    
    printf("\n\n**********************************************\n\n");
    return 0; 
}

int isItPubRequest(char* requestTerm){   
    return isItPubRequestForFile("easylist.txt",requestTerm);	
}





