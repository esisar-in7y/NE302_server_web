#ifndef STRUCTURES_H
#define STRUCTURES_H
#include "global.h"
#include "mime.h"

typedef enum 
{
    GET, //GET
    POST, //POST
    HEAD //HEAD
} _Methode;

typedef enum 
{
    HTTP1_0,
    HTTP1_1
} _HTTP_version;

typedef enum 
{
    KEEP_ALIVE,
    CLOSE
} _Connection;

typedef struct {
    bool isPresent;
    bool initialized;
    bool BR;
    bool GZIP;
    bool DEFLATE;
    bool COMPRESS;
    bool CHUNKED;
} _Encoding;

typedef struct {
    int start;
    int end;
} _Range;

typedef struct {
    char* name;
    int value;
} _Server_timing;

//chained list or _Server_timing
typedef struct _Server_timings {
    _Server_timing* timings;
    struct _Server_timings* next;
} _Server_timings;

typedef struct {
    _HTTP_version* version;
    _Connection* connection;
    int* content_length;
    _Methode* methode;
    _Encoding transfert_encoding;
    _Encoding accept_encoding;
    char* host;

    _Range* ranges;
    unsigned int range_count;
} _headers_request;

typedef struct {
    _HTTP_version* version;
    _Connection* connection;
    int* content_length;
    int* status_code;
    char* content_type;
    _Encoding transfert_encoding;

    _Range* ranges;
    unsigned int range_count;

    _Server_timings* server_timings;
} _headers_response;


typedef struct {
    _headers_response headers_response;
    unsigned int clientId;// pour la socket
    char* body;
} _Reponse;


#endif