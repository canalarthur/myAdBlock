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
#define MAXLINE 80

/*
Serveur TCP :

Socket => Bind => Listen => Accept => Exchange (reader/writer) => Close
*/

int readline (int fd, char* ptr, int maxlen);
void passRequest(int clientSocket,char* fromClient, char* hostName );

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




void *proxy( void *arg){
    printf("THREAD BEGGINED \n");
    int clientSocket = *((int *) arg);

    char buffer[1024*10]; // on lit la requete que veut faire le navigateur dans ce buffer
    // ca fait pas mal mais j'ai envie de lire tout d'un coup
    
    int socketEnvoi;
    int size = recv(clientSocket, buffer, sizeof(buffer), 0);
    
    //On affiche la requete
    printf("size : %d \r\n", size);
    //les 4 lignes qui suivent, sert a remplir de\0 ce qui nous interesse pas. Si j'enleve cette ligne j'ai des "??" qui apparaissent a la fin, c'est zarb
    if(size>0){
        str_replace(buffer,"Proxy-Connection: keep-alive","Connection : close");
        str_replace(buffer,"Connection: keep-alive","Connection : close");
        printf("Request Received By The BROWSER\r\n");
        printf("---------\r\n%s---------\r\n", buffer);
        
        char * host = get_host(buffer);
        printf("Host : [%s]\r\n", get_host(buffer));
        
        
        if(1==1){// Consideration sur le host dans cette condition -> C'est ici qu'on checkes si c'est une pub
            
            // le host n'est pas dans la easylist
            passRequest(clientSocket,buffer,host);
        }else {
            //C'est une pub... Reste a savoir quoi faire dans ces cas la.
        }
        

    }
    close(clientSocket);
    printf("Client socket closed\n");
    printf("THREAD ENDED \n");

    return 0;


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
        
        printf("Avant la création du thread.\n");
        
        if(pthread_create(&thread1, NULL, proxy,&clientSocket) == -1) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
        
        printf("Après la création du thread.\n");
    }
	close(serverSocket);

	return 0;

}

void passRequest(int clientSocket,char* fromClient, char* hostName ){
    
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
        webServer.sin_port   = htons(80); // on communique avec le serveur, sur le port 80 c'est le port pour l'http.
        webServer.sin_addr   = *((struct in_addr *)structHost->h_addr);
        
        memset(&(webServer.sin_zero),'\0',8); // on remplit de 0
        
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


/*
 * Lire  une ligne terminee par \n à partir d'un descripteur de socket
 */
int readline (int fd, char* ptr, int maxlen)
{
  
  int n, rc, retvalue, encore=1;
  char c, *tmpptr;

  tmpptr=ptr;
  for (n=1; (n < maxlen) && (encore) ; n++) {
    if ( (rc = read (fd, &c, 1)) ==1) {
      *tmpptr++ =c; 
      if (c == '\n')  /* fin de ligne atteinte */
      {
         encore =0;
         retvalue = n;
      }
    }
    else if (rc ==0) {  /* plus rien à lire */
      encore = 0;
      if (n==1) retvalue = 0;  /* rien a été lu */
      else retvalue = n;
    }
    else { /*rc <0 */
      if (errno != EINTR) {
	encore = 0;
	retvalue = -1;
      }
    }
  }
  *tmpptr = '\0';  /* pour terminer la ligne */
  return (retvalue);
}


