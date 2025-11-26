/*librerie  di sistema */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "protocol.h"


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
int main()
{
setbuf(stdout, NULL);
srand(time(NULL));
#if  defined(_WIN32) || defined(_WIN64)
if(!winstartup())
{
return -1;
}
#endif

int wsocks;
wsocks=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
if(wsocks<0)
{
puts("errore di creazione della socket!");
return -1;
}

int opt = 1;
    if (setsockopt(wsocks, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        puts("setsockopt failed");
        return -1;
    }




struct sockaddr_in s_adr;
s_adr.sin_family= AF_INET; /*Su protocollo IPV4 */
s_adr.sin_port=htons(PORT);
s_adr.sin_addr.s_addr=inet_addr("127.0.0.1"); /*local host ip */

if(bind(wsocks,(struct sockaddr *) &s_adr,sizeof(s_adr))<0)
{
puts("errore di binding");
return -1;
}



if(listen(wsocks,QUEUE_SIZE)<0)
{
puts("connessione fallita");
return -1;
}


/*creazione della socket di connessione */
int conn_socks;
struct sockaddr_in cl_addr;
int bufferclient=sizeof(cl_addr); /*buffer dei dati host */
while(1)  /* inizio ascolto da parte del server */
{

conn_socks=	accept(wsocks,(struct sockaddr *) &cl_addr, &bufferclient);
if(conn_socks <0)
{
puts("connessione rifiutata");
continue;
}

/*host connesso */
printf("\t\a \033[34m%s  %s \033[0m\n",inet_ntoa(cl_addr.sin_addr),"connesso!");


/* attente la richiesta meteo  dal client connesso */

weather_request_t information;
weather_response_t wrsp;
memset(&wrsp,0,sizeof(wrsp));


if(recv(conn_socks, &information, sizeof(weather_request_t),0) <=0)
{
puts("errore di ricezione del messaggio");
closesocket(conn_socks);
continue;
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
info=get_temperature();
wrsp.value=info;
wrsp.type='t';
break;
}

case 'T':
{
info=get_temperature();
wrsp.value=info;
wrsp.type='t';
break;
}


/* calcolo dell'umidità (se richiesta) */
case 'h':
{
info=get_humidity();
wrsp.value=info;
wrsp.type='h';
break;
}

case 'H':
{
info=get_humidity();
wrsp.value=info;
wrsp.type='h';
break;
}

/* calcolo della velocità del vento (se richiesta)  */
case 'w':
{
info=get_wind();
wrsp.value=info;
wrsp.type='w';
break;
}

case 'W':
{
info=get_wind();
wrsp.value=info;
wrsp.type='w';
break;
}

/* calcolo della pressione (se richiesta)  */
case 'p':
{
info= get_pressure();
wrsp.value=info;
wrsp.type='p';
break;
}

case 'P':
{
info= get_pressure();
wrsp.value=info;
wrsp.type='p';
break;
}
} //fine switch
}



send(conn_socks,&wrsp,sizeof(weather_response_t),0);
closesocket(conn_socks);
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
