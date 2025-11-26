/* client main */

/*librerie  di sistema  principali*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"


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
int main(int argc, char *argv[]) {
setbuf(stdout, NULL);

#if  defined(_WIN32) || defined(_WIN64)
if(!winstartup())
{
return -1;
}
#endif

int connysocks;
int port=PORT;
weather_request_t request;
int promt;
memset(&request,0,sizeof(request));
while((promt=getopt(argc,argv,"r:p:c:t:"))!=-1)
{
switch(promt)
{
case 'r':
{
sscanf(optarg,"%c %63[^\n]",&request.type,request.city);  /*accetta nella linea di commando ogni possibile stringa di città*/
break;
}
case 'p': //se si vuole dichiarare una porta
{
port=atoi(optarg);
break;
}

//per richieste "universali"

case 't':
{
request.type=optarg[0];
break;
}

case 'c':
{
strncpy(request.city, optarg, sizeof(request.city) - 1);
break;
}
}
}

if(request.type==0 && strlen(request.city)<=0)
{
puts("errore di richiesta!");
return -1;
}


connysocks=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(connysocks<0)
{
puts(" Errore nella creazione della socket!");
return -1;
}


struct sockaddr_in s_adr;
s_adr.sin_family= AF_INET; /*Su protocollo IPV4 */
s_adr.sin_port=htons(port);
s_adr.sin_addr.s_addr=inet_addr("127.0.0.1"); /*local host */
printf("\n");

int connection=connect(connysocks,(struct sockaddr *)&s_adr,sizeof(s_adr));
if(connection <0)
{
puts("errore di connessione!");
closesocket(connysocks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}

printf("\n");
if(send(connysocks,&request,sizeof(request),0) !=sizeof(request))
{
closesocket(connysocks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}

weather_response_t response;

if(recv(connysocks,&response,sizeof(weather_response_t),0)>0)
{
	closesocket(connysocks);
	#if  defined(_WIN32) || defined(_WIN64)
	clearwinsock();
	#endif
	return -1;
}

printf("ricevuto risultato dal server ip %s.  " ,inet_ntoa(s_adr.sin_addr) );
switch(response.status)
{
case 0 :
{
switch(response.type) //t,h,w,p
{
case 't':
{
printf("%s: Temperatura = %.1f °C",request.city,response.value);
break;
}

case 'h':
{
printf("%s: Umidità = %.1f %%",request.city,response.value);
break;
}

case 'w':
{
printf("%s: Vento = %.1f km/h",request.city,response.value);
break;
}

case 'p':
{
printf("%s: Pressione = %.1f hPa",request.city,response.value);
break;
}

}
printf("\n");
break;
}

case 1:  //città non disponibile
{
printf("Città non disponibile\n");
break;
}

case 2:
{
printf("Richiesta non valida\n");
break;
}
}
fflush(stdout);

closesocket(connysocks);

#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return 0;
} // main end
