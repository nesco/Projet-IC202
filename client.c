#include "common.h"
#include <signal.h>

char *srv_name = "localhost";
int clt_sock;

int DFLAG;

void sig_handler(int s)
{
  msg_header_t msg;

  switch (s) 
    {
    case SIGINT:
      msg.type = END_OK;
      msg.size = 0;
      
      send_msg(clt_sock, msg.type, 0, NULL);
      //send(clt_sock, &msg, HEADER_SIZE, 0);
  
      close(clt_sock);
      exit(EXIT_SUCCESS);
    default: return;
    }
}

/* Établie une session TCP vers srv_name sur le port srv_port
 * char *srv_name: nom du serveur (peut-être une adresse IP)
 * int srv_port: port sur lequel la connexion doit être effectuée
 *
 * renvoie: descripteur vers le socket
 */ 
int connect_to_server(const char *const srv_name, const char *const srv_port){
  //struct hostent *host;
  //struct sockaddr_in sock_addr;
  struct addrinfo hints;
  struct addrinfo *result = NULL, *rp = NULL;
  int ret_code;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  //struct in_addr ip_addr;
  int clt_sock = -1;

  memset(&hints, 0, sizeof(struct addrinfo));

  /* Récupération des informations de connexion au serveur */

  if (getaddrinfo("127.0.0.1", SRV_PORT, &hints, &result) != 0)
    PERROR("Erreur de récupération des informations de connexion");

// Boucle sur les interfaces réseaux 
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    /* Tentative création de la socket */
    clt_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (clt_sock == -1)
      PERROR("Erreur lors de la création du soscket");

    /* Tentative connexion au serveur */

    if (connect(clt_sock, rp->ai_addr, rp->ai_addrlen) == -1)
    {
      PERROR("Erreur lors de la tentative de connexion");
    }
    else
      break;

    /* Sinon, essayer l'entrée suivante */
  }
  
  if (rp == NULL){  /* No address succeeded */
    PERROR("socket");
    return -1;
  }

  freeaddrinfo(result); /* no longer needed */

  return clt_sock;
}

int authenticate(const int clt_sock){
  
  while (1){
    unsigned char code;
    uint16_t size;
    char login[BUFFSIZE];
    
    /* Code nécessaire à l'authentification auprès du serveur :

       - attendre un paquet AUTH_REQ

       - répondre avec un paquet AUTH_RESP
     
       - attendre un paquet ACCESS_OK / ACCESS_DENIED / AUTH_REQ

       - agir en conséquence ...

    */

    return -1;
  } /* while */
}

int instant_messaging(const int clt_sock){
  
  while(1){
    /*fd_set rset;
    unsigned char code;
    uint16_t size;
    char *data;
    
    FD_ZERO(&rset);
    FD_SET(clt_sock, &rset);
    FD_SET(STDIN_FILENO, &rset);
    */

    /* pour les étapes 2 à 4 se contenter de recevoir les messages
       envoyés par le serveur et les afficher à l'utilisateur
    */

    /*if (select(clt_sock+1, &rset, NULL, NULL, NULL) < 0){
      PERROR("select");
      exit(EXIT_FAILURE);
      }*/
    
    /*if (FD_ISSET(STDIN_FILENO, &rset)){
      DEBUG("STDIN_FILENO isset");
      data = malloc(BUFFSIZE);
      if (fgets(data, BUFFSIZE, stdin) == NULL){
	/* gérer feof et ferror */

    //   <COMPLÉTER>
    
    //	return 0;
    //}
    // size = strlen(data)+1;
      
    // DEBUG("sending MESG %s(%d)", data, size);
      
    //  <COMPLÉTER>

    // free(data);
      
    //}

    //if (FD_ISSET(clt_sock, &rset)){
      /* réception d'un message du serveur */
      /* expected: <code><datalen>[<data>] */

      //  <COMPLÉTER>

    //}
    
  } /* while (1) */

  return 0;
}

int main(int argc, char *argv[]){
  // char srv_name[BUFFSIZE];
  char *srv_port = SRV_PORT;

  DFLAG = 1;

  signal(SIGINT, sig_handler);
  
  clt_sock = connect_to_server(srv_name, srv_port);
  if (clt_sock < 0)
    exit(EXIT_FAILURE);

  if (authenticate(clt_sock) < 0){
    close(clt_sock);
    eprintf("connexion closed\n");
    exit(EXIT_FAILURE);
  }

  if (instant_messaging(clt_sock) < 0){
    close(clt_sock);
    eprintf("connexion closed\n");
    exit(EXIT_FAILURE);
    }

  close(clt_sock);
  eprintf("connexion closed\n");
  exit(EXIT_SUCCESS);
}
