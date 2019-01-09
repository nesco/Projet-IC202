#ifndef CHATROOM_H
#define CHATROOM_H

#define MAX_CLIENTS 5
#define IP_LENGTH 16

void initialize_chat_room();
void stop_chat_room();
int login_chatroom(const int clt_sock, const char *const ip, const int port);

#endif /* CHATROOM_H */
