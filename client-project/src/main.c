/* client main */

/*librerie  di sistema  principali*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"


/*pulizia "portabile" dello schermo */
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
int main(int argc, char *argv[]) {
clean();
#if  defined(_WIN32) || defined(_WIN64)
if(!winstartup())
{
return -1;
}
#endif

int connysocks;

weather_request_t request;
int promt;
while((promt=getopt(argc,argv,"r:"))!=-1)
{
if(promt=='r')
{
sscanf(optarg,"%c %[^\n]",&request.type,request.city);  /*accetta nella linea di commando ogni possibile stringa di città*/
}
}
connysocks=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
if(connysocks<0)
{
puts(" Errore di sistema!!");
return -1;
}


struct sockaddr_in s_adr;
s_adr.sin_family= AF_INET; /*Su protocollo IPV4 */
s_adr.sin_port=htons(PORT);
s_adr.sin_addr.s_addr=inet_addr("127.0.0.1"); /*local host */
printf("\n");
int connection=connect(connysocks,(struct sockaddr *)&s_adr,sizeof(s_adr));
if(connection <0)
{
clean();
closesocket(connysocks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}
clean();
printf("\n");
if(send(connysocks,&request,sizeof(request),0) !=sizeof(request))
{
clean();
closesocket(connysocks);
#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return -1;
}
weather_response_t response;

if(recv(connysocks,&response,sizeof(weather_response_t),0) <=0)
{
	closesocket(connysocks);
	#if  defined(_WIN32) || defined(_WIN64)
	clearwinsock();
	#endif
	return -1;
}
printf("ricevuto risultato dal server ip %s." ,inet_ntoa(s_adr.sin_addr) );
switch(response.status)
{
case 0 :
{
switch(response.type) //t,h,w,p
{
case 't':
{
printf("%s: temperatura=%.1f °C",request.city,response.value);
break;
}

case 'h':
{
printf("%s: umidita'=%.1f %%",request.city,response.value);
break;
}

case 'w':
{
printf("%s: velocita'  del vento=%.1f km/h",request.city,response.value);
break;
}

case 'p':
{
printf("%s: pressione=%.1f hPa",request.city,response.value);
break;
}

}
printf("\n");
break;
}

case 1:  //città non disponibile
{
printf("citta' non disponibile\n");
break;
}

case 2:
{
printf("richiesta non valida\n");
break;
}
}
printf("\n");

closesocket(connysocks);
clean();

#if  defined(_WIN32) || defined(_WIN64)
clearwinsock();
#endif
return 0;
} // main end
