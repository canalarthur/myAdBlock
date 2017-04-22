#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAXLINE 80

/*
Serveur TCP :

Socket => Bind => Listen => Accept => Exchange (reader/writer) => Close
*/

int readline (int fd, char* ptr, int maxlen);

void usage(){
	printf("usage : servecho numero_port_serveur \n");
}


int main(int argc, char** argv){
	int serverSocket,clientSocket;/*socket d'écoute et de dialogue*/
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

	if((serverSocket=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("servecho : probleme socket \n");
		exit(2);
	}


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


	//listen et cfg du nb de connexion pending
	if(listen(serverSocket,SOMAXCONN) <0){
		perror("servecho: erreur listen\n");
		exit(1);
	}


	//Accept
	while(1){
		//la structure cli_add permettra de recuperer les donnees du client (adresse ip et port)
		clilen = sizeof(cli_addr);
		clientSocket = accept(serverSocket,(struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
		if(clientSocket <0){
			perror("servecho : erreur accept \n");
			exit(1);
		}
	

		//Envoi des données : discution 
		int r;
		while(1){
			r = readline(clientSocket,fromClient,MAXLINE);
			printf("%s",fromClient);
			if(r<=0 && (strcmp(fromClient,"\n")==0))break;
		}
		close(clientSocket);
		printf("Client socket closed");
	}
	close(serverSocket);

	return 0;

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
	{encore =0; retvalue = n;}
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


