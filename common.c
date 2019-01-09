#include "common.h"

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
  
  // msg.code = code;
  // msg.size = size;

  /* Code nécessaire à envoyer le message correspondant au protocle
     sur la socket
  */

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
  msg_header_t msg;
  
  return 0;
}
