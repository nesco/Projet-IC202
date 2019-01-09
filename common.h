#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <inttypes.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip.h>

#include <ctype.h>

#include <getopt.h>

#define BUFFSIZE 256

#define ADDRLEN sizeof(struct sockaddr_in)

#define SRV_PORT "6008"

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define DEBUG(...) {if (DFLAG!=0) {					\
      fprintf(stderr, "%s@%s,line %d: ", __func__, __FILE__, __LINE__); \
      fprintf(stderr, __VA_ARGS__);					\
      fprintf(stderr, "\n");						\
      fflush(stderr);							\
    }}

#define PERROR(str) {							\
    fprintf(stderr, "%s@%s,line %d: ", __func__, __FILE__, __LINE__);	\
    perror((str));							\
  }

/* Protocol codes */
#define AUTH_REQ 11
#define AUTH_RESP 21

#define ACCESS_OK 12
#define ACCESS_DENIED 13
#define BUSY 14

#define MESG 21

#define END_OK 32
#define END_ERROR 33

/* 0 means "off", any other integer means "on" */
extern int DFLAG;

/* structure des messages échangés 
   type + size forment l'entête du message,
   data, le contenu du message
 */
typedef struct
{
  unsigned char type;
  uint16_t size;
} msg_header_t;

#define HEADER_SIZE sizeof(msg_header_t)

int send_msg(const int sock,
	     const unsigned char code,
	     const uint16_t size,
	     const char *const body);
int recv_msg(const int sock,
	     unsigned char *const code,
	     uint16_t *const size,
	     char **const body);

#endif /* COMMON_H */
