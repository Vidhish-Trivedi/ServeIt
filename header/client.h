#include <vector>
#include <string>
#include "./utils.h"
#ifndef CLIENT_H
#define CLIENT_H
int32_t send_req(int fd, const std::vector<std::string> &cmd);
int32_t read_res(int fd);

#endif