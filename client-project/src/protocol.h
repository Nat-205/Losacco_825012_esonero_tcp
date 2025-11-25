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

/*struttura che salva la risposta del server */
typedef struct {
    unsigned int status;  // Response status code
    char type;            // Echo of request type
    float value;          // Weather data value
} weather_response_t;



/* prototipi di funzioni windows */
#if defined (_WIN32) || (_WIN64)

int Winstartup();
void clearwinsock();
#endif





#endif /* PROTOCOL_H_ */
