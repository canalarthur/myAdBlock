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

//NOS INCLUDES A NOUS
#include "finderPub.c"
#include "string-utils.c"

#include <netdb.h>

#define MAXLINE 80

int countThread=0;

int readline (int fd, char* ptr, int maxlen);
void passRequest(int clientSocket,char* fromClient, char* hostName, int port);

void usage(){
	printf("usage : servecho numero_port_serveur \n");
}

//COPIE COLLE
char * get_host(char * httpRequest){
    unsigned short i = 0, j = 0;
    char * buffer = strstr(httpRequest, "Host: " );
    char * host = (char *)malloc(256 * sizeof(char));
    while( buffer[i] != '\n' )
        i++;
    for(j = 6; j < i-1; j++)
        host[j - 6] = buffer[j];
    host[j-6+1] = '\0';
    return host;
}


char* getRequestURL(char* buffer){
    char* tmpBuffer=malloc(sizeof(buffer));
    strcpy(tmpBuffer,buffer);
    str_replace(tmpBuffer,"GET ","");
    str_replace(tmpBuffer,"POST ","");
    int indexEndURL=strchrIndex(' ',tmpBuffer);
    return substring(tmpBuffer,0,indexEndURL);

}

void *proxy( void *arg){
    printf("THREAD BEGUN (IRREGULAR VERB: BEGIN BEGAN BEGUN ?) \n");
    int clientSocket = *((int *) arg);
    char *hostName,*urlRequest=malloc(sizeof(char)*1000);
    int port;
    char buffer[1024*10]; // on lit la requete que veut faire le navigateur dans ce buffer
    // ca fait pas mal mais j'ai envie de lire tout d'un coup
    
    int socketEnvoi;
    int size = recv(clientSocket, buffer, sizeof(buffer), 0);
    
    //On affiche la requete
    printf("size : %d \r\n", size);
    
    if(size>0){
        str_replace(buffer,"Proxy-Connection: keep-alive","Connection : close");
        str_replace(buffer,"Connection: keep-alive","Connection : close");
        printf("Request Received By The BROWSER\r\n");
        printf("---------\r\n%s---------\r\n", buffer);
        
        
        if(strstr(buffer,"https://")!=NULL){
            port=443;
        }else{
            port=80;
        }
        
        hostName = get_host(buffer);
        str_replace(hostName,":443","");
        
        str_replace(hostName,".fr?",".fr");
        str_replace(hostName,".fr-",".fr");
        str_replace(hostName,".com-",".com");
        str_replace(hostName,".com?",".com");

        printf("Host : [%s]\r\n", hostName);
        
        urlRequest=getRequestURL(buffer);
        printf("URL REQUEST IS = %s\n\n",urlRequest);

        if(1==1){//isItPubRequest(urlRequest)){
            // Consideration sur le host dans cette condition -> C'est ici qu'on checkes si c'est une pub
            
            // le host n'est pas dans la easylist
            passRequest(clientSocket,buffer,hostName,port);
        }else {
            printf("ON A BALAYE UNE PUB : %s ",urlRequest);
            //C'est une pub... Reste a savoir quoi faire dans ces cas la.
            //si on fait rien c'est le navigateur qui va mouliner pour rien.
        }
        close(clientSocket);
        printf("Client socket closed\n");
        printf("THREAD ENDED \n");
        countThread--;
        return 0;
    }else{
        close(clientSocket);
        countThread--;
        return 0;
    }
   


}
int main(int argc, char** argv){
	int serverSocket;/*socket d'écoute et de dialogue*/
	int clilen;
    
	struct sockaddr_in serv_addr,cli_addr;
	char fromClient[MAXLINE];


	if (argc!=2){
		usage();
		exit(1);
	}
    
    
	/*
		Ouvrir une socket (a tcp socket)
	*/

    printf("SERVEUR SOCKET WILL BE INITALIZED\n");

	if((serverSocket=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("servecho : probleme socket \n");
		exit(2);
	}
    printf("SERVEUR SOCKET INITALIZED\n");


	/*
		On lie la socket a l'adresse
	*/
	bzero((char *) &serv_addr, sizeof(serv_addr));
	int portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
    
	if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
              perror("ERROR on binding");
		exit(1);
	}
  
    printf("SERVEUR SOCKET BOUND\n");

	//listen et cfg du nb de connexion pending
	if(listen(serverSocket,SOMAXCONN) <0){
		perror("servecho: erreur listen\n");
		exit(1);
	}
    printf("SERVEUR SOCKET LISTENING\n");

    clilen = sizeof(cli_addr);


	//Accept
	while(1){
        
		//la structure cli_addr permettra de recuperer les donnees du client (adresse ip et port)
		int clientSocket = accept(serverSocket,(struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        
		if(clientSocket <0){
			perror("servecho : erreur accept \n");
			exit(1);
		}
        pthread_t thread1;
        
        
        if(pthread_create(&thread1, NULL, proxy,&clientSocket) == -1) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }else{
            countThread++;
            printf("Création du thread No %d.\n",countThread);
        }

    }
	close(serverSocket);

	return 0;

}


void affichageIPV4(void * ipV4){
    char ipWebServer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,ipV4, ipWebServer, INET_ADDRSTRLEN);
    printf("IP DU SERVEUR WEB (IPV4) : %s\n", ipWebServer); // prints "192.0.2.33"

}

