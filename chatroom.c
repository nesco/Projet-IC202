#include "chatroom.h"
#include "common.h"
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#define MAX_AUTH_ATTEMPS 3

typedef struct 
{
  int sock;
  char login[BUFFSIZE];
  char ip[IP_LENGTH];
  int port;
} buddy_t;
  
pthread_t chatroom_id;
buddy_t chat_room[MAX_CLIENTS];
int curr_nb_clients = 0;

/* new_client is a handler used to catch SIGUSR1 sent when a new
   client is registered in the chat room */
void new_client(int s)
{
  switch (s)
    {
    case SIGUSR1: // new client signaled
      break;
    default:
      break;
    }
}

void *chatroom(void *arg);

void initialize_chat_room()
{
  DEBUG("Initializing chat room");
  bzero(chat_room, sizeof(chat_room));

  /* using SIGUSR1 to signal new client
     (will interrupt select call in chatroom() )
   */
  signal(SIGUSR1, new_client);

  /* Create the chat room */
  pthread_create(&chatroom_id, NULL, &chatroom, NULL);
}

int broadcast_shutdown();

/* stop_chat_room() should be a safe function */
void stop_chat_room()
{
  /* Create the chat room */
  pthread_cancel(chatroom_id);
  broadcast_shutdown();

  pthread_join(chatroom_id, NULL);
}

int register_new_client(const int sock,
			const char *const login,
			const char *const ip, const int port) 
{
  int i = 0;

  DEBUG("registering client %s(%s:%d)", login, ip, port);

  /* find the first empty cell in chat_room */
  while (chat_room[i].sock != 0 && i < MAX_CLIENTS ) i++;
  
  if (i >= MAX_CLIENTS )
    { /* already too many clients */
      DEBUG("registration failed: already too many client");
      send_msg(sock, BUSY, 0, NULL);

      return -1;
    }

  chat_room[i].sock = sock;
  strncpy(chat_room[i].login, login, BUFFSIZE);
  strncpy(chat_room[i].ip, ip, IP_LENGTH);
  chat_room[i].port = port;

  curr_nb_clients++;
  
  DEBUG("client %s(%s:%d) registered", chat_room[i].login, chat_room[i].ip, chat_room[i].port);
  DEBUG("total number of registered clients: %d",curr_nb_clients );
  
  /* signal the new client in the chatroom */
  pthread_kill(chatroom_id, SIGUSR1);
    
  return curr_nb_clients;
}

int deregister_client(const int sock) 
{
  int i = 0;

  /* find the first empty cell in chat_room */
  while (chat_room[i].sock != sock && i < MAX_CLIENTS ) i++;
  
  if (i >= MAX_CLIENTS ) 
    { /* client not found */
      DEBUG("deregistration failed: client not found");
      return -1;
    }
  
  chat_room[i].sock = 0;
  DEBUG("client %s,%s:%d deregistered", chat_room[i].login, chat_room[i].ip, chat_room[i].port);

  return --curr_nb_clients;
}

int get_client_socket(const int i) 
{
  if (i >= MAX_CLIENTS || i < 0 ) 
    {
      DEBUG("get client socket failed: wrong value %d", i);
      return -1;
    }
  
  return chat_room[i].sock;
}

char *get_client_login(const int i) 
{
  if (i >= MAX_CLIENTS || i < 0 ) 
    {
      DEBUG("get client IP failed: wrong value %d", i);
      return NULL;
    }
  
  return chat_room[i].login;
}

char *get_client_ip(const int i) 
{
  if (i >= MAX_CLIENTS || i < 0 ) 
    {
      DEBUG("get client IP failed: wrong value %d", i);
      return NULL;
    }
  
  return chat_room[i].ip;
}

int get_client_port(const int i) 
{
  if (i >= MAX_CLIENTS || i < 0 ) 
    {
      DEBUG("get client port failed: wrong value %d", i);
      return -1;
    }
  
  return chat_room[i].port;
}

/* broadcast_shutdown send a END_OK message to all chat room clients
   This function is a safe function (could be called in a handler)
 */
int broadcast_shutdown() 
{
  int sock;
  msg_header_t msg;
  char *shutdown_msg = "Server is shutting down";
  msg.type = END_OK;
  msg.size = 23+1;
  
  if (curr_nb_clients == 0) 
    {
      return 2;
    }

  for (int i = 0; i < MAX_CLIENTS; i++) 
    {
      if ( (sock = chat_room[i].sock) != 0 )
	{ 
	  /* sending message head */
	  if ( send(sock, &msg, HEADER_SIZE, 0) == -1) 
	    { /* on error, don't try to send body */
	      continue;
	    }
	
	  /* sending message body if any */
	  send(sock, shutdown_msg, msg.size, 0);
	}
    } // end for
  
  return 0;  
}

/* broadcast_msg: send a message to all chat room client
 */
