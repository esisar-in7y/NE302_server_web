#include "answer.h"
#define SHOWHEAD	0

void send_status(int status, int clientId) {
	char status_char[4];
	sprintf(status_char, "%d ", status);
	writeDirectClient(clientId, status_char, 3);
	// writeDirectClient(clientId," OK\r\n\r\n",SHOWHEAD?7:5);
	switch (status) {
	case 100: writeDirectClient(clientId, "Continue\r\n\r\n", SHOWHEAD ? 12 : 10); break;
	case 101: writeDirectClient(clientId, "Switching Protocols\r\n\r\n", SHOWHEAD ? 23 : 21); break;
	case 102: writeDirectClient(clientId, "Processing\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 103: writeDirectClient(clientId, "Early Hints\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 200: writeDirectClient(clientId, "OK\r\n\r\n", SHOWHEAD ? 7 : 5); break;
	case 201: writeDirectClient(clientId, "Created\r\n\r\n", SHOWHEAD ? 12 : 10); break;
	case 202: writeDirectClient(clientId, "Accepted\r\n\r\n", SHOWHEAD ? 13 : 11); break;
	case 203: writeDirectClient(clientId, "Non-Authoritative Information\r\n\r\n", SHOWHEAD ? 34 : 32); break;
	case 204: writeDirectClient(clientId, "No Content\r\n\r\n", SHOWHEAD ? 14 : 12); break;
	case 205: writeDirectClient(clientId, "Reset Content\r\n\r\n", SHOWHEAD ? 17 : 15); break;
	case 206: writeDirectClient(clientId, "Partial Content\r\n\r\n", SHOWHEAD ? 19 : 17); break;
	case 207: writeDirectClient(clientId, "Multi-Status\r\n\r\n", SHOWHEAD ? 16 : 14); break;
	case 208: writeDirectClient(clientId, "Already Reported\r\n\r\n", SHOWHEAD ? 21 : 19); break;
	case 226: writeDirectClient(clientId, "IM Used\r\n\r\n", SHOWHEAD ? 11 : 9); break;
	case 300: writeDirectClient(clientId, "Multiple Choices\r\n\r\n", SHOWHEAD ? 20 : 18); break;
	case 301: writeDirectClient(clientId, "Moved Permanently\r\n\r\n", SHOWHEAD ? 22 : 20); break;
	case 302: writeDirectClient(clientId, "Found\r\n\r\n", SHOWHEAD ? 9 : 7); break;
	case 303: writeDirectClient(clientId, "See Other\r\n\r\n", SHOWHEAD ? 13 : 11); break;
	case 304: writeDirectClient(clientId, "Not Modified\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 305: writeDirectClient(clientId, "Use Proxy\r\n\r\n", SHOWHEAD ? 12 : 10); break;
	case 306: writeDirectClient(clientId, "Switch Proxy\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 307: writeDirectClient(clientId, "Temporary Redirect\r\n\r\n", SHOWHEAD ? 22 : 20); break;
	case 308: writeDirectClient(clientId, "Permanent Redirect\r\n\r\n", SHOWHEAD ? 22 : 20); break;
	case 400: writeDirectClient(clientId, "Bad Request\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 401: writeDirectClient(clientId, "Unauthorized\r\n\r\n", SHOWHEAD ? 16 : 14); break;
	case 402: writeDirectClient(clientId, "Payment Required\r\n\r\n", SHOWHEAD ? 19 : 17); break;
	case 403: writeDirectClient(clientId, "Forbidden\r\n\r\n", SHOWHEAD ? 13 : 11); break;
	case 404: writeDirectClient(clientId, "Not Found\r\n\r\n", SHOWHEAD ? 13 : 11); break;
	case 405: writeDirectClient(clientId, "Method Not Allowed\r\n\r\n", SHOWHEAD ? 21 : 19); break;
	case 406: writeDirectClient(clientId, "Not Acceptable\r\n\r\n", SHOWHEAD ? 18 : 16); break;
	case 407: writeDirectClient(clientId, "Proxy Authentication Required\r\n\r\n", SHOWHEAD ? 33 : 31); break;
	case 408: writeDirectClient(clientId, "Request Timeout\r\n\r\n", SHOWHEAD ? 18 : 16); break;
	case 409: writeDirectClient(clientId, "Conflict\r\n\r\n", SHOWHEAD ? 12 : 10); break;
	case 410: writeDirectClient(clientId, "Gone\r\n\r\n", SHOWHEAD ? 8 : 6); break;
	case 411: writeDirectClient(clientId, "Length Required\r\n\r\n", SHOWHEAD ? 18 : 16); break;
	case 412: writeDirectClient(clientId, "Precondition Failed\r\n\r\n", SHOWHEAD ? 23 : 21); break;
	case 413: writeDirectClient(clientId, "Payload Too Large\r\n\r\n", SHOWHEAD ? 20 : 18); break;
	case 414: writeDirectClient(clientId, "URI Too Long\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 415: writeDirectClient(clientId, "Unsupported Media Type\r\n\r\n", SHOWHEAD ? 26 : 24); break;
	case 416: writeDirectClient(clientId, "Range Not Satisfiable\r\n\r\n", SHOWHEAD ? 24 : 22); break;
	case 417: writeDirectClient(clientId, "Expectation Failed\r\n\r\n", SHOWHEAD ? 22 : 20); break;
	case 418: writeDirectClient(clientId, "I'm a teapot\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 421: writeDirectClient(clientId, "Misdirected Request\r\n\r\n", SHOWHEAD ? 22 : 20); break;
	case 422: writeDirectClient(clientId, "Unprocessable Entity\r\n\r\n", SHOWHEAD ? 23 : 21); break;
	case 423: writeDirectClient(clientId, "Locked\r\n\r\n", SHOWHEAD ? 9 : 7); break;
	case 424: writeDirectClient(clientId, "Failed Dependency\r\n\r\n", SHOWHEAD ? 20 : 18); break;
	case 426: writeDirectClient(clientId, "Upgrade Required\r\n\r\n", SHOWHEAD ? 18 : 16); break;
	case 428: writeDirectClient(clientId, "Precondition Required\r\n\r\n", SHOWHEAD ? 23 : 21); break;
	case 429: writeDirectClient(clientId, "Too Many Requests\r\n\r\n", SHOWHEAD ? 21 : 19); break;
	case 431: writeDirectClient(clientId, "Request Header Fields Too Large\r\n\r\n", SHOWHEAD ? 34 : 32); break;
	case 451: writeDirectClient(clientId, "Unavailable For Legal Reasons\r\n\r\n", SHOWHEAD ? 32 : 30); break;
	case 500: writeDirectClient(clientId, "Internal Server Error\r\n\r\n", SHOWHEAD ? 24 : 22); break;
	case 501: writeDirectClient(clientId, "Not Implemented\r\n\r\n", SHOWHEAD ? 19 : 17); break;
	case 502: writeDirectClient(clientId, "Bad Gateway\r\n\r\n", SHOWHEAD ? 14 : 12); break;
	case 503: writeDirectClient(clientId, "Service Unavailable\r\n\r\n", SHOWHEAD ? 22 : 20); break;
	case 504: writeDirectClient(clientId, "Gateway Timeout\r\n\r\n", SHOWHEAD ? 18 : 16); break;
	case 505: writeDirectClient(clientId, "HTTP Version Not Supported\r\n\r\n", SHOWHEAD ? 29 : 27); break;
	case 506: writeDirectClient(clientId, "Variant Also Negotiates\r\n\r\n", SHOWHEAD ? 26 : 24); break;
	case 507: writeDirectClient(clientId, "Insufficient Storage\r\n\r\n", SHOWHEAD ? 23 : 21); break;
	case 508: writeDirectClient(clientId, "Loop Detected\r\n\r\n", SHOWHEAD ? 16 : 14); break;
	case 510: writeDirectClient(clientId, "Not Extended\r\n\r\n", SHOWHEAD ? 15 : 13); break;
	case 511: writeDirectClient(clientId, "Network Authentication Required\r\n\r\n", SHOWHEAD ? 33 : 31); break;
	}
	printf("answer back\n");
}