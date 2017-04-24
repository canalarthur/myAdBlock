#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/finderPub.h"

//verifie si le deuxieme argument verifier l'expression reguliere du second argument
int matchSpecialExpression(char* strRegex, char*str){
	int testIfRegex=0;
	int i,left,right;
	for(i=0;i<strlen(strRegex);i++){
		if(strRegex[i]=='*'){
			testIfRegex=1;
			left = matchSpecialExpression(substring(strRegex,0,i),str);
			right = matchSpecialExpression(substring(strRegex,i+1,strlen(strRegex)),str);
			if( left == 0 || right == 0 ){
				//printf("before * = %s strRegex = %s\n",substring(strRegex,i+1,strlen(strRegex)), strRegex);   
				return 0;
			}
		}	
	}
	if(testIfRegex) return 1;
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
    char *line;
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
    read = getline(&line, &len, fp);
    printf("\tDEBUG : first getline passed\n");
    int dbgCount = 0;
    
    while (read > 0) {
		//str_replace(line,"\n","");
		//printf("isItPubRequestForFile methode :%s \n\n",line);
		dbgCount ++;
		if (strchrIndex('!',line)!=0) // les lignes commencants par ! sont des commentaires ->  a ignorer
		{
			printf("\tDEBUG : notcom line %d : comparing |%s| to |%s|\n",dbgCount,requestTerm,line);
			
			if(strchrIndex('@',line)==0) line=substring(line,1,strlen(line));
			
			if(strchrIndex('@',line)==0) line=substring(line,1,strlen(line));

			if(strchrIndex('|',line)==0) line=substring(line,1,strlen(line));
			
			if(strchrIndex('|',line)==0) line=substring(line,1,strlen(line));
			
			if(matchSpecialExpression(line,requestTerm) || strstr(line, requestTerm))
			{
				printf("Pub repérée : %s is in \"%s\" \n", requestTerm, line);
				return 1;
			}
		}
		free(line);
		line = NULL;
		read = getline(&line, &len, fp);   
    }
    if(line){
        free(line);
    }
    fclose(fp);
    printf("Pas de pub reperée %s pour le mot clé %s\n",fileName,requestTerm);
    
    printf("\n\n**********************************************\n\n");
    return 0; 
}

int isItPubRequest(char* requestTerm){   
    return isItPubRequestForFile("easylist.txt",requestTerm);	
}





