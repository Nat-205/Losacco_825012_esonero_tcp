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
int main() {
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
clean();
printf("\t\a \033[34m%s  %s \033[0m\n",inet_ntoa(cl_addr.sin_addr),"connesso!");


/* attente la richiesta meteo  dal client connesso */

weather_request_t information;
weather_response_t wrsp;

if(recv(conn_socks, &information, sizeof(weather_request_t),0) <=0)
{
err_msg("non e' stato ricevuto nessun messaggio!");
load_msg("chiusura della connessione con l'host",4);
closesocket(conn_socks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
}

if(information.type=='t' || information.type=='h' || information.type=='w' ||information.type=='p'||
information.type=='T' || information.type=='H' || information.type=='W' ||information.type=='P')  //case insensitive //
{
wrsp.status=0; //codice=successo
}
else
{
wrsp.status=2;
}

/*confronta tutti i possibili casi (evitando anche i problemi di case-sensitive( viene resituito 0 se la condizione è soddisfatta */
if(wrsp.status !=2)
{
char city[64];
int flag=0;
strcpy(city,information.city);

const char *cities[]={
"BARI","VENEZIA","FIRENZE","BOLOGNA","GENOVA","PALERMO","TORINO","NAPOLI","MILANO","ROMA",
"Bari","Venezia","Firenze","Bologna","Genova","Palermo","Torino","Napoli","Milano","Roma",
"bari","venezia","firenze","bologna","genova","palermo","torino","napoli","milano","roma"};

int n = sizeof(cities) / sizeof(cities[0]);

for(int i=0; i<n;i++)
{
if(strcmp(city,cities[i])==0)
flag++;
}
if(flag==0)
{
wrsp.status=1;
}
}


float info;
if(wrsp.status==0)
{
switch (information.type)
{

/* calcolo della temperatura (se richiesta)  */
case 't':
{
load_msg(" acquisizione della temperatura generandone la risposta",2);
info=get_temperature();
wrsp.value=info;
wrsp.type='t';
break;
}

case 'T':
{
load_msg(" acquisizione della temperatura generandone la risposta",2);
info=get_temperature();
wrsp.value=info;
wrsp.type='t';
break;
}


/* calcolo dell'umidità (se richiesta) */
case 'h':
{
load_msg(" acquisizione dell'umidita' generandone la risposta",2);
info=get_humidity();
wrsp.value=info;
wrsp.type='h';
break;
}

case 'H':
{
load_msg(" acquisizione dell'umidita' generandone la risposta",2);
info=get_humidity();
wrsp.value=info;
wrsp.type='h';
break;
}

/* calcolo della velocità del vento (se richiesta)  */
case 'w':
{
load_msg(" acquisizione della velocita' del vento generandone la risposta",2);
info=get_wind();
wrsp.value=info;
wrsp.type='w';
break;
}

case 'W':
{
load_msg(" acquisizione della velocita' del vento generandone la risposta",2);
info=get_wind();
wrsp.value=info;
wrsp.type='w';
break;
}

/* calcolo della pressione (se richiesta)  */
case 'p':
{
load_msg(" acquisizione della pressione dell'aria generandone la risposta",2);
info= get_pressure();
wrsp.value=info;
wrsp.type='p';
break;
}

case 'P':
{
load_msg(" acquisizione della pressione dell'aria generandone la risposta",2);
info= get_pressure();
wrsp.value=info;
wrsp.type='p';
break;
}


//default
default:
{
puts("richiesta non valida!");
wrsp.status=2;
break;
}
} //fine switch
}



if(send(conn_socks,&wrsp,sizeof(weather_response_t),0) != sizeof(weather_response_t))
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
