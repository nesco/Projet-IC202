#include "common.h"
#include "chatroom.h"
#include <signal.h>
#include <pthread.h>

#define MAX_CONN 10            // nombre maximale de requêtes en attentes

int DFLAG;
int srv_sock;

void sig_handler(int s)
{
  switch (s) 
    {
    case SIGINT:
      stop_chat_room();
      sleep(1); /* wait for client to close */
      close(srv_sock);
      exit(0);
    case SIGPIPE: 
      break;
    default: break;
    }
}

/* Crée une socket d'écoute
 * srv_port: numéro de port d'écoute sous forme de chaîne de caractères
 * maxconn: nombre maximum de demandes de connexions en attente
 *
 * retourne la socket créée  */
int create_a_listening_socket(const char *const srv_port, const int maxconn){
  struct addrinfo hints;
  struct addrinfo *result = NULL, *rp = NULL;
  int ret_code = -1;
  int srv_sock = 0;
  
  memset(&hints, 0, sizeof(struct addrinfo));
  /* hint for IPv4 and IPv6*/
  /* hint stream socket */
  /* hint for wildcard IP address */

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  /* Récupération des paramètres de création de la socket */
  
  if (getaddrinfo(NULL, srv_port, &hints, &result) != 0)
    PERROR("Erreur de récupération des informations de connexions");
  
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    /* Tentative de création de la socket */
    srv_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (srv_sock == -1)
    {
      PERROR("Erreur lors du création de la socket");
      continue;
    }

    /* Tentative d'appel à bind */

    if (bind(srv_sock, rp->ai_addr, rp->ai_addrlen) != 0)
    {
      PERROR("Bind n'a pas réussi");
      continue;
    }
    
    if (listen(srv_sock, 5) == -1)
    {
      PERROR("Listen n'a pas réussi");
      continue;
    }
    
    break;
    
  }

  
  if (rp == NULL ) {               /* Aucune socket créée */
    fprintf(stderr, "Could not bind\n");
    return -1;
  }
  
  freeaddrinfo(result);           /* No longer needed */

  /* Configuration de la socket en écoute passive */
  
  return srv_sock;
}

/* Attend l'arrivée de demandes de connexions
 * srv_sock: la socket d'écoute
 * clt_sockaddr: pointeur sur une structure sockaddr devant recevoir
 *               les informations de connexion du client
 *
 * retourne: le numéro de la socket créée pour le nouveau client  */
int accept_clt_conn(const int srv_sock, struct sockaddr_in *const clt_sockaddr){
  int clt_sock = -1;
  int addrlen = ADDRLEN;
  
  /* mise en attente de demande de conexion sur la socket */

  if (accept(srv_sock, clt_sockaddr, &addrlen) == -1)
    perror("Erreur lors de l'acceptation du socket");

  DEBUG("connexion accepté");

  return clt_sock;
}

    int main(int argc, char *argv[])
{
  signal(SIGINT, sig_handler);
  signal(SIGPIPE, sig_handler);
  
  DFLAG = 1;

  /* création de la socket d'écoute */

  int clt_sock = create_a_listening_socket(SRV_PORT, MAX_CONN);
  
  /* initialisation du salon de discussion */
  initialize_chat_room();
    
  while (1){
    struct sockaddr_in clt_sockaddr;
    char *clt_ip;
    int clt_port;

    char login;
    
    // acceptation d'une demande de connexion
    accept_clt_conn(clt_sock, &clt_sockaddr);

    // récupération des information client
    clt_ip = inet_ntoa(clt_sockaddr.sin_addr);
    clt_port = ntohs(clt_sockaddr.sin_port);

    login = clt_authentication(clt_sock);
    
    /* Enregistrement du client dans le salon */
    if ( login_chatroom(clt_sock, clt_ip, clt_port) != 0 ) 
    { 
      DEBUG("client %s:%d not accepted", clt_ip, clt_port);
      close(clt_sock);
      DEBUG("close clt_sock %s:%d", clt_ip, clt_port);
    }
    
  } /* while */

  return EXIT_SUCCESS;
}
