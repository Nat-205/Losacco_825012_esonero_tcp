/*
 * protocol.h
 *
 * Server header file
 * Definitions, constants and function prototypes for the server
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_
/* parametri del server (condivisi col client) */
#define BUFFER_SIZE 512    // dimensione massima del buffer
#define QUEUE_SIZE 5       // lunghezza coda
#define PORT 2024

/*prototipi di funzione */
void sleeping(int value); /* funzione di attesa portabile */
void clean(); /* pulizia portatile dello schermo */
void load_msg(const char *msg, int wait); /*stampa un messaggio di caricamento "animato" */
void color_reset();  /*resetta la stampa del colore */
void err_msg(const char * msg);  /* stampa  messaggio di errore   */

/* prototipi di funzioni windows */
#if  defined(_WIN32) || defined(_WIN64)

int Winstartup();
void clearwinsock();
#endif


/*dichiarazioni tipo strutturale */

/*risposta del server */
typedef struct {
    unsigned int status;  // Response status code
    char type;            // Echo of request type
    float value;          // Weather data value
} weather_response_t;


/*memorizzazione dei dati inviati dal client */
typedef struct{
    char type;
    char city[64];
}weather_request_t;





/*funzioni di calcolo del valore richiesto dal client  */
float get_temperature();    // Range: -10.0 to 40.0 °C
float get_humidity();       // Range: 20.0 to 100.0 %
float get_wind();           // Range: 0.0 to 100.0 km/h
float get_pressure();       // Range: 950.0 to 1050.0 hPa









#endif /* PROTOCOL_H_ */
