#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "./utils.h"

#ifndef CONN_H
#define CONN_H

enum
{
    STATE_REQ = 0, // Reading requests.
    STATE_RES = 1, // Sending responses.
    STATE_END = 2  // Mark the connection for removal.
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

void conn_put(std::vector<Conn *> &fd2conn, struct Conn *conn);
int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd);
bool try_fill_buffer(Conn *conn);
void state_req(Conn *conn);
bool try_one_request(Conn *conn);
void state_res(Conn *conn);
bool try_flush_buffer(Conn *conn);

void connection_io(Conn *conn);

#endif
