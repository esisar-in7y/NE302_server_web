#ifndef STRUCTURES_H
#define STRUCTURES_H
#include "global.h"
#include "mime.h"

// enum avec = 1 pour que unitialized = 0
typedef enum {
	GET = 1, 
	POST,
	HEAD
} _Methode;

typedef enum { HTTP1_0 = 1, HTTP1_1 } _HTTP_version;

typedef enum { KEEP_ALIVE = 1, CLOSE } _Connection;

typedef struct {
	bool isPresent;
	bool initialized;
	bool BR;
	bool GZIP;
	bool DEFLATE;
	bool COMPRESS;
	bool CHUNKED;
	bool IDENTITY;
} _Encoding;

typedef enum { BR=1, GZIP, CHUNKED, COMPRESS, DEFLATE, IDENTITY } _Encoding_resp;

typedef struct {
	long int start;
	long int end;
	unsigned long int size;
} _Range;

// chained list of _Range
typedef struct _Ranges {
	_Range* range;
	struct _Ranges* next;
} _Ranges;

typedef struct {
	char* name;
	int value;
} _Server_timing;

// chained list of _Server_timing
typedef struct _Server_timings {
	_Server_timing* timings;
	struct _Server_timings* next;
} _Server_timings;

typedef struct {
	_HTTP_version version;
	_Connection connection;
	long int* content_length;
	_Methode methode;
	_Encoding transfert_encoding;
	_Encoding accept_encoding;
	char* host;

	_Ranges* ranges;
} _headers_request;

typedef struct {
	_HTTP_version version;
	_Connection connection;
	long int* content_length;
	int status_code;
	char* content_type;
	_Encoding_resp transfert_encoding;

	_Range* range; // on répond qu'on prems
	_Server_timings* server_timings;
} _headers_response;

typedef struct {
	unsigned int clientId;	// pour la socket
	_headers_response headers_response;
	char* body;
} _Response;

#endif
