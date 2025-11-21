/* client main */

/*librerie  di sistema  principali*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"

/*funzione portabile di attesa*/
/*è basata su un valore di millisecondi */
void sleeping (int value)
{
#if  defined(_WIN32) || defined(_WIN64)
Sleep(value);
#else
usleep(value*1000);  /*conversione */
#endif
}


/*funzione di caricamento dei messaggi */
void load_msg(const char *msg, int wait)
{
printf("\t  \033[32m%5s ",  msg);
for(int i=0;i<3;i++)
{
printf(".");
sleeping(wait);
}
fflush(stdout);
printf("\033[0m\n");
printf("\n");
}

/*gestione degli errori */
void err_msg(const char * msg,int *choice)
{
printf("\a\t \033[1;4;31m%s\n", msg);
printf("\033[0m\n");
puts("\triprovare?");
do
{
printf("1)\033[1;32m%s  || 0)\033[1;31m%s\n","SI","NO");
printf("\033[0m\n");
scanf("%1d",choice);
}
while(*choice!=0 && *choice !=1);
}

/*reset del colore */
void color_reset()
{
printf("\033[0m\n");
}


void clean()
{
#if  defined(_WIN32) || defined(_WIN64)
system("cls");
#else
system("clear");
#endif
}



/*funzioni e librerire dedicate interamente a windows */
#if  defined(_WIN32) || defined(_WIN64)
#include <winsock.h>

/* funzione windows che de-alloca le socket  */
void clearwinsock() {
WSACleanup();
system("cls");
system("pause");
system("cls");
}

/*set-up della winsock */

int winstartup()
{
system("cls");
WSADATA windata;
int boot=WSAStartup(MAKEWORD(2,2), &windata);
if(boot !=0)
{
puts("\a\t windows boot problem!");
puts("\tRestart the server");
system("cls");
return 0;
}
return 1;
}

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

/* inizio main  client */
int main() {
clean();
#if  defined(_WIN32) || defined(_WIN64)
if(!winstartup())
{
return -1;
}
#endif

int connysocks;
int cc=0;
do
{
connysocks=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
if(connysocks<0)
{
err_msg(" Errore di sistema!!",&cc);
if(!cc)
{
return -1;
}
}
}
while(cc);


struct sockaddr_in s_adr;
s_adr.sin_family= AF_INET; /*Su protocollo IPV4 */
s_adr.sin_port=htons(PORT);
s_adr.sin_addr.s_addr=inet_addr("127.0.0.1"); /*local host */
printf("\n");
cc=0;
do
{
load_msg("tentando di stabilire una connessione",2500);
int connection=connect(connysocks,(struct sockaddr *)&s_adr,sizeof(s_adr));
if(connection <0)
{
err_msg("connessione non riuscita!",&cc);
clean();
if(!cc)
{
closesocket(connysocks);

#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}
}
}
while(cc);
clean();
puts("\aConnesso!");

char memobuff[BUFFER_SIZE]="\0";  /* setta l'array pulendolo*/

/*Riceve il messaggio di richiesta di inserimento dei dati */
int communication;
cc=0;
do
{
clean();
communication=recv(connysocks,memobuff, BUFFER_SIZE-1,0);
if(communication <=0)
{
err_msg("richiesta non ricevuta!",&cc);
clean();
if(!cc)
{
closesocket(connysocks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}
}
}
while(cc);
printf("\033[36m%s\n",memobuff); /*stampa il messaggio inviato dal server nel buffer */
color_reset();
memset(memobuff,0, BUFFER_SIZE); /*Ripulisce il buffer */

weather_request_t msg;
char req;

puts("\t ==|| cosa vorresti sapere? ||==");
printf("\n");
puts("-voglio conoscere la temperatura (inserisci 't')");
puts("-voglio conoscere l'umidita' presente nell'aria (inserisci 'h')");
puts("-voglio conoscere la velocita' del vento (inserisci 'w')");
puts("-voglio conoscere la pressione dell'aria (inserisci 'p')");
scanf(" %c",&req);
msg.type=req;
printf("\n");
char location[BUFFER_SIZE];
clean();
puts("\t ==||di quale localita' vorresti avere informazioni?||==");
printf("\n");
printf("-\033[31m%s\n\033[0m","BARI");
printf("-\033[33m%s\n\033[0m","ROMA");
printf("-\033[36m%s\n\033[0m","TORINO");
printf("-\033[37m%s\n\033[0m","MILANO");
printf("-\033[34m%s\n\033[0m","NAPOLI");
printf("-\033[35m%s\n\033[0m","PALERMO");
printf("-\033[36m%s\n\033[0m","GENOVA");
printf("-\033[35m%s\n\033[0m","BOLOGNA");
printf("-\033[31m%s\n\033[0m","FIRENZE");
printf("-\033[32m%s\n\033[0m","VENEZIA");
printf("\n");
scanf("%s",location);
strcpy(msg.city,location);
cc=0;
do
{
clean();
load_msg("invio della richiesta al server",2500);
if(send(connysocks,&msg,sizeof(msg),0) !=sizeof(msg))
{
err_msg("problema nella ricezione della richiesta!",&cc);
if(!cc)
{
	clean();
closesocket(connysocks);

#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}
}
}
while(cc);
load_msg("attesa della risposta  del server",3400);
cc=0;
do
{
clean();
communication=recv(connysocks,memobuff,BUFFER_SIZE-1,0);
if(communication <=0)
{
err_msg(" il server non ha risposto!",&cc);
if(!cc)
{
closesocket(connysocks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}
}
}
while(cc);
printf("\033[34m%s\n",memobuff);
color_reset();
getchar();
printf("\n");
load_msg("chiusura del collegamento",4300);
closesocket(connysocks);
clean();

#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
puts("connessione con il server chiusa,arrivederci");
return 0;
} // main end
