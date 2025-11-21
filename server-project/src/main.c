/*librerie  di sistema */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "protocol.h"

/*funzione portabile di pulizia dello schermo*/
void clean()
{
#if  defined(_WIN32) || defined(_WIN64)
system("cls");
#else
system("clear");
#endif
}

/*funzione portabile di attesa*/
void sleeping (int value)
{
#if  defined(_WIN32) || defined(_WIN64)
sleep(value);
#else
usleep(value*1000);
#endif
}


/*funzione di caricamento dei messaggi */
void load_msg(const char *msg, int wait)
{
printf("\t  \033[32m%s ",  msg);
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
void err_msg(const char * msg)
{
printf("\a\t \033[1;4;31m%s\n", msg);
printf("\033[0m\n");
}



#if  defined(_WIN32) || defined(_WIN64)
#include <winsock.h>

/* funzione windows che de-alloca le socket  */
void clearwinsock() {
	WSACleanup();
puts("\a\t socket cleaned");
}

/*set-up della winsock */

int winstartup()
{
system("cls");
load_msg(" winsock is starting up ",2);
printf("\n");
WSADATA windata;
int boot=WSAStartup(MAKEWORD(2,2), &windata);
if(boot !=0)
{
puts("\a\t windows boot problem!");
puts("\tRestart the server");
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

/* inizio main */
int main(int argc, char *argv[]) {
srand(time(NULL));
#if  defined(_WIN32) || defined(_WIN64)
if(!winstartup())
{
return -1;
}
else
{
load_msg("windows socket boot completed", 1);
system("cls");
load_msg("done",1);
system("cls");
}
#endif

int wsocks;
load_msg("starting the server", 2);
wsocks=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
if(wsocks<0)
{
err_msg("errore di creazione della socket!");
return -1;
}

struct sockaddr_in s_adr;
s_adr.sin_family= AF_INET; /*Su protocollo IPV4 */
s_adr.sin_port=htons(PORT);
s_adr.sin_addr.s_addr=inet_addr("127.0.0.1"); /*local host ip */
printf("\n");
 clean();
load_msg("binding operation",3);
if(bind(wsocks,(struct sockaddr *) &s_adr,sizeof(s_adr))<0)
{
err_msg(" errore di binding!");
return -1;
}
clean();
load_msg("binding completato",2);
printf("\n");

if(listen(wsocks,QUEUE_SIZE)<0)
{
err_msg("connessione fallita");
return -1;
}
clean();
load_msg("attendendo una richiesta di connessione",2);
/*creazione della socket di connessione */
int conn_socks;
struct sockaddr_in cl_addr;
int bufferclient=sizeof(cl_addr); /*buffer dei dati host */
while(1)  /* inizio ascolto da parte del server */
{

conn_socks=	accept(wsocks,(struct sockaddr *) &cl_addr, &bufferclient);
if(conn_socks <0)
{
err_msg("connessione rifiutata");
load_msg(" ",4);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}

/*host connesso */
void clean();
printf("\t\a \033[34m%s  %s \033[0m\n",inet_ntoa(cl_addr.sin_addr),"connesso!");
const char *waiting= "benvenuto nel \"weather server\" inserisca la sua richiesta";

if(send(conn_socks,waiting,strlen(waiting),0) != strlen(waiting) ) /*invio messaggio di richiesta al client */
{
err_msg("errore nella richiesta!");
load_msg("chiusura della connessione",4);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}


/* attente la richiesta meteo  dal client connesso */



weather_request_t information;


if(recv(conn_socks, &information, sizeof(weather_request_t),0) <=0)
{
err_msg("non e' stato ricevuto nessun messaggio!");
load_msg("chiusura della connessione con l'host",4);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}

char t=information.type;
int state;
if(t=='t' || t=='h' || t=='w' ||t=='p')
{
state=0; //codice=successo
}
else
{
state=2;
}

char city[64];
strcpy(city,information.city);
int ok=0;  /*impostato come "non trovato" */


/*confronta tutti i possibili casi (evitando anche i problemi di case-sensitive( viene resituito 0 se la condizione è soddisfatta */

if(strcasecmp(city,"Bari") ==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Roma")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Milano")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Napoli")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Torino")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Palermo")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Genova")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Bologna")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Firenze")==0)
{
ok=1;
}
else
{
if(strcasecmp(city,"Venezia")==0)
{
ok=1;
}
}
}
}
}
}
}
}
}
}

float info;
weather_response_t wrsp;
wrsp.status=state;
wrsp.type=t;


if(ok !=1 && state !=2)
{
state=1;
}


