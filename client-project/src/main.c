/* client main */

/*librerie  di sistema  principali*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"

/*funzioni e librerire dedicate interamente a windows */

#if defined(_WIN32) || defined(_WIN64)
#include <winsock.h>

/* funzione windows che de-alloca le socket  */
void clearwinsock() {
    WSACleanup();
}

/*set-up della winsock */
int winstartup() {
    WSADATA windata;
    int boot = WSAStartup(MAKEWORD(2,2), &windata);
    if (boot != 0) {
        puts("Errore in WSAStartup()");
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

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);

#if defined(_WIN32) || defined(_WIN64)
    if (!winstartup())
{
 return -1;
}
#endif

    int port = SERVER_PORT;
    char ip[32] = DEFAULT_IP;   // IP di default
    weather_request_t request;
    memset(&request, 0, sizeof(request));

    int promt;
    while ((promt = getopt(argc, argv, "r:p:c:t:s:")) != -1) {
        switch (promt) {
            case 'r':
                sscanf(optarg, " %c %63[^\n]", &request.type, request.city);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                request.type = optarg[0];
                break;
            case 'c':
                strncpy(request.city, optarg, sizeof(request.city) - 1);
                break;
            case 's':   // nuovo: IP server personalizzato
                strncpy(ip, optarg, sizeof(ip) - 1);
                ip[sizeof(ip) - 1] = '\0';
                break;
        }
    }

    if (request.type == 0 || strlen(request.city) == 0) {
        puts("Errore di richiesta!");
        return -1;
    }

    int connysocks = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connysocks < 0) {
        puts("Errore nella creazione della socket!");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(connysocks, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        puts("Errore di connessione!");
        closesocket(connysocks);
#if defined(_WIN32) || defined(_WIN64)
        clearwinsock();
#endif
        return -1;
    }

    if (send(connysocks, &request, sizeof(request), 0) != sizeof(request)) {
        puts("Errore nell'invio della richiesta!");
        closesocket(connysocks);
#if defined(_WIN32) || defined(_WIN64)
        clearwinsock();
#endif
        return -1;
    }

    weather_response_t response;
    if (recv(wsock, &response, sizeof(response), 0) <= 0) {
        puts("Errore nella ricezione!");
        closesocket(connysocks);
#if defined(_WIN32) || defined(_WIN64)
        clearwinsock();
#endif
        return -1;
    }

    request.city[0] = toupper(request.city[0]);
    printf("Ricevuto risultato dal server ip %s. ", inet_ntoa(server_addr.sin_addr));

    switch (response.status) {
        case STATUS_SUCCESS:
            switch (response.type) {
                case TYPE_TEMPERATURE:
                    printf("%s: Temperatura = %.1f°C\n", request.city, response.value);
                    break;
                case TYPE_HUMIDITY:
                    printf("%s: Umidità = %.1f%%\n", request.city, response.value);
                    break;
                case TYPE_WIND:
                    printf("%s: Vento = %.1f km/h\n", request.city, response.value);
                    break;
                case TYPE_PRESSURE:
                    printf("%s: Pressione = %.1f hPa\n", request.city, response.value);
                    break;
            }
            break;
        case STATUS_CITY_UNAVAILABLE:
            printf("Città non disponibile\n");
            break;
        case STATUS_INVALID_REQUEST:
            printf("Richiesta non valida\n");
            break;
        default:
            printf("Errore sconosciuto\n");
    }

    closesocket(connysocks);
#if defined(_WIN32) || defined(_WIN64)
    clearwinsock();
#endif
    return 0;
}
