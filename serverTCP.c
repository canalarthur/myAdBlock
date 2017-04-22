#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>

#define MAXLINE 80

/*
Serveur TCP :

Socket => Bind => Listen => Accept => Exchange (reader/writer) => Close
*/

void usage(){
	printf("usage : servecho numero_port_serveur \n");
}

int main(int argc, char** argv){
	int serverSocket,clientSocket;/*socket d'écoute et de dialogue*/
	int n,retread,clilen,childpid,servlen;

	struct sockaddr_in serv_addr,cli_addr;
	char fromClient[MAXLINE];
	char fromUser[MAXLINE];

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

	//la structure cli_add permettra de recuperer les donnees du client (adresse ip et port)
	clilen = sizeof(cli_addr);
	clientSocket = accept(serverSocket,(struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
	if(clientSocket <0){
		perror("servecho : erreur accept \n");
		exit(1);
	}
	

	//Envoi des données : discution 

	if((n=writen(clientSocket,"Bonjour\n",strlen("Bonjour\n")) ) != strlen("Bonjour\n")){
		perror("serveecho : erreur writen");
	}

	while((retread=readline(clientSocket,fromClient,MAXLINE))>0){
		printf("corr: %s",fromClient);
		if(strcmp(fromClient,"Au revoir\n") == 0)
			break; // fin de la lecture
		//Saisir le message de l'util
		printf("vous : ");
		if(fgets(fromUser,MAXLINE,stdin)==NULL){
			perror("error fgets\n");
			exit(1);
		}

		//Envoyer le message au client

		if((n=writen(clientSocket, fromUser,strlen(fromUser))) != strlen(fromUser)){
			printf("Erreur de writen");
			exit(1);
		}
	}
	if(retread<0){
		perror("erreur readline \n");
		//exit(1);
	}
	close(serverSocket);
	close(clientSocket);

}


int writen (fd, ptr, nbytes)
     int  fd;
     char *ptr;
     int nbytes;
{
  int nleft, nwritten; 
  char *tmpptr;

  nleft = nbytes;
  tmpptr=ptr;
  while (nleft >0) {
    nwritten = write (fd,ptr, nleft);
    if (nwritten <=0) {
      if(errno == EINTR)
	nwritten=0;
      else{
	perror("probleme  dans write\n");
	return(-1);
      }
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return (nbytes);
}


/*
 * Lire  "n" octets à partir d'un descripteur de socket
 */
int readn (fd, ptr, maxlen)
     int  fd;
     char *ptr;
     int maxlen;
{
  char *tmpptr;
  int nleft, nreadn;

  nleft = maxlen;
  tmpptr=ptr;
  
  while (nleft >0) {
    nreadn = read (fd,ptr, nleft);
    if (nreadn < 0) {
      if(errno == EINTR)
	nreadn=0;
      else{
	perror("readn : probleme  dans read \n");
	return(-1);
      }
    }
    else if(nreadn == 0){
      /* EOF */ 
      break ;
    }
    nleft -= nreadn;
    ptr += nreadn;
  }
  return (maxlen - nleft);
}

/*
 * Lire  une ligne terminee par \n à partir d'un descripteur de socket
 */
int readline (fd, ptr, maxlen)
     int  fd;
     char *ptr;
     int maxlen;
{
  
  int n, rc, retvalue, encore=1;  char c, *tmpptr; 

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

