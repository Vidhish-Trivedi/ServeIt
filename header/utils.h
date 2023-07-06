#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#ifndef UTILS_H
#define UTILS_H

static void read_write_test(int connfd);
// Fix max size of one request.
const size_t k_max_msg = 4096;
static void die(const char *msg);
static void msg(const char *msg);
static int32_t read_full(int fd, char *buf, size_t n);
static int32_t write_all(int fd, const char *buf, size_t n);

#endif