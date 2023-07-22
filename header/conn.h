#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include "./utils.h"

#ifndef CONN_H
#define CONN_H

enum
{
    STATE_REQ = 0, // Reading requests.
    STATE_RES = 1, // Sending responses.
    STATE_END = 2  // Mark the connection for removal.
};

enum {
    RES_OK = 0,
    RES_ERR = 1,
    RES_NX = 2,
};

struct Conn
{
    int fd = -1;
    uint32_t state = 0; // STATE_REQ or STATE_RES

    // Buffer for reading.
    size_t rbuf_size = 0;
    uint8_t rbuf[4 + k_max_msg];

    // Buffer for writing.
    size_t wbuf_size = 0;
    size_t wbuf_sent = 0;
    uint8_t wbuf[4 + k_max_msg];
};

const size_t k_max_args = 1024;

void conn_put(std::vector<Conn *> &fd2conn, struct Conn *conn);
int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd);
bool try_fill_buffer(Conn *conn);
void state_req(Conn *conn);
bool try_one_request(Conn *conn);
void state_res(Conn *conn);
bool try_flush_buffer(Conn *conn);
int32_t do_request(const uint8_t *req, uint32_t reqlen, uint32_t *rescode, uint8_t *res, uint32_t *reslen);
int32_t parse_req(const uint8_t *data, size_t len, std::vector<std::string> &cmd);
uint32_t do_get(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);
uint32_t do_set(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);
uint32_t do_del(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen);
void connection_io(Conn *conn);

#endif
