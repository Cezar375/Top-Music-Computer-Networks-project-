#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

/* portul folosit */
#define PORT 2024


/// In username avem numele clientului din fork curent
/// Numele melodiilor vor fi scrise cu litera mare la inceputul fiecarui cuvant si cu un singur spatiu intre cuvinte
/// Numele genului muzical va fi scris intre # la add
/// La comanda add dupa numele melodiei se va pune | urmat de >2 spatii
/// La comanda comment dupa numele melodiei se va pune |


///vote for nume melodie
///add numemelodie|    descriere    #genre#    link
///block vote username
///signup username (admin)
///login username
///show genre numelegenre-ului
///comment numemelodie|   comentariu
///delete numemelodie   : vor fi sterse si comentariile asociate ei

///exit

int nrclient;

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main ()
{
   int USER,USERbannedvote,TOP,COMENTARII;

   USER=open("usr",O_RDWR | O_APPEND);
   USERbannedvote=open("usrbanned",O_RDWR | O_APPEND);
   TOP=open("top",O_RDWR | O_APPEND);
   COMENTARII=open("comentarii",O_RDWR | O_APPEND);


  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;
  char mesajin[1000];          //mesajul primit de la client
  char mesajout[1000];        //mesaj de raspuns pentru client
  int sd;			        //descriptorul de socket

  pid_t pid; /// pt fork

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }


    int optiune=setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&(int){ 1 }, sizeof(int)); /// pune o optiune pe socket sa poata reutiliza adresa

  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  /* servim in mod iterativ clientii... */
  while (1)
    {
      int client;
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */

      client = accept (sd, (struct sockaddr *) &from, &length);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
	{
	  perror ("[server]Eroare la accept().\n");close(client); /// inchidem connection descriptorul
	  continue;
	}

      /* s-a realizat conexiunea, se astepta mesajul */

      nrclient++;/// nr de ordine a clientului

      pid=fork();

  if(pid==0) /// procesul copil
   {

     char token[10000],ch,tok[1000];
     char username[100];
     char songName[100];

     int logat=0,esteAdmin=0;
     int indice=0;

    while(1) /// citim mesaje si trimitem aceluiasi client
      {

      close(sd); ///inchidem listen descriptorul

      printf ("[server]Waiting for a command from a client\n");

      fflush (stdout);

      /* citirea mesajului */
      read (client,&mesajin,sizeof(mesajin));

      if(strcmp(mesajin,"exit")==0) break;
        else if(strncmp(mesajin,"signup",6)==0 && strstr(mesajin,"admin")&&logat==0)    ///signup admin
        {

            strcpy(token,"");
            indice=0;
            int nrSpatii=0;

            int gasitUSER=0;       /// daca gasitUSER e 1 la final numele este deja utilizat
            char *pointer;
            char numeUSER[100];
            int parcurgere22=open("usr",O_RDWR);


            pointer=strtok(mesajin+7," "); /// separam numele de "admin"
            strcpy(numeUSER,pointer);


            while(read(parcurgere22,&ch,1))
            {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                else
                {
                    token[indice]=NULL;indice=0;
                    if(strstr(token," ")) {pointer=strtok(token," ");strcpy(token,pointer);} /// numai daca am spatiu in token
                    if(strcmp(token,numeUSER)==0) gasitUSER=1;

                }
            }


            if(gasitUSER==0)
                  {
                   strcpy(token,mesajin+7);
                   strcat(token," admin");
                   write(USER,&token,strlen(token));
                   ch='\n';
                   write(USER,&ch,1);
                  }

            if(gasitUSER==0)/// trebuie verificat daca nu cumva exista cineva cu numele ala
            {
                strcpy(mesajout,"V-ati inregistrat ca admin.\n");
                write (client,&mesajout,sizeof(mesajout));
                fflush (stdout);
            }
            else
            {
              strcpy(mesajout,"Username-ul este deja folosit, va rugam sa alegeti altul.\n");
              write (client,&mesajout,sizeof(mesajout));
              fflush (stdout);

            }


        }
        else if(strncmp(mesajin,"signup",6)==0 &&logat==0)                            /// signup user
        {

            strcpy(token,"");
            indice=0;
            int gasitUSR=0;                          /// daca gasitUSR e 1 la final numele este deja utilizat
            int parcurgere21=open("usr",O_RDWR);
            char *pointer3;

            while(read(parcurgere21,&ch,1))
            {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                else
                {
                    token[indice]=NULL;indice=0;
                    if(strstr(token," ")) {pointer3=strtok(token," ");strcpy(token,pointer3);}
                    if(strcmp(token,mesajin+7)==0) gasitUSR=1;

                }
            }


            if(gasitUSR==0)
                  {
                   strcpy(token,mesajin+7);
                   write(USER,&token,strlen(token));
                   ch='\n';
                   write(USER,&ch,1);
                  }

            if(gasitUSR==0)/// trebuie verificat daca nu cumva exista cineva cu numele ala
            {
                strcpy(mesajout,"V-ati inregistrat ca user.\n");
                write (client,&mesajout,sizeof(mesajout));
                fflush (stdout);
            }
            else
            {
                strcpy(mesajout,"Username-ul este deja folosit, va rugam sa alegeti altul.\n");
                write (client,&mesajout,sizeof(mesajout));
                fflush (stdout);

            }
        }
        else if(strncmp(mesajin,"login",5)==0 && logat==0)           ///login-ul
        {
            strcpy(token,"");
            indice=0;

            int kadmin=0; /// daca linia extrasa contine admin sau nu, voi sti folosindu-ma de aceasta variabila

            char *pointer2,auxiliar[100000];
            int parcurgere1=open("usr",O_RDWR);

            while(read(parcurgere1,&ch,1))
            {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                else
                {   kadmin=0;
                    token[indice]=NULL;indice=0;

                    if(strstr(token," "))
                                          {
                                            strcpy(auxiliar,token);
                                            pointer2=strtok(token," ");

                                            kadmin=1; /// tokenul continea admin
                                          }

                               else pointer2=token;

                                                    /// mesajin+6 e doar username-ul pt ca scriem doar login ana de exemplu

                    if(strcmp(mesajin+6,pointer2)==0 && kadmin==1) {
                                                                         logat=1;
                                                                         esteAdmin=1;
                                                                         strcpy(username,mesajin+6);
                                                                         }
                     else if(strcmp(mesajin+6,pointer2)==0)
                                                    {
                                                     logat=1;
                                                     esteAdmin=0;
                                                     strcpy(username,mesajin+6);}
                }
            }
            token[indice]=NULL;

             if(logat==1 && esteAdmin==1) {strcpy(mesajout,"Logare reusita ca admin.\n");
                                           write (client,&mesajout,sizeof(mesajout));
                                           fflush (stdout);}

             else if(logat==1 && esteAdmin==0) {strcpy(mesajout,"Logare reusita ca user.\n");
                                                write (client,&mesajout,sizeof(mesajout));
                                                fflush (stdout);}

             else if(logat==0) {strcpy(mesajout,"Username incorect.\n");
                          write (client,&mesajout,sizeof(mesajout));
                          fflush (stdout);}
        }
         else if(logat==0) {
                            strcpy(mesajout,"Va rugam sa va logati pt. a putea scrie comenzi.\n");
                            write (client,&mesajout,sizeof(mesajout));
                            fflush (stdout);
                           }

         else if(strncmp(mesajin,"add",3)==0 &&logat==1)
         {
            strcpy(token,mesajin+4);
            strcpy(songName,mesajin+4);

            indice=0;

            for(indice=0;indice<strlen(songName);indice++)
                if(songName[indice]=='|') break;

            songName[indice]=NULL;

            int gasitMelod=0;
            char toke[1000];

            strcpy(toke,"");
            indice=0;

            int parcurgere23=open("top",O_RDWR);
            while(read(parcurgere23,&ch,1))
             {
                if(ch!='\n')
                {toke[indice]=ch;
                indice++;
                }
                 else
                {
                    toke[indice]=NULL;

                    for(indice=0;indice<strlen(toke)-1;indice++)
                    {
                        if(toke[indice]==' ' && toke[indice+1]==' ') break; /// mi-am separat numele melodiei de restul randului
                    }

                    toke[indice]=NULL;

                    indice=0;


                    if(strcmp(toke,songName)==0) {gasitMelod=1;break;} /// putem face break, am gasit-o deja in top.
                }
             }

            strcpy(toke,"");

            if(gasitMelod==0)   /// trebuie verificat daca nu cumva exista deja in top
                {
                   write(TOP,&songName,strlen(songName)); /// numele melodiei

                   strcpy(token,token+strlen(songName)+1); /// pt a scapa de numele melodiei si de | din token

                   write(TOP,&token,strlen(token)); /// o punem in top
                   ch='\n';
                   write(TOP,&ch,1);


                  strcpy(mesajout,"Melodia a fost adaugata topului.\n");
                  write (client,&mesajout,sizeof(mesajout));
                  fflush (stdout);
                }
               else if(gasitMelod==1)
               {
                   strcpy(mesajout,"Melodia exista deja in top.\n");
                   write (client,&mesajout,sizeof(mesajout));
                   fflush (stdout);
               }

         }
         else if(strstr(mesajin,"show genre")&&logat==1)
         {
             char genre[100];

             int gasitGenre=0;

             strcpy(genre,mesajin+11);

             strcpy(mesajout,"");
             strcpy(mesajout,"\n");

             strcpy(token,"");
             indice=0;
             char auxil[1000];
             char *pointer7,*pointer9;

             int parcurgere2=open("top",O_RDWR);

             while(read(parcurgere2,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;
                    indice=0;

                    strcpy(auxil,token);
                    pointer7=strtok(auxil,"#");
                    pointer7=strtok(NULL,"#");


                    ///adaugat joi



                    pointer9=strtok(pointer7,",");

                    //printf("%s",pointer9);
                    //printf("%s\n",genre);

                    while(pointer9!=NULL)
                    {

                        if(strcmp(pointer9,genre)==0) {gasitGenre=1;strcat(mesajout,token);strcat(mesajout,"\n");}

                        //printf("%s",pointer9);
                        //printf("%s\n",genre);

                        pointer9=strtok(NULL,",");
                    }



                  }
             }


             if(gasitGenre==1)  { /// am concatenat deja tot ce trebuia in mesajout
                                 write (client,&mesajout,sizeof(mesajout));
                                 fflush (stdout);
                                }
                         else   {strcpy(mesajout,"Nu exista melodii cu acest genre in top.\n");
                                 write (client,&mesajout,sizeof(mesajout));
                                 fflush (stdout);
                                }

         }
         else if(strncmp(mesajin,"comment",7)==0 &&logat==1)  /// clientul va preciza numele melodiei la care doreste sa comenteze, va pune | dupa numele melodiei

        {

            strcpy(songName,mesajin+8);

            int gasitMelodie=0;

            indice=0;

            for(indice=0;indice<strlen(songName);indice++)
                if(songName[indice]=='|') break;

            songName[indice]=NULL;

            indice=0;

            strcpy(tok,"");

             int parcurgere3=open("top",O_RDWR);

             while(read(parcurgere3,&ch,1))
             {
                if(ch!='\n')
                {tok[indice]=ch;
                indice++;
                }
                 else
                {

                    tok[indice]=NULL;

                    for(indice=0;indice<strlen(tok)-1;indice++)
                    {
                        if(tok[indice]==' ' && tok[indice+1]==' ') break; /// mi-am separat numele melodiei de restul randului
                    }

                    tok[indice]=NULL;

                    indice=0;




                    if(strcmp(tok,songName)==0) {write(COMENTARII,&username,strlen(username));

                                              strcpy(token," ,about the song:");
                                              write(COMENTARII,&token,strlen(token));

                                              write(COMENTARII,&songName,strlen(songName));

                                              ch=':';
                                              write(COMENTARII,&ch,1);

                                              strcpy(token,mesajin+9+strlen(songName));
                                              write(COMENTARII,&token,strlen(token));

                                              ch='\n';
                                              write(COMENTARII,&ch,1);

                                              gasitMelodie=1;

                                             }
                }
            }
        if(gasitMelodie==0) {strcpy(mesajout,"Melodia specificata nu exista in top.\n");
                             write (client,&mesajout,sizeof(mesajout));
                             fflush (stdout);}

                    else    {strcpy(mesajout,"Comentariul a fost postat mnelodiei aferente.\n");
                             write (client,&mesajout,sizeof(mesajout));
                             fflush (stdout);}
        }

       else if(strncmp(mesajin,"block vote",10)==0 &&logat==1&&esteAdmin==1)
       {
           char user[1000];
           int amBlocat=0;
           int kadmin2=0;
           char *pointer10,auxilia[1000];

           strcpy(user,mesajin+11);

           strcpy(token,"");

           int parcurgere4=open("usr",O_RDWR);

             while(read(parcurgere4,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {

                    kadmin2=0;
                    token[indice]=NULL;indice=0;

                    if(strstr(token," "))
                                          {
                                            strcpy(auxilia,token);
                                            pointer10=strtok(token," ");

                                            kadmin2=1; /// tokenul continea admin
                                          }

                               else pointer10=token;

                                                    /// mesajin+6 e doar username-ul pt ca scriem doar login ana de exemplu

                    //printf("%d",kadmin2);
                    //printf("%s",user);
                    //printf("%s\n",pointer10);

                    if(strcmp(pointer10,user)==0 && kadmin2==1)     {strcpy(mesajout,"Utilizatorului nu ii poate fi blocat dreptul la vot, fiind admin.\n");
                                                                   write (client,&mesajout,sizeof(mesajout));
                                                                   fflush (stdout);amBlocat=-1;}

                                            else if(strcmp(pointer10,user)==0)
                                                    {
                                                     write(USERbannedvote,&user,strlen(user));
                                                     ch='\n';
                                                     write(USERbannedvote,&ch,1);

                                                     strcpy(mesajout,"Utilizatorului i-a fost blocat dreptul la vot.\n");
                                                     write (client,&mesajout,sizeof(mesajout));
                                                     fflush (stdout);

                                                     amBlocat=1;
                                                    }
                }
             }

            if(amBlocat==0) /// userul nu exista in tabelul cu useri
            {
                strcpy(mesajout,"Utilizatorul specificat nu exista.\n");
                write (client,&mesajout,sizeof(mesajout));
                fflush (stdout);
            }

       }
       else if(strncmp(mesajin,"vote for",8)==0 &&logat==1)
       {
           struct flock lacat_blocat,lacat_deblocat;


           strcpy(songName,mesajin+9);

           int gasitMel=0;
           int gasitUSR=0;

           strcpy(token,"");
           indice=0;

           int parcurgere5=open("usrbanned",O_RDWR);

             while(read(parcurgere5,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;
                    indice=0;
                    if(strstr(token,username)) gasitUSR=1;

                }
             }

           strcpy(token,"");
           indice=0;
           int lungime=0;

           int parcurgere6=open("top",O_RDWR);

             while(read(parcurgere6,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;

                    lungime=strlen(token);

                    for(indice=0;indice<strlen(token)-1;indice++)
                    {
                        if(token[indice]==' ' && token[indice+1]==' ') break; /// mi-am separat numele melodiei de restul randului
                    }

                    token[indice]=NULL;

                    indice=0;

                    if(strcmp(token,songName)==0 && gasitUSR==0) /// daca gasim melodia si utilizatorul nu e blocat punem lacat

                                                   {gasitMel=1;
                                                   lacat_blocat.l_type=F_WRLCK;
                                                   lacat_blocat.l_whence=SEEK_CUR;
                                                   lacat_blocat.l_start=-lungime+1;
                                                   lacat_blocat.l_len=lungime;
                                                   fcntl(parcurgere6,F_SETLKW,&lacat_blocat); /// punem lock pe randul care av fi mdoificat+1 pt ca adaugam *
                                                   break;} /// putem face break, am gasit-o deja in top.
                }
             }

             strcpy(token,"");
             indice=0;
             int parcurgere7=open("top",O_RDWR);

             while(read(parcurgere7,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;

                    for(indice=0;indice<strlen(token)-1;indice++)
                    {
                        if(token[indice]==' ' && token[indice+1]==' ') break; /// mi-am separat numele melodiei de restul randului
                    }

                    token[indice]=NULL;

                    indice=0;



                    if(strcmp(token,songName)==0) {gasitMel=1;break;} /// putem face break, am gasit-o deja in top.
                }
             }

          if(gasitUSR==0&&gasitMel==1) /// avem exact finalul liniei corespunzatoare melodiei si utilizatorul nu e blocat
           {
               strcpy(token,"");
               indice=0;

               while(read(parcurgere6,&ch,1))
             {
                token[indice]=ch;
                indice++;
             }

             token[indice]=NULL;

             ch='*';
             lseek(parcurgere7,-1,SEEK_CUR);
             write(parcurgere7,&ch,1);
             ch='\n';
             write(parcurgere7,&ch,1);

             lacat_deblocat.l_type=F_UNLCK;
             lacat_deblocat.l_whence=SEEK_CUR;
             lacat_deblocat.l_start=-lungime-1;
             lacat_deblocat.l_len=lungime+2;

             fcntl(parcurgere7,F_SETLKW,&lacat_deblocat);

             write(parcurgere7,&token,strlen(token));

             strcpy(mesajout,"Melodia specificata a fost votata.\n");
             write (client,&mesajout,sizeof(mesajout));
             fflush (stdout);



           }
           else
             {
               strcpy(mesajout,"Melodia nu a fost gasita sau nu aveti drept de vot.\n");
               write (client,&mesajout,sizeof(mesajout));
               fflush (stdout);
             }


       }

       else if(strncmp(mesajin,"delete",6)==0 &&logat==1&&esteAdmin==1)
       {
           strcpy(songName,mesajin+7);

           strcpy(token,"");
           indice=0;
           char auxili[1000];


           int parcurgere8=open("top",O_RDWR);
           int amtrecut=0;
           char tokenInceput[1000];
           strcpy(tokenInceput,"");

             while(read(parcurgere8,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;
                    strcpy(auxili,token);

                    for(indice=0;indice<strlen(token)-1;indice++)
                    {
                        if(token[indice]==' ' && token[indice+1]==' ') break; /// mi-am separat numele melodiei de restul randului
                    }

                    token[indice]=NULL;

                    indice=0;

                    if(strcmp(token,songName)==0) {amtrecut=1;} /// am trecut de melodia cautata si am si gasit-o
                        else if(amtrecut==0) {strcat(tokenInceput,auxili);strcat(tokenInceput,"\n");}
                                else {strcat(tokenInceput,auxili);strcat(tokenInceput,"\n");}
                }
             }

           if(amtrecut==1)  /// am trecut imi spune daca am gasit melodia cu numele ala in top sau nu.
           {
            int parcurgere9=open("top",O_WRONLY | O_TRUNC);
            write(parcurgere9,&tokenInceput,strlen(tokenInceput));
            //ch='\n';
            //write(parcurgere9,&ch,1); /// ca sa pun endl dupa in caz ca vreau sa mai dau add la ceva dupa ce sterg

            strcpy(mesajout,"Melodia a fost gasita si stearsa.\n");
            write (client,&mesajout,sizeof(mesajout));
            fflush (stdout);
           }
           else
           {
              strcpy(mesajout,"Melodia nu exista in top.\n");
              write (client,&mesajout,sizeof(mesajout));
              fflush (stdout);
           }


           ///STERGEREA COMENTARIILOR ASOCIATE MELODIEI///


           strcpy(token,"");
           indice=0;

           int parcurgere10;
           int parcurgere11;

           amtrecut=0;
           strcpy(tokenInceput,"");

           int gasitMelo=1;
           char auxi[1000];
           char *pointer5;

          while(gasitMelo==1)
         {
             gasitMelo=0;

             parcurgere10=open("comentarii",O_RDWR);

             while(read(parcurgere10,&ch,1))
             {
                if(ch!='\n')
                {token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;
                    indice=0;

                    strcpy(auxi,token);
                    pointer5=strtok(auxi,":");
                    pointer5=strtok(NULL,":");

                    //printf("%s",pointer5);
                    //printf("%s\n",token);



                    if(strcmp(pointer5,songName)==0) {amtrecut=1;gasitMelo=1;}
                    else {strcat(tokenInceput,token);strcat(tokenInceput,"\n");}

                }
             }

           if(gasitMelo==0) break;

           parcurgere11=open("comentarii",O_RDWR | O_TRUNC);

           write(parcurgere11,&tokenInceput,strlen(tokenInceput));
           //ch='\n';
           //write(parcurgere11,&ch,1); /// ca sa pun endl dupa in caz ca vreau sa mai dau add la ceva dupa ce sterg

           lseek(parcurgere11,0,SEEK_SET);
           lseek(parcurgere10,0,SEEK_SET);

           close(parcurgere11);
           close(parcurgere10);
        }

       }
       else if(strncmp(mesajin,"rank vote",9)==0 &&logat==1)
       {
           char numeMelodie[100][100];
           int nrVoturi[100]={0};

           int k=0;

              strcpy(token,"");
              strcpy(mesajout,"\n");

             indice=0;
             int parcurgere30=open("top",O_RDWR);

             while(read(parcurgere30,&ch,1))
             {
                if(ch!='\n')
                {
                if(ch=='*') nrVoturi[k]++;

                token[indice]=ch;
                indice++;
                }
                 else
                {
                    token[indice]=NULL;

                    for(indice=0;indice<strlen(token)-1;indice++)
                    {
                        if(token[indice]==' ' && token[indice+1]==' ') break; /// mi-am separat numele melodiei de restul randului
                    }

                    token[indice]=NULL;

                    indice=0;


                    strcpy(numeMelodie[k],token);

                    k++; /// am mai gasit o melodie

                }
             }

             int i,j;
             char conversie[100]={0};
             char AUXmelodie[100];
             int AUXnrVoturi=0;

             for(i=0;i<k-1;i++)
                for(j=i+1;j<k;j++)
                  if(nrVoturi[j]>nrVoturi[i]) {strcpy(AUXmelodie,numeMelodie[i]);strcpy(numeMelodie[i],numeMelodie[j]);strcpy(numeMelodie[j],AUXmelodie);
                                            AUXnrVoturi=nrVoturi[i];nrVoturi[i]=nrVoturi[j];nrVoturi[j]=AUXnrVoturi;}


             for(i=0;i<k;i++)
             {
                 strcat(mesajout,numeMelodie[i]);
                 sprintf(conversie,"%d",nrVoturi[i]);
                 strcat(mesajout,"  ");
                 strcat(mesajout,conversie);
                 strcat(mesajout,"\n");

             }

             write (client,&mesajout,sizeof(mesajout));
             fflush (stdout);


       }
        else if((strncmp(mesajin,"block vote",10)==0 || strncmp(mesajin,"delete",6)==0 ) && esteAdmin==0)
        {
            strcpy(mesajout,"Permisiuni insuficiente.\n");
            write (client,&mesajout,sizeof(mesajout));
            fflush (stdout);

        }
        else if((strncmp(mesajin,"signup",6)==0 || strncmp(mesajin,"login",5)==0 )&&logat==1)
        {
            strcpy(mesajout,"Sunteti deja logat.\n");
            write (client,&mesajout,sizeof(mesajout));
            fflush (stdout);
        }

        else {

            strcpy(mesajout,"Comanda este incorecta.\n");
            write (client,&mesajout,sizeof(mesajout));
            fflush (stdout);

             }


      printf ("\e[0;34m[server]Command received from client NO %d: %s\e[m\n",nrclient,mesajin);

      }

      /* am terminat cu acest client, inchidem conexiunea */
      close (client);
      exit(0); /// daca nu pun exit-ul asta aici el ia din nou while-ul de la inceput si tot creeaza copii pt fiecare client
  }


    else     ///tatal
    {
       close(client);/// inchidem connection descriptorul

    }
    }
}
