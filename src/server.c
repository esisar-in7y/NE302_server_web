#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../lib/request.h"
#include "../utils/global.h"
#include "../utils/manip.h"
#include "../utils/mime.h"
#include "../utils/req.c"
#include "api.h"
#include "semantic.h"
#include "answer.h"

#define PORT		8000
#define BUFFER_SIZE 1024
void send_end(int clientId) {
	writeDirectClient(clientId, "\r\n\r\n", 4);
}


// TODO connection keep alive
int main2(int argc, char* argv[]){
	message* requete;
	printf("Serveur HTTP demarre sur le port %d\n", PORT);

	while (1) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest.
		if ((requete = getRequest(PORT)) == NULL) return -1;
		// Affichage de debug
		printf("#########################################\nDemande recue depuis le client %d\n", requete->clientId);
		printf("Client [%d] [%s:%d]\n", requete->clientId, inet_ntoa(requete->clientAddress->sin_addr), htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande %.*s\n\n", requete->len, requete->buf);
		int status = 0;
		if (parseur(requete->buf, requete->len) == 0) {
			writeDirectClient(requete->clientId, "HTTP/1.0 ", 9);
			send_status(400, requete->clientId);
			endWriteDirectClient(requete->clientId);
			requestShutdownSocket(requete->clientId);
		} else {
			printf("get root\n");
			tree_node* root = (tree_node*)getRootTree();
			tree_node_print_all(root,0);
			printf("popu resp\n");
			if(checkVersion(root)==1){
				writeDirectClient(requete->clientId, "HTTP/1.1 ", 9);
			}else{
				writeDirectClient(requete->clientId, "HTTP/1.0 ", 9);
			}
			status = getstatus(root);
			if (status > 0) {
				send_status(status, requete->clientId);
				send_end(requete->clientId);
				// Gérer le header connection pour savoir si on garder la connexion ouverte ou non
				endWriteDirectClient(requete->clientId);
				requestShutdownSocket(requete->clientId);
			} else {
				// send_status(200, requete->clientId);
				answerback(root, status, requete->clientId);
				// Fermer la connexion avec le client
				// endWriteDirectClient(requete->clientId);
				if (!keepAlive(root)) {
#ifdef DEBUG
					debug_http("Not keep alive", __LINE__);
#endif
					requestShutdownSocket(requete->clientId);
				} else {
#ifdef DEBUG
					debug_http("Keep alive", __LINE__);
#endif
				}
			}
		}

		// on ne se sert plus de requete a partir de maintenant, on peut donc liberer...
		freeRequest(requete);
	}
	return (1);
}

#define false 0
#if HTTP==1
int main(int argc, char* argv[]) {
	return main2(argc, argv);
}
#endif
// echo -ne 'GET /sites/www.fake.com/index.html HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0\r\n\r\n' | ncat -C --hex-dump out localhost 8101
// echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n' | ncat -C --hex-dump out localhost 8000
// curl -Iv http://localhost:8000/www.toto.com/index.html 2>&1 | grep -i 'connection #0'
// echo -ne 'GET /www.toto.com/index.html HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\nConnection: keep-alive\r\n\r\n'

// curl -v http://localhost:8000/www.toto.com/index.html 2>&1 | grep -i 'connection #0'
// curl -v http://localhost:8000/www.toto.com/index.html  --next http://localhost:8000/www.toto.com/index.html 2>&1 | grep -i '#0'