int broadcast_msg(const unsigned char code, const uint16_t size, const char *data)
{
  //int clt_sock;

  DEBUG("broadcasting message \"%s\"", data);

  // envoie le message à tous les clients connectée la socket d'un
  // client peut être récupérée avec la fonction get_client_socket(i),
  // i allant de 0 à MAX_CLIENTS-1

  return 0;  
}

int broadcast_text(const char *const login, const char *const data) 
{
  char *tell = " tells: ";
  char *text;
  uint16_t size;
  int retval;
  
  size = strlen(login)+strlen(tell)+strlen(data)+1;
  text = malloc(size);
  bzero(text, size);

  strncat(text, login, size);
  strncat(text, tell, size);
  strncat(text, data, size);

  retval = broadcast_msg(MESG, size, text);
  free(text);
  
  return retval;
}

/* clt_authentication authenticate a new buddy and return her login
   clt_sock: client socket
   return a pointer to newly allocated string containing the login
 */
char* clt_authentication(const int clt_sock){
  unsigned char code;
  uint16_t size;
  char* login;

  for(int attemp=0; attemp < MAX_AUTH_ATTEMPS; attemp++){
    /* 
       authentification du client :

       - envoi du AUTH_REQ

       - lecture du AUTH_RESP
       
       - envoi du ACCESS_OK / ACCESS_DENIED ou nouveau tour de boucle

       - en cas de succès retourne un pointeur vers la chaîne de
         caractère contenant le login, sinon retourne NULL

     */
    
  } /* for */

  /* too many failed attempt */
  if ( send_msg(clt_sock, ACCESS_DENIED, 0, NULL) == -1 ) 
    {
      PERROR("Sending ACCESS_DENIED failed");
      return NULL;
    }
  
  return NULL;
}

int login_chatroom(const int clt_sock, const char *const ip, const int port)
{
  char *login;
  
  if ( curr_nb_clients == MAX_CLIENTS ) 
    {
      DEBUG("Already too many clients");
      DEBUG("%s:%d refused", ip, port);
      
      if ( send_msg(clt_sock, BUSY, 0, NULL) == -1 ) 
	{
	  PERROR("Sending BUSY failed");
	  return -1;
	}
      
      return -1;
    }
  
  /* authenticate the connected client */
  login = clt_authentication(clt_sock);
  
  if ( login == NULL){
      DEBUG("authentication failed for %s:%d", ip, port);      
      return -1;
    }

  DEBUG("client %s(%s:%d) authenticated", login, ip, port);

  /* register the clients in the chat room */
  if ( register_new_client(clt_sock, login, ip, port) <=0 )
    {
      DEBUG("registration failed for %s(%s:%d)", login, ip, port);
      free(login);
      return -1;
    }
  
  /* client registered */
  DEBUG("client %s(%s:%d) logged in", login, ip, port);
  free(login);
    
  return 0;
}

void *chatroom(void *arg)
{
  while ( 1 ) 
    {
      fd_set rset;
      int clt_sock;
      int nfds;
      
      //unsigned char code;
      //uint16_t size;
      //char *data;
      
      FD_ZERO(&rset);

      /* adding clients' sockets to rset */
      for (int i = 0; i < MAX_CLIENTS; i++) 
	{
	  if ( (clt_sock = get_client_socket(i)) != 0 )
	    { /* client i */
	      DEBUG("adding %s to rset", get_client_login(i));
	      
	      FD_SET(clt_sock, &rset);
	      if (FD_ISSET(clt_sock, &rset)){
		DEBUG("%s(%d) is in rset", get_client_login(i), clt_sock);
	      } else 
		{
		  DEBUG("%s(%d) is not in rset", get_client_login(i), clt_sock);
		} 
	      nfds = clt_sock > nfds ? clt_sock: nfds;
	    }
	}
      
      if (select(nfds+1, &rset, NULL, NULL, NULL) <= 0){	
	if ( errno == EINTR ) {
	  DEBUG("select interrupted");
	  continue;
	}
	PERROR("select");
	return NULL;
      }
      	 
      /* find which client sent data */
      for (int i = 0; i < MAX_CLIENTS; i++) 
	{
	  clt_sock = get_client_socket(i);
	  if ( clt_sock == 0 ) continue;
	  
	  if (!FD_ISSET(clt_sock, &rset)){
	    continue;
	  }
      
	  /* read client i message */
	  /* 
	     - lecture du message émit par le client i

	     - si code MESG: envoie du message à tous les clients avec la fonction broadcast_text()
	     la fonction get_client_login(i) permet de récupérer le login du client i.

	     - sinon traité le message de façon approprié: un client
	     qui se déconnecte (END_OK) ou qui renvoi une erreur ou
	     provoque un erreur doit être retiré du salon de
	     discussion. La fonction deregister_client(i) permet de retirer le client i.

	  */
	}
      
    } /* while (1) */

  return NULL;
}
