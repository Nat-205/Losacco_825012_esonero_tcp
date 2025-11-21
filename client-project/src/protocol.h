/*header file dei protocoli del client */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/* parametri del client  (condivisi col server) */
#define BUFFER_SIZE 512    // Buffer size for messages
#define PORT 2024


/* tipo strutturale che contiene i dati*/
typedef struct {
   char type;        // Weather data type: 't', 'h', 'w', 'p'
   char city[64];    // City name (null-terminated string)
} weather_request_t;

/*prototipi di funzione */
void sleeping(int value); /* funzione di attesa portabile */
void load_msg(const char *msg, int wait); /*stampa un messaggio di caricamento "animato" */
void color_reset();  /*resetta la stampa del colore */
void err_msg(const char * msg,int *choice); /* stampa un messaggio di errore ed eventuale scelta dell'utente*/
void clean(); /*pulizia dello schermo */

/* prototipi di funzioni windows */
#if defined (_WIN32) || (_WIN64)

int Winstartup();
void clearwinsock();
#endif





#endif /* PROTOCOL_H_ */
