#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "../lib/request.h"
#include "../utils/fastcgi.h"
#include "../utils/global.h"
#include "../utils/manip.h"
#include "../utils/mime.h"
#include "answer.h"
#include "api.h"
#include "semantic.h"

#define PORT		8000
#define BUFFER_SIZE 1024
void send_end(int clientId) {
	writeDirectClient(clientId, "\r\n\r\n", 4);
}
#define false 0
#if HTTP == 1
int main() {
	message* requete;
	printf("Serveur HTTP demarre sur le port %d\n", PORT);
#if LEAK_CHECK == 1
	int e = 0;
	while (e < 10) {
		e++;
#else
	while (1) {
#endif
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest.
		if ((requete = getRequest(PORT)) == NULL) return -1;
		// Affichage de debug
		printf("#########################################\nDemande recue depuis le client %d\n", requete->clientId);
		printf("Client [%d] [%s:%d]\n", requete->clientId, inet_ntoa(requete->clientAddress->sin_addr), htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande %.*s\n\n", requete->len, requete->buf);
		bool parseur_status = parseur(requete->buf, requete->len);
		tree_node* root = (tree_node*)getRootTree();
		if (parseur_status == 0) {
			writeDirectClient(requete->clientId, "HTTP/1.0 ", 9);
			send_status(400, requete->clientId);
			endWriteDirectClient(requete->clientId);
			requestShutdownSocket(requete->clientId);
		} else {
#if DEBUG == 1
			tree_node_print_all(root, 0);
#endif
			// ICI DEVIATION REQUETE POUR FASTCGI
			char* abs_path = get_first_value(root, "absolute_path");
			if (strstr(abs_path, ".php") != NULL) {
				printf("TO CGI\n");
				int keepalive = sendFCGI(root, requete);
				endWriteDirectClient(requete->clientId);
				if (!keepalive) {
					requestShutdownSocket(requete->clientId);
				}
			} else {
				_headers_request headers_request = {0};
				_Response response = {0};
				response.clientId = requete->clientId;
				if ((response.headers_response.status_code = getstatus(root, &headers_request)) >= 300) {
					response.headers_response.version = HTTP1_0;
					response.headers_response.connection = CLOSE;
					send_headers(&response);
					send_end(response.clientId);
					endWriteDirectClient(response.clientId);
					requestShutdownSocket(response.clientId);
				} else {
					//  Connection / Content Length ect ...
					populateRespFromReq(&headers_request, &response);
					// Content Length, Content Type populate in send_data
					if (!send_data(root, &headers_request, &response)) {
						send_response(&response);
					}
					// Fermer la connexion avec le client
					endWriteDirectClient(response.clientId);
					if (response.headers_response.connection == CLOSE) {
						requestShutdownSocket(response.clientId);
					} else {
					}
					freeResponse(&response, &headers_request);
				}
			}
			free(abs_path);
		}

		purgeTree(root);
		// on ne se sert plus de requete a partir de maintenant, on peut donc liberer...
		freeRequest(requete);
		requete = NULL;
	}
	return (1);
}
#endif
