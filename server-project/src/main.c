
//main del server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "protocol.h"
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define closesocket close
#endif

static const char *SUPPORTED_CITIES[] = {
    "Bari", "Roma", "Milano", "Napoli", "Torino",
    "Palermo", "Genova", "Bologna", "Firenze", "Venezia"
};

void errorhandler(char *errorMessage) { printf("%s\n", errorMessage); }
void clearwinsock() {
#if defined(_WIN32) || defined(_WIN64)
    WSACleanup();
#endif
}

float random_float(float min, float max) {
    float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

void valida(weather_request_t *req, weather_response_t *resp) {
    if(req->type != TYPE_TEMPERATURE && req->type != TYPE_HUMIDITY &&
       req->type != TYPE_WIND && req->type != TYPE_PRESSURE) {
        resp->status = STATUS_INVALID_REQUEST;
        return;
    }
    int flag = 1;
    for (int i = 0; i < 10; i++) {
        if(strcasecmp(req->city, SUPPORTED_CITIES[i]) == 0) {
            flag = 0;
            break;
        }
    }
    resp->status = (flag == 1) ? STATUS_CITY_UNAVAILABLE : STATUS_SUCCESS;
}

float get_temperature(void) { return random_float(-10.0, 40.0); }
float get_humidity(void)    { return random_float(20.0, 100.0); }
float get_wind(void)        { return random_float(0.0, 100.0); }
float get_pressure(void)    { return random_float(950.0, 1050.0); }

int main(int argc, char *argv[]) {
#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
        printf("Errore in WSAStartup()\n");
        return 0;
    }
#endif

    int port = SERVER_PORT;
    if (argc > 2 && strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
    }

    int my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (my_socket < 0) {
        errorhandler("Creazione socket fallita.\n");
        clearwinsock();
        return -1;
    }

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = htonl(INADDR_ANY);
    sad.sin_port = htons(port);

    if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        errorhandler("Bind fallito.\n");
        closesocket(my_socket);
        clearwinsock();
        return -1;
    }

    if (listen(my_socket, QUEUE_SIZE) < 0) {
        errorhandler("Listen fallito.\n");
        closesocket(my_socket);
        clearwinsock();
        return -1;
    }

    struct sockaddr_in cad;
    int client_socket;
    int client_len;

    srand(time(NULL));
    printf("Server in ascolto sulla porta %d...\n", port);

    while (1) {
        client_len = sizeof(cad);
        if ((client_socket = accept(my_socket, (struct sockaddr*) &cad, (socklen_t*)&client_len)) < 0) {
            errorhandler("Accept fallito.\n");
            continue;
        }

        weather_request_t request;
        if (recv(client_socket, (char*)&request, sizeof(request), 0) <= 0) {
            closesocket(client_socket);
            continue;
        }

        printf("Richiesta '%c %s' dal client ip %s\n", request.type, request.city, inet_ntoa(cad.sin_addr));

        weather_response_t response;
        valida(&request, &response);

        if(response.status == STATUS_SUCCESS) {
            switch (request.type) {
                case TYPE_TEMPERATURE: response.value = get_temperature(); break;
                case TYPE_HUMIDITY:    response.value = get_humidity(); break;
                case TYPE_WIND:        response.value = get_wind(); break;
                case TYPE_PRESSURE:    response.value = get_pressure(); break;
            }
            response.type = request.type;
        } else {
            response.type = '\0';
            response.value = 0.0;
        }

        send(client_socket, (char*)&response, sizeof(response), 0);
        closesocket(client_socket);
    }

    closesocket(my_socket);
clearwinsock();
return 0;
}
