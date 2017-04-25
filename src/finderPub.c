#include <stdio.h>
#include <stdlib.h>
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

int checkDomain(char* requestTerm,char* domainList){
    const char s[2] = "|";
    char* token = strtok(domainList, s);
    while( token != NULL )
    {
        if(strstrIndex(token,requestTerm)!=-1 && strchrIndex('~',token)!=0)return 1;
        token = strtok(NULL, s);
    }
    return 0;
}

int isItPubRequestForFile(char* fileName, char* requestTerm){

    FILE * fp;
    char *line;
    size_t len = 0;
    ssize_t read;
    char* urlRequest=malloc(sizeof(char)*strlen(requestTerm));
    strcpy(urlRequest,requestTerm);
    
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
    
    read = getline(&line, &len, fp);
    int dbgCount = 0;
    int i;
    while (read > 0) {
		str_replace(line,"\n" ,"");
        str_replace(line, "^*","*");

		//printf("isItPubRequestForFile methode :%s \n\n",line);
		dbgCount ++;
		if (strchrIndex('!',line)!=0) // les lignes commencants par ! sont des commentaires ->  a ignorer
		{
            

			if(strchrIndex('@',line)==0) line=substring(line,1,strlen(line));
			
			if(strchrIndex('@',line)==0) line=substring(line,1,strlen(line));

			if(strchrIndex('|',line)==0) line=substring(line,1,strlen(line));
			
			if(strchrIndex('|',line)==0) line=substring(line,1,strlen(line));
            

            str_replace(line,"^$","$");
            i=strstrIndex("$",line);
            
            if(i>=0){
                char* infos=substring(line,i+1,strlen(line));
                int j;
                int check=1;
                
                j=strstrIndex("domain=",infos);
                
                /*
                if(strstr(line,"amazonproducts.js")){
                    printf("line = %s\n",infos);
                }*/
                
                if(j>=0){
                    check=checkDomain(urlRequest,substring(infos,j+7,strlen(infos)));
                    
                   
                }
                /*
                j=strstrIndex("document=",infos);
                if(i>=0)line=substring(line,0,i);
                
                j=strstrIndex("popup=",infos);
                if(i>=0)line=substring(line,0,i);
                
                i=strstrIndex("third-party",line);*/
                
                if(check){
                    line=substring(line,0,i);
                }
                
            }
            

            if(matchSpecialExpression(line,urlRequest) || strstr(line, urlRequest))
            {
                printf("line rejete = %s  requestTerm = %s count =%d\n",line,urlRequest, dbgCount);

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
    
    return 0;
}

int isItPubRequest(char* requestTerm){

    return isItPubRequestForFile("easylist.txt",requestTerm);
}

/*
int main(int argc, char** argv){
    
    printf("is \"%s\"  advertisement? \n",argv[1]);
    int isIt = isItPubRequest(argv[1]);
    printf("=> %d\n",isIt);
}
*/



