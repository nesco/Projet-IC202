#include "common.h"

/*int datsize(const unsigned char type)
{
  int datumsize;
  if (strcmp(type, "char"))
    datumsize = sizeof(char);
  if (strcmp(type, "int"))
    datumsize = sizeof(int);
  if (strcmp(type, "long"))
    datumsize = sizeof(long);

  return datumsize;
}*/

/* send_msg send a message on socket sock
   sock: the socket
   type: message's protocol type
   size: message's size
   msg: message to be sent
*/


int send_msg(const int sock,
	     const unsigned char type,
	     const uint16_t size,
	     const char *const body) 
{
  msg_header_t msg;
  
  msg.type = type;
  msg.size = size;

  /* Code nécessaire à envoyer le message correspondant au protocle
     sur la socket
  */

  int length[2];
  length[0] = send(sock, &msg, HEADER_SIZE, 0);
  length[1] = send(sock, body, size, 0);

  return 0;
}

/* recv_msg recv a message from the socket sock
   sock: the socket
   type: message's protocol type
   size: message's size
   msg: message to be received
*/
int recv_msg(const int sock,
	     unsigned char *const type,
	     uint16_t *const size,
	     char **const body) 
{
  // J'ai supprimé le const  de size
  msg_header_t msg;

  int length[2];

  length[0] = recv(sock, &msg, HEADER_SIZE, 0);
  length[1] = recv(sock, body, msg.size, 0);
  *size = msg.size;

  return 0;
}
