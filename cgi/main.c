#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
/*
int main(int argc,char *argv[])
{
	int fd;
	size_t len;  

// TEST /// 

	FCGI_Header h; 
	fd=createSocket(9000); 
	sendBeginRequest(fd,10,FCGI_RESPONDER,FCGI_KEEP_CONN); 
	h.version=FCGI_VERSION_1; 
	h.type=FCGI_PARAMS; 
	h.requestId=htons(10); 
	h.contentLength=0; 
	h.paddingLength=0; 
	//addNameValuePair(&h,"SCRIPT_NAME","/info.php"); 
	addNameValuePair(&h,"REQUEST_METHOD","GET"); 
	addNameValuePair(&h,"SCRIPT_FILENAME","/var/www/html/info.php"); 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
	h.contentLength=0; 
	h.paddingLength=0; 
	writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength)); 
	do {	
		readData(fd,&h,&len);  	
		printf("read %d\n",len); 
		printf("version: %d\n",h.version); 
		printf("type: %d\n",h.type); 
		printf("requestId: %d\n",h.requestId); 
		printf("length: %d\n",h.contentLength);
		printf("pad: %d\n",h.paddingLength);
//		printf("data: %.*s\n",h.contentLength,h.contentData);	
	} while ((len != 0 ) && (h.type != FCGI_END_REQUEST)); 
}
*/
int main(int argc,char *argv[])
{
    int fd;
    size_t len;

    int content_length = -1;

    // Establish a connection to the FastCGI server
    fd = createSocket(9000);

    // Send a FCGI_BEGIN_REQUEST message to start a new request
    sendBeginRequest(fd,10,FCGI_RESPONDER,FCGI_KEEP_CONN);

    // Send a FCGI_PARAMS message with required environment variables
    FCGI_Header h;
    h.version = FCGI_VERSION_1;
    h.type = FCGI_PARAMS;
    h.requestId = htons(10);
    h.contentLength = 0;
    h.paddingLength = 0;
    addNameValuePair(&h,"REQUEST_METHOD","POST");
    addNameValuePair(&h,"SCRIPT_FILENAME","/var/www/html/post.php");
    addNameValuePair(&h,"QUERY_STRING","ip=192.168.0.1");
    writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));

    // Send an empty FCGI_PARAMS message
    h.contentLength = 0;
    h.paddingLength = 0;
    writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));


    char *data = "ip=192.168.0.1";
    sendWebData(fd,FCGI_STDIN,13,data,strlen(data)) ;

    // Read the response from the server
    do {
        readData(fd,&h,&len);
        if (h.type == FCGI_STDOUT) {
            if (content_length == -1) {
                content_length = ntohs(h.contentLength);
            }
            if (len > 0) {
            printf("buf=%s",h.contentData);

            }
        }
    } while ((len != 0 ) && (h.type != FCGI_END_REQUEST));

    return 0;
}

/*
int main(int argc,char *argv[])
{
    int fd;
    size_t len;
    char buf[1024];
    int content_length = -1;

    // Establish a connection to the FastCGI server
    fd = createSocket(9000);

    // Send a FCGI_BEGIN_REQUEST message to start a new request
    sendBeginRequest(fd,10,FCGI_RESPONDER,FCGI_KEEP_CONN);

    // Send a FCGI_PARAMS message with required environment variables
    FCGI_Header h;
    h.version = FCGI_VERSION_1;
    h.type = FCGI_PARAMS;
    h.requestId = htons(10);
    h.contentLength = 0;
    h.paddingLength = 0;
    addNameValuePair(&h,"REQUEST_METHOD","GET");
    addNameValuePair(&h,"SCRIPT_FILENAME","/var/www/html/info.php");
    writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));

    // Send an empty FCGI_PARAMS message
    h.contentLength = 0;
    h.paddingLength = 0;
    writeSocket(fd,&h,FCGI_HEADER_SIZE+(h.contentLength)+(h.paddingLength));

    // Read the response from the server
    do {
        readData(fd,&h,&len);
        if (h.type == FCGI_STDOUT) {
            printf("TAille=%d",len);
            if (content_length == -1) {
                content_length = ntohs(h.contentLength);
            }
            if (len > 0) {
                memcpy(buf, h.contentData, len);
                fwrite(buf, 1, len, stdout);
            }
        }
    } while ((len != 0 ) && (h.type != FCGI_END_REQUEST));

    // Extract the HTML content from the response
    // The content length is specified in the FCGI_STDOUT message header
    // and the content itself is in the message body
    if (content_length > 0) {
        fread(buf, 1, content_length, stdin);
        fwrite(buf, 1, content_length, stdout);
    }

    return 0;
}*/