if(state==0)
{
switch (t)
{

/* calcolo della temperatura (se richiesta)  */
case 't':
{
info=get_temperature();
wrsp.value=info;
load_msg(" acquisizione della temperatura generandone la risposta",2);
char rsp[BUFFER_SIZE];
memset(rsp,'\0',sizeof(rsp));
sprintf(rsp,"Ricevuto risultato dal server ip %s  %s: temperatura= %.1f°C",inet_ntoa(cl_addr.sin_addr),city,wrsp.value);
if(send(conn_socks, rsp, strlen(rsp),0) != strlen(rsp))
{
puts("\t\a problema di  overflow !");
printf("\n");
err_msg("la stringa e' troppo grande!");
load_msg("chiusura della connessione",4000);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}
break;
}
/* calcolo dell'umidità (se richiesta) */
case 'h':
{
info=get_humidity();
wrsp.value=info;
load_msg(" acquisizione dell'umidita' generandone la risposta",2);
char rsp[BUFFER_SIZE];
memset(rsp,'\0',sizeof(rsp));
sprintf(rsp,"Ricevuto risultato dal server ip %s  %s: percentuale di umidita'= %.0f",inet_ntoa(cl_addr.sin_addr),city,wrsp.value);
if(send(conn_socks, rsp, strlen(rsp),0) != strlen(rsp))
{
puts("\t\a problema di  overflow !");
printf("\n");
err_msg("la stringa e' troppo grande!");
load_msg("chiusura della connessione",4000);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}
break;
}

/* calcolo della velocità del vento (se richiesta)  */
case 'w':
{
info=get_wind();
wrsp.value=info;
load_msg(" acquisizione della velocita' del vento generandone la risposta",2);
char rsp[BUFFER_SIZE];
memset(rsp,'\0',sizeof(rsp));
sprintf(rsp,"Ricevuto risultato dal server ip %s  %s: velocita' del vento= %.1f km/h",inet_ntoa(cl_addr.sin_addr),city,wrsp.value);
if(send(conn_socks, rsp, strlen(rsp),0) != strlen(rsp))
{
puts("\t\a problema di  overflow !");
printf("\n");
err_msg("la stringa e' troppo grande!");
load_msg("chiusura della connessione",4000);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}
break;
}
/* calcolo della pressione (se richiesta)  */
case 'p':
{
 info= get_pressure();
 wrsp.value=info;
 load_msg(" acquisizione della pressione dell'aria generandone la risposta",2);
 char rsp[BUFFER_SIZE];
 memset(rsp,'\0',sizeof(rsp));
 sprintf(rsp,"Ricevuto risultato dal server ip %s  %s: pressione dell'aria= %.1f hPa",inet_ntoa(cl_addr.sin_addr),city,wrsp.value);
 if(send(conn_socks, rsp, strlen(rsp),0) != strlen(rsp))
 {
 puts("\t\a problema di  overflow !");
 printf("\n");
 err_msg("la stringa e' troppo grande!");
 load_msg("chiusura della connessione",4000);
 closesocket(conn_socks);
 #if  defined(_WIN32) || defined(_WIN64)
 clearwinsock();
 #endif
 }
break;
}
} //fine switch
}  //fine if
else
{
	switch (state)
	{
	case 1:  /* caso "città non disponibile" */
	{
	char rsp[BUFFER_SIZE];
	memset(rsp,'\0',sizeof(rsp));
	sprintf(rsp,"Ricevuto risultato dal server ip %s Citta' non disponibile ",inet_ntoa(cl_addr.sin_addr));
	if(send(conn_socks, rsp, strlen(rsp), 0) != strlen(rsp))
	{
	puts("\t\a problema di  overflow!!");
	printf("\n");
	err_msg("la stringa ha un numero di bytes diverso da quanto asspettato!");
	load_msg("chiusura della connessione",4);
	closesocket(conn_socks);
	#if  defined(_WIN32) || defined(_WIN64)
	clearwinsock();
	#endif
	}
	break;
	}

	case 2:     /* caso "informazione errata" */
	{
	char rsp[BUFFER_SIZE];
	memset(rsp,'\0',sizeof(rsp));
	sprintf(rsp,"Ricevuto risultato dal server ip %s Richiesta non valida ",inet_ntoa(cl_addr.sin_addr));
	if(send(conn_socks, rsp, strlen(rsp), 0) != strlen(rsp))
	{
	puts("\t\a problema di  overflow!!");
	printf("\n");
	err_msg("la stringa ha un numero di bytes diverso da quanto asspettato!");
	load_msg("chiusura della connessione",4);
	closesocket(conn_socks);
		#if  defined(_WIN32) || defined(_WIN64)
		clearwinsock();
	#endif
	}
	break;
	}
	}  /*fine switch */
	} /*fine else */
clean();
printf("\a\t terminando la connessione con  %s.\n",inet_ntoa(cl_addr.sin_addr));
printf("\n");
load_msg("chiusura della connessione",5);
printf("\t\a\033[1;44m%s\n","connessione chiusa");
}    /*fine ascolto del server */

closesocket(wsocks);

#if  defined(_WIN32) || defined(_WIN64)
	clearwinsock();
#endif
printf("\n");
printf("\t\a\033[1;44m%s\n","server chiuso");
	return 0;
} // main end



/*definizione delle funzioni */

/*calcolo del range */
float range (float min, float max)
{
float rr;
rr= min+(rand() /(float)RAND_MAX) * (max-min);
return rr;
}


/*funzione che calcola la temperatura  nel Range: -10.0 to 40.0 °C */
float get_temperature()
{
float temp;
temp= range(-10.0,40.0);
return temp;
}

/*funzione che calcola l'umidità  nel Range: 20.0 to 100.0 % */
float get_humidity()
{
float humy;
humy= range(0.20, 1.00);
humy= (humy*100);
return humy;
}

/*funzione che calcola la potenza del vento  nel Range: 0.0 to 100.0 km/h */

float get_wind()
{
float wind;
wind=range(0.0,100.0);
return wind;
}

/*funzione che calcola la pressione dell'aria nel Range: 950.0 to 1050.0 hPa */
float get_pressure(void)
{
float press;
press=range(950.0,1050.0);
return press;
}
