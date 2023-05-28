#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../lib/request.h"
#define PORT	 8000
#define REPONSE1 "HTTP/1.1 200 OK\r\n"
#define REPONSE2 "\r\n"

int main(int argc, char* argv[]) {
	message* requete;

	while (1) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest.
		if ((requete = getRequest(PORT)) == NULL) return -1;

		// Affichage de debug
		printf("#########################################\nDemande recue depuis le client %d\n", requete->clientId);
		printf("Client [%d] [%s:%d]\n", requete->clientId, inet_ntoa(requete->clientAddress->sin_addr), htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande %.*s\n\n", requete->len, requete->buf);

		writeDirectClient(requete->clientId, REPONSE1, strlen(REPONSE1));
		writeDirectClient(requete->clientId, "Connection: keep-alive\r\n", 24);
		writeDirectClient(requete->clientId, "Content-Length: 0\r\n", 19);
		writeDirectClient(requete->clientId, REPONSE2, strlen(REPONSE2));
		endWriteDirectClient(requete->clientId);
		// requestShutdownSocket(requete->clientId);
		// on ne se sert plus de requete a partir de maintenant, on peut donc liberer...
		freeRequest(requete);
	}
	return (1);
}

// echo -ne 'GET /sites/www.fake.com/index.html HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0\r\n\r\n' | ncat -C --hex-dump out localhost 8101
// echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n' | ncat -C --hex-dump out localhost 8000
// curl -Iv http://localhost:8000/www.toto.com/index.html 2>&1 | grep -i 'connection #0'
// echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\nConnection: keep-alive\r\n\r\n' | ncat -C --hex-dump out localhost 8000

// curl -v http://localhost:8000/www.toto.com/index.html 2>&1 | grep -i 'connection #0'
// curl -v http://localhost:8000/www.toto.com/index.html  --next http://localhost:8000/www.toto.com/index.html 2>&1 | grep -i '#0'

// echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7:\r\nAccept-Language: en-US,en;q=0.9:\r\nCache-Control: no-cache:\r\nConnection: keep-alive:\r\nPragma: no-cache:\r\nSec-Fetch-Dest: document:\r\nSec-Fetch-Mode: navigate:\r\nSec-Fetch-Site: none:\r\nSec-Fetch-User: ?1:\r\nUpgrade-Insecure-Requests: 1:\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36:\r\nsec-ch-ua: "Chromium";v="110", "Not A(Brand";v="24", "Google Chrome";v="110":\r\nsec-ch-ua-mobile: ?0:\r\nsec-ch-ua-platform: "Linux"\r\n\r\n| ncat -C --hex-dump out localhost 8000
