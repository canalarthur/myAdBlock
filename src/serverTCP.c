#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>

#include "finderPub.c"
#include "string-utils.c"

//Le nombre maxmimal de threads, avant que l'on attende qu'un se termine.
//-1 = AUCUN THREAD
#define MAX_NUMBER_OF_THREADS 4

//COULEURS POUR LES LOGS
#define NORMAL "\x1B[0m"
#define ROUGE  "\x1B[31m"
#define VERT   "\x1B[32m"
#define JAUNE  "\x1B[33m"

char HTTP_PUB_BLOQUE[]=
"HTTP/1.1 404 Not Found\r\n"
"Server: Proxy/1.0\r\n"
"Connection: close\r\n"
"Content-Type: text/html; charset=iso-8859-1\r\n"
"<HTML><BODY><H1>AD BLOCKED</H1></BODY></HTML>";

int countThread=0;
int readline (int fd, char* ptr, int maxlen);
int passRequest(int clientSocket,char* fromClient, char* hostName, int port);

void usage(){
	printf("Use : ./proxy serverPortNumber \n");
}

char * get_host(const char * httpRequest){
    unsigned short  i = 0, j = 0;
    size_t size = 0;
    
    char * bufferHost = strstr(httpRequest, "Host: ");
    char * host;
    
    if( bufferHost == NULL)
	{
        return NULL;
    }
	else
	{
        size = strlen(bufferHost);
        
        while( bufferHost[i] != '\n' )
		{
            i++;
            if( i >= size ){
                return NULL;
            }
        }
        
        host = calloc(i+1, sizeof(char));
        if(host == NULL)
		{
            printf("get_host : MEMORY ALLOCATON ERROR\n");
            exit(1);
        }
        
        for(j = 6; j < i-1; j++)
		{
			host[j - 6] = bufferHost[j];
		}    
        host[j-6] = '\0';
        
        if(strchrIndex(':',host)>=0)
		{
            return substring(host,0,strchrIndex(':',host));
        }
        return host;
    }
}


char* getRequestURL(char* buffer)
{
    char tmpBuffer[1024*16]={0};
    strcpy(tmpBuffer,buffer);

    str_replace(tmpBuffer,"GET ","");
    str_replace(tmpBuffer,"POST ","");
    str_replace(tmpBuffer,"HEAD ","");
    str_replace(tmpBuffer,"CONNECT ","");

    int indexEndURL = strchrIndex(' ',tmpBuffer);
    return substring(tmpBuffer,0,indexEndURL);
}

void *proxy( void *arg)
{
    int clientSocket = *((int *) arg);
    char *hostName = malloc(sizeof(char)*256);
    char *urlRequest = malloc(sizeof(char)*256);
    int port;
    
    char buffer[1024*16];// on majore 16Ko pour des requetes HTTP

    int size = recv(clientSocket, buffer, 1024*10, 0);
    
    //On affiche la requete
    
    if(size>0)
	{
        
        str_replace(buffer,"Proxy-Connection: keep-alive","Connection : close");
        str_replace(buffer,"Connection: keep-alive","Connection : close");
        
        printf("%s**********************************************\nREQUEST RECEIVED FROM THE BROWSER\n**********************************************\n\n%s",JAUNE,NORMAL);
        printf("----------------------------------------------\r\nsize = %lu\n%s----------------------------------------------\r\n",strlen(buffer), buffer);
        
        
        if(strstr(buffer,"https://")!=NULL)
		{
            port=443;
        }
		else
		{
            port=80;
        }
        
        hostName = get_host(buffer);
        printf("HOST : [%s]\r\n", hostName);
        
        urlRequest=getRequestURL(buffer);
        if(!isItPubRequest(urlRequest))
		{
            // Consideration sur le host dans cette condition -> C'est ici qu'on vérifie si c'est une pub
            // Si le host n'est pas dans la easylist
            if(passRequest(clientSocket,buffer,hostName,port)<0)
			{
                printf("%s**********************************************\nREQUEST TRANSMISSION ERROR\n**********************************************\n\n%s",ROUGE,NORMAL);
            }
        }
		else 
		{
            printf("%s**********************************************\nAD BLOCKED : %s \n**********************************************\n\n%s",VERT,urlRequest,NORMAL);
            //C'est une pub.
            send(clientSocket, HTTP_PUB_BLOQUE, strlen(HTTP_PUB_BLOQUE), 0);
        }
        
    }
    
    close(clientSocket);
    countThread--;
    
    if(hostName)
	{
        printf("FREED HOSTNAME\n");
        free(hostName);
    }
    if(urlRequest)
	{
        printf("FREED URL REQUEST\n");
        free(urlRequest);
    };
    
    printf("THREADS STILL RUNNING = %d\n",countThread);
    if(MAX_NUMBER_OF_THREADS!=-1) pthread_exit(NULL);
    return 0;
}

