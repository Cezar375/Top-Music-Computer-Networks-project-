#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare
  char mesajin[1000],mesajout[1000];

  /* stabilim portul */
  port = 2024;

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr("10.0.2.15"); /// adresa ip
  /* portul de conectare */
  server.sin_port = htons (port); /// portul este 2024

  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
while(1)
  {
  /* citirea mesajului */
  printf ("\n\e[0;31m[client]Command: \e[m");
  fflush (stdout);


  fgets(mesajin,100,stdin);
  fflush (stdin);
  strtok(mesajin, "\n");/// pt a elimina /n pus la fgets



  /* trimiterea mesajului la server */
  write (sd,&mesajin,sizeof(mesajin));

  /* citirea raspunsului dat de server
     (apel blocant pina cind serverul raspunde) */

  if(strcmp(mesajin,"exit")==0) break; /// am iesit, si nu mai putem continua comunica cu sv


  read (sd, &mesajout,sizeof(mesajout));

  /* afisam mesajul primit */
  printf ("\n\e[0;36m[client]Received from sv: %s\e[m\n", mesajout);
  fflush (stdout);
  }
  /* inchidem conexiunea, am terminat */
  close (sd);
}

