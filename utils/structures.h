#include "global.h"
#ifndef STRUCTURES_H
#define STRUCTURES_H
typedef enum 
{
    GET, //GET
    POST, //POST
    HEAD //HEAD
} Methode;

typedef enum 
{
    HTTP1_0,
    HTTP1_1
} HTTP_version;

typedef enum 
{
    KEEP_ALIVE,
    CLOSE
} Connection;

typedef struct {
    int start;
    int end;
} Range;

typedef enum 
{
    CHUNKED,
    COMPRE,
    deflate,
    gzip
} Transfer_encoding;


typedef struct {
    HTTP_version version;
    Range* ranges;
    Connection connection;
    int content_length;
    Methode methode;
} headers_request;

typedef struct {
    HTTP_version version;
    Connection connection;
    int content_length;
    int status_code;
    //TODO
    Transfer_encoding transfEncod;
    Range* ranges;
} headers_response;

#endif