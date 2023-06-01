#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../utils/tree.h"
#include "fastcgi.h" 

// =========================================================================================================== // 

size_t readSocket(int fd,char *buf,size_t len)
{
size_t readlen=0;
ssize_t nb=0;  

	if (len==0) return 0; 

	do {
	// try to read 	
		do {
			nb = read(fd, buf+readlen, len-readlen);	
		} while (nb == -1 && errno == EINTR);
		if (nb > 0 ) readlen+=nb; 
	} while ((nb > 0) && (len != readlen )); 

	if (nb < 0 ) readlen=-1; 
	return readlen;  
}
//============================================================================================================ // 

void readData(int fd,FCGI_Header *h,size_t *len) 
{
	size_t nb; 
	*len=0; 

	nb=sizeof(FCGI_Header)-FASTCGILENGTH; 
	if ((readSocket(fd,(char *)h,nb) == nb)) {
		h->requestId=htons(h->requestId); 
		h->contentLength=htons(h->contentLength);
		*len+=nb;
		nb=h->contentLength+h->paddingLength; 
		if ((readSocket(fd,(char *)h->contentData,nb) == nb)) {
			*len+=nb; 
		} else {
			*len=0; 
		}
	}
}

// =========================================================================================================== // 
void writeSocket(int fd,FCGI_Header *h,unsigned int len)
{
	int w;

	h->contentLength=htons(h->contentLength); 
	h->paddingLength=htons(h->paddingLength); 
 

	while (len) {
	// try to write 	
		do {
			w = write(fd, h, len);
		} while (w == -1 && errno == EINTR);
	len-=w; 
	}
} 

// =========================================================================================================== // 
void writeLen(int len, char **p) {
	if (len > 0x7F ) { 
		*((*p)++)=(len>>24)&0x7F; 
		*((*p)++)=(len>>16)&0xFF; 
		*((*p)++)=(len>>8)&0xFF; 
		*((*p)++)=(len)&0xFF; 
	} else *((*p)++)=(len)&0x7F;
}
	
// =========================================================================================================== // 
int addNameValuePair(FCGI_Header *h,char *name,char *value)
{
	char *p; 
	unsigned int nameLen=0,valueLen=0;

	if (name) nameLen=strlen(name); 
	if (value) valueLen=strlen(value);

	if ((valueLen > FASTCGIMAXNVPAIR) || (valueLen > FASTCGIMAXNVPAIR) ) return -1; 
	if ((h->contentLength+((nameLen>0x7F)?4:1)+((valueLen>0x7F)?4:1)) > FASTCGILENGTH ) return -1; 
	
	p=(h->contentData)+h->contentLength; 
	writeLen(nameLen,&p); 
	writeLen(valueLen,&p);
	strncpy(p,name,nameLen); 
	p+=nameLen; 
	if (value) strncpy(p,value,valueLen); 
	h->contentLength+=nameLen+((nameLen>0x7F)?4:1);
	h->contentLength+=valueLen+((valueLen>0x7F)?4:1);
}	 
// =========================================================================================================== // 		
	
void sendGetValue(int fd) 
{
FCGI_Header h; 

	h.version=FCGI_VERSION_1; 
	h.type=FCGI_GET_VALUES; 
	h.requestId=htons(FCGI_NULL_REQUEST_ID); 
	h.contentLength=0; 
	h.paddingLength=0; 
	addNameValuePair(&h,FCGI_MAX_CONNS,NULL); 
	addNameValuePair(&h,FCGI_MAX_REQS,NULL); 
	addNameValuePair(&h,FCGI_MPXS_CONNS,NULL); 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}

// =========================================================================================================== // 
void sendBeginRequest(int fd,unsigned short requestId,unsigned short role,unsigned char flags) 
{
FCGI_Header h; 
FCGI_BeginRequestBody *begin; 

	h.version=FCGI_VERSION_1; 
	h.type=FCGI_BEGIN_REQUEST; 
	h.requestId=htons(requestId); 
	h.contentLength=sizeof(FCGI_BeginRequestBody); 
	h.paddingLength=0; 
	begin=(FCGI_BeginRequestBody *)&(h.contentData); 
	begin->role=htons(role); 
	begin->flags=flags; 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}
// =========================================================================================================== // 
void sendAbortRequest(int fd,unsigned short requestId) 
{
FCGI_Header h; 

	h.version=FCGI_VERSION_1; 
	h.type=htons(FCGI_ABORT_REQUEST); 
	h.requestId=requestId; 
	h.contentLength=0; 
	h.paddingLength=0; 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
}
#define sendStdin(fd,id,stdin,len) sendWebData(fd,FCGI_STDIN,id,stdin,len)
#define sendData(fd,id,data,len) sendWebData(fd,FCGI_DATA,id,data,len)

//============================================================================================================ // 

void sendWebData(int fd,unsigned char type,unsigned short requestId,char *data,unsigned int len) 
{
FCGI_Header h; 

	if (len > FASTCGILENGTH) return ; 
	
	h.version=FCGI_VERSION_1; 
	h.type=type; 
	h.requestId=htons(requestId); 
	h.contentLength=len; 
	h.paddingLength=0;
	memcpy(h.contentData,data,len); 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));  
}

