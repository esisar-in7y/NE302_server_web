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
    unsigned int clientId;// pour la socket

    _HTTP_version* version;
    _Connection* connection;
    int* content_length;
    int* status_code;
    char* content_type;
    _Encoding transfert_encoding;

    _Range* ranges;
    unsigned int range_count;
} _headers_response;

#endif