void affichageIPV6(void * ipV6){
    char ipWebServer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, ipV6, ipWebServer, INET_ADDRSTRLEN);
    printf("IP DU SERVEUR WEB (IPV6) : %s\n", ipWebServer); // prints "192.0.2.33"

}

void passRequest(int clientSocket,char* fromClient, char* hostName, int port){
    
    struct hostent *structHost;
    struct sockaddr_in webServer;
    char buffer[1024*50];
    int socketEnvoi;
    int size;
    
    if((structHost = gethostbyname(hostName)) == NULL){
        printf("ERROR Host\n");
        return;

    }
    socketEnvoi = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketEnvoi == -1){
        printf("ERROR socket\r\n" );
        return;

    }else{
        // on renseigne la bonne adresse ip obtenue a partir du packet qu'il nous a été envoyé sur le clientSocket, par le navigateur
        webServer.sin_family = AF_INET;
        webServer.sin_port   = htons(port); // on communique avec le serveur, sur le port 80 c'est le port pour l'http.
        webServer.sin_addr   = *((struct in_addr *)structHost->h_addr);

        memset(&(webServer.sin_zero),'\0',8); // on remplit de 0
        
        //AFFICHAGE DE L'ADRESSE IP DU SERVEUR
        if(structHost->h_addrtype==AF_INET){
            affichageIPV4(&(webServer.sin_addr));
        }else{
            affichageIPV6(&(webServer.sin_addr));
        }
        
        //CONNEXIN AU SERVEUR WEB
        if(connect(socketEnvoi,(struct sockaddr * )&webServer,sizeof(struct sockaddr)) == -1){
            printf("ERROR connect\r\n" );
            return;
        }
        
        printf("Request SENT TO THE WEB SERVER\r\n");

        send(socketEnvoi, fromClient, strlen(fromClient), 0);
        
        memset(buffer,'\0',sizeof(buffer)); // on remplit de 0
        
        // On recoit les données du vrai serveur web
        printf("REPONSE RECEIVED FROM THE WEB SERVER\r\n");

        while( (size = recv(socketEnvoi,buffer,sizeof(buffer),0)) != 0){
            
            if(size>0){
                
                buffer[size] = '\0';
                str_replace(buffer,"Proxy-Connection: keep-alive","Connection : close");
                str_replace(buffer,"Connection: keep-alive","Connection : close");

                printf("%s",buffer); // on affiche la reponse partielle du vrai serveur web
                
                // pour les retourner sur le navigateur
                if(send(clientSocket, buffer,size,0)<0){
                    printf("ERROR send to browser");
                    break;
                }
                memset(buffer,'\0',sizeof(buffer));
            }else{
                printf("ERROR de la reponse");
                break;
            }
            
        }
        close(socketEnvoi);
        printf("Envoyé au navigateur !\n");
    }
    
    
}




