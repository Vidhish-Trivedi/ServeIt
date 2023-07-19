#include "./utils.h"
#ifndef CLIENT_H
#define CLIENT_H
int32_t send_req(int fd, const char *text);
int32_t read_res(int fd);

#endif