int main(int argc, char** argv)
{
	int serverSocket;/*socket d'écoute et de dialogue*/
	int clilen;
	struct sockaddr_in serv_addr,cli_addr;

	if (argc!=2)
	{
		usage();
		exit(2);
	}

    printf("SERVER SOCKET WILL BE INITALIZED\n");

	if((serverSocket=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("SOCKET PROBLEM \n");
		exit(3);
	}
    printf("SERVER SOCKET INITALIZED\n");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	int portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
    
	if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
        printf("BINDING ERROR\n");
		exit(4);
	}
  
    printf("SERVER SOCKET BOUND\n");

	//listen et cfg du nb de connexion pending
	if(listen(serverSocket,SOMAXCONN) <0)
	{
		printf("LISTENING ERROR\n");
		exit(5);
	}
    printf("%sLISTENING SERVER SOCKET\n%s",VERT,NORMAL);

    clilen = sizeof(cli_addr);
	int clientSocket;
    
	//Accept
	while(1)
	{
		//la structure cli_addr permettra de recuperer les donnees du client (adresse ip et port)
		clientSocket = accept(serverSocket,(struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        
		if(clientSocket <0)
		{
			printf("ACCEPT ERROR \n");
            break;
		}
        
        if(MAX_NUMBER_OF_THREADS!=-1)
		{
            pthread_t thread1;
            if(pthread_create(&thread1, NULL, proxy,&clientSocket) == -1) 
			{
                printf("pthread_create");
                break;
            }
			else
			{
                countThread++;
                printf("Thread #%d created.\n",countThread);
            }
            
            if(countThread>MAX_NUMBER_OF_THREADS)
			{
                pthread_join(thread1, NULL);
            }
        }
		else
		{
            proxy(&clientSocket);
        }
    }
	close(serverSocket);
	return 0;
}


void affichageIPV4(void * ipV4)
{
    char ipWebServer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,ipV4, ipWebServer, INET_ADDRSTRLEN);
    printf("WEB SERVER (IPV4) : %s\n", ipWebServer); // prints "192.0.2.33"

}

void affichageIPV6(void * ipV6)
{
    char ipWebServer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, ipV6, ipWebServer, INET_ADDRSTRLEN);
    printf("WEB SERVER (IPV6) : %s\n", ipWebServer); // prints "192.0.2.33"

}

int passRequest(int clientSocket,char* fromClient, char* hostName, int port){
    
    struct hostent *structHost;
    struct sockaddr_in webServer;
    char buffer[1024*4]={0};
    int socketEnvoi;
    int size;
    
    if((structHost = gethostbyname(hostName)) == NULL)
	{
        printf("%s\n\n**********************************************\nHOST ERROR : %s\n**********************************************\n\n%s",ROUGE,hostName,NORMAL);
        return -1;

    }
    socketEnvoi = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketEnvoi == -1)
	{
        printf("%s\n\n**********************************************\nSOCKET ERROR\n**********************************************\n\n%s",ROUGE,NORMAL);
        close(socketEnvoi);
        return -1;

    }
	else
	{
        // on renseigne la bonne adresse ip obtenue a partir du packet qu'il nous a été envoyé sur le clientSocket, par le navigateur
        webServer.sin_family = AF_INET;
        webServer.sin_port   = htons(port); // on communique avec le serveur, sur le port 80 c'est le port pour l'http.
        webServer.sin_addr   = *((struct in_addr *)structHost->h_addr);

        memset(&(webServer.sin_zero),'\0',8); // on remplit de 0
        
        //AFFICHAGE DE L'ADRESSE IP DU SERVEUR
        if(structHost->h_addrtype==AF_INET)
		{
            affichageIPV4(&(webServer.sin_addr));
        }
		else
		{
            affichageIPV6(&(webServer.sin_addr));
        }
        
        //CONNEXION AU SERVEUR WEB
        if(connect(socketEnvoi,(struct sockaddr * )&webServer,sizeof(struct sockaddr)) == -1){
            printf("CONNECT ERROR\n" );
            return -1;
        }
        

        send(socketEnvoi, fromClient, strlen(fromClient), 0);
        
        memset(buffer,'\0',sizeof(buffer)); // on remplit de 0
        
        // On recoit les données du vrai serveur web
        
        printf("%s**********************************************\nANSWER SENT TO THE BROWSER\n**********************************************\n\n%s",JAUNE,NORMAL);
        

        while( (size = recv(socketEnvoi,buffer,sizeof(buffer)-1,0)) > 0)
		{
            
            if(size>0)
			{
                
                buffer[size] = '\0';
                str_replace(buffer,"Proxy-Connection: keep-alive","Connection : close");
                str_replace(buffer,"Connection: keep-alive","Connection : close");

                printf("%s",buffer); // on affiche la reponse partielle du vrai serveur web
                
                // pour les retourner sur le navigateur
                if(send(clientSocket, buffer,size,0)<0)
				{
                    printf("%s\n\n**********************************************\nERROR DURING THE TRANSMISSION TO THE BROWSER\n**********************************************\n\n%s",ROUGE,NORMAL);
                    close(socketEnvoi);
                    return -1;
                }
                memset(buffer,'\0',sizeof(buffer));
            }
			else
			{
                printf("%s\n\n**********************************************\nERROR DURING THE RESPONSE\n**********************************************\n\n%s",ROUGE,NORMAL);
                close(socketEnvoi);
                return -1;
            }
            
        }
        close(socketEnvoi);
        printf("\n\n############\n SENT TO THE BROWSER \n###########\n\n");
    }

    shutdown(clientSocket, SHUT_RDWR);
    return 0;
}