// =========================================================================================================== // 
static int createSocket(int port)
{
	int fd;
	struct sockaddr_in serv_addr;
	int enable = 1;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation failed\n");
		return (-1);
	}

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(port);

	if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed\n");
		return (-1);
	}

	return fd;
}
// =========================================================================================================== // 


void send_direct_header_cgi(tree_node* root,FCGI_Header* h,char* header,char* query){
	char* tmp;
	tmp=get_first_value(root, query);
	if(tmp!=NULL){
		addNameValuePair(h,header,tmp);
		better_free(tmp);
	}
}
void send_indirect_header_cgi(tree_node* root,FCGI_Header* h,char* header,char* query){
	char* tmp;
	tmp=getFieldValueFromFieldName(root, query);
	if(tmp!=NULL){
		addNameValuePair(h,header,tmp);
		better_free(tmp);
	}
}

void fill_headers(tree_node* root,FCGI_Header* h){
//   'REQUEST_METHOD' => "method" direct,
//   'REQUEST_URI' => request_target direct,
//    'QUERY_STRING' => query direct,
//   'CONTENT_LENGTH' => Content_Length direct,
//   'SCRIPT_FILENAME' => absolute_path direct,
//   'DOCUMENT_URI' => 'absolute_path direct',
//   'SERVER_PROTOCOL' => HTTP_version direct,
//   'HTTP_HOST' => uri_host direct,
//   'HTTP_ACCEPT' => Accept indirect,
//   'HTTP_CONNECTION' => connection_option direct
	send_direct_header_cgi(root,h,"REQUEST_METHOD","method");
	send_direct_header_cgi(root,h,"REQUEST_URI","request_target");
	send_direct_header_cgi(root,h,"QUERY_STRING","query");
	send_direct_header_cgi(root,h,"CONTENT_LENGTH","Content_Length");
	send_direct_header_cgi(root,h,"SCRIPT_FILENAME","absolute_path");
	send_direct_header_cgi(root,h,"DOCUMENT_URI","absolute_path");
	send_direct_header_cgi(root,h,"SERVER_PROTOCOL","HTTP_version");
	send_direct_header_cgi(root,h,"HTTP_HOST","uri_host");
	send_direct_header_cgi(root,h,"HTTP_ACCEPT","Accept");
	send_direct_header_cgi(root,h,"HTTP_CONNECTION","connection_option");

//   'CONTENT_TYPE' => 'Content-Type:' indirect,
//   'HTTP_ACCEPT_LANGUAGE' => "Accept-Language" indirect,
//   'HTTP_ACCEPT_ENCODING' => Accept-Encoding indirect,
//   'HTTP_USER_AGENT' =>  User-Agent indirect,
	send_indirect_header_cgi(root,h,"CONTENT_TYPE","Content-Type:");
	send_indirect_header_cgi(root,h,"HTTP_ACCEPT_LANGUAGE","Accept-Language");
	send_indirect_header_cgi(root,h,"HTTP_ACCEPT_ENCODING","Accept-Encoding");
	send_indirect_header_cgi(root,h,"HTTP_USER_AGENT","User-Agent");
//   'SERVER_SOFTWARE' => 'sup4rserv300',
//   'SERVER_NAME' => 'sup4rserv300',
	addNameValuePair(h,"SERVER_SOFTWARE","sup4rserv300");
	addNameValuePair(h,"SERVER_NAME","sup4rserv300");
}

void sendFCGI(tree_node* root)
{
    int fd;
    size_t len;
    FCGI_Header h;

    // Establish a connection to the FastCGI server
    fd = createSocket(9000);
	int ID=1;
    // Send a FCGI_BEGIN_REQUEST message to start a new request
    sendBeginRequest(fd,ID,FCGI_RESPONDER,FCGI_KEEP_CONN);

    // Send a FCGI_PARAMS message with required environment variables
    h.version = FCGI_VERSION_1;
    h.type = FCGI_PARAMS;
    h.requestId = htons(ID);
    h.contentLength = 0;
    h.paddingLength = 0;
	fill_headers(root,&h);
    
    writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));

    // Send an empty FCGI_PARAMS message
    h.contentLength = 0;
    h.paddingLength = 0;
    writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));

//    BODY=> "message_body" direct
	char* length_string=get_first_value(root, "Content_Length");
	int length=0;
	sscanf(length_string,"%d",&length);
	char* data=get_first_value(root,"message_body");
    sendWebData(fd,FCGI_STDIN,ID,data,length);
    sendWebData(fd,FCGI_STDIN,ID,NULL,0);    
    
    // Read the response from the server
    do {
        readData(fd,&h,&len);
        if (h.type == FCGI_STDOUT) {
            if (len > 0) {
            printf("buf=%s",h.contentData);

            }
        }
    } while ((len != 0 ) && (h.type != FCGI_END_REQUEST));

}

