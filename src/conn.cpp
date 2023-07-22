#include "./../header/conn.h"
#include <iostream>

// Data structure for the key space. May be replaced in future.
std::map<std::string, std::string> g_map;

void conn_put(std::vector<Conn *> &fd2conn, struct Conn *conn)
{
    if (fd2conn.size() <= (size_t)conn->fd)
    {
        fd2conn.resize(conn->fd + 1);
    }
    fd2conn[conn->fd] = conn;
}

int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd)
{
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0)
    {
        msg("accept() error");
        return -1; // error
    }

    // set the new connection fd to nonblocking mode
    fd_set_nb(connfd);
    // creating the struct Conn
    struct Conn *conn = (struct Conn *)malloc(sizeof(struct Conn));
    if (!conn)
    {
        close(connfd);
        return -1;
    }
    conn->fd = connfd;
    conn->state = STATE_REQ;
    conn->rbuf_size = 0;
    conn->wbuf_size = 0;
    conn->wbuf_sent = 0;
    conn_put(fd2conn, conn);
    return 0;
}

// State machine: Reader.
bool try_fill_buffer(Conn *conn)
{
    // try to fill the buffer
    assert(conn->rbuf_size < sizeof(conn->rbuf));
    ssize_t rv = 0;
    do
    {
        size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
        rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
        printf("rv: %ld\n", rv);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0 && errno == EAGAIN)
    {
        // got EAGAIN, stop.
        return false;
    }
    if (rv < 0)
    {
        perror(" ");
        msg("read() error3");
        conn->state = STATE_END;
        return false;
    }
    if (rv == 0)
    {
        if (conn->rbuf_size > 0)
        {
            msg("unexpected EOF");
        }
        else
        {
            msg("EOF");
        }
        conn->state = STATE_END;
        return false;
    }

    conn->rbuf_size += (size_t)rv;
    assert(conn->rbuf_size <= sizeof(conn->rbuf));

    // Try to process requests one by one.
    while (try_one_request(conn))
    {
    }
    return (conn->state == STATE_REQ);
}

void state_req(Conn *conn)
{
    while (try_fill_buffer(conn))
    {
    }
}

uint32_t do_get(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen) {
    if(!g_map.count(cmd[1])) {
        msg("get not ok");
        return(RES_NX);
    }
    std::string &val = g_map[cmd[1]];
    std::cout << "---------------test val: " << val.data() << std::endl;
    assert(val.size() <= k_max_msg);
    memcpy(res, val.data(), val.size());
    *reslen = (uint32_t)val.size();
    msg("get ok");
    return(RES_OK);
}

uint32_t do_set(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen) {
    (void)res;
    (void)reslen;
    g_map[cmd[1]] = cmd[2];
    return(RES_OK);
}

uint32_t do_del(const std::vector<std::string> &cmd, uint8_t *res, uint32_t *reslen) {
    (void)res;
    (void)reslen;
    g_map.erase(cmd[1]);
    return(RES_OK);
}

bool cmd_is(const std::string &word, const char *cmd) {
    printf("test1: %s\ntest2: %s\n", cmd, word.c_str());
    return(0 == strcasecmp(word.c_str(), cmd));
}

int32_t do_request(const uint8_t *req, uint32_t reqlen, uint32_t *rescode, uint8_t *res, uint32_t *reslen)
{
    std::vector<std::string> command;
    if (parse_req(req, reqlen, command) != 0)
    {
        msg("Bad Request!");
        return (-1);
    }

    if (command.size() == 2 && cmd_is(command[0], "get"))
    {
        printf("in get\n");
        *rescode = do_get(command, res, reslen);
    }
    else if (command.size() == 3 && cmd_is(command[0], "set"))
    {
        printf("in set\n");
        *rescode = do_set(command, res, reslen);
    }
    else if (command.size() == 2 && cmd_is(command[0], "del"))
    {
        printf("in del\n");
        *rescode = do_del(command, res, reslen);
    }
    else
    {
        // Unknown command.
        *rescode = RES_ERR;
        const char *msg = "Unknown Command";
        strcpy((char *)res, msg);
        *reslen = strlen(msg);
    }
    return (0);
}

int32_t parse_req(const uint8_t *data, size_t len, std::vector<std::string> &cmd)
{
    if (len < 4)
    {
        return -1;
    }
    uint32_t n = 0;
    memcpy(&n, &data[0], 4);
    if (n > k_max_args)
    {
        return -1;
    }

    size_t pos = 4;
    while (n--)
    {
        if (pos + 4 > len)
        {
            return -1;
        }
        uint32_t sz = 0;
        memcpy(&sz, &data[pos], 4);
        if (pos + 4 + sz > len)
        {
            return -1;
        }
        cmd.push_back(std::string((char *)&data[pos + 4], sz));
        pos += 4 + sz;
    }

    if (pos != len)
    {
        return -1; // trailing garbage
    }
    return 0;
}

// Protocol parsing.
bool try_one_request(Conn *conn)
{
    // try to parse a request from the buffer
    if (conn->rbuf_size < 4)
    {
        // not enough data in the buffer. Will retry in the next iteration
        return false;
    }
    uint32_t len = 0;

    memcpy(&len, &conn->rbuf[0], 4);
    printf("try_one_request len: %d\n", len);
    if (len > k_max_msg)
    {
        msg("too long");
        conn->state = STATE_END;
        return false;
    }
    if (4 + len > conn->rbuf_size)
    {
        // not enough data in the buffer. Will retry in the next iteration
        return false;
    }

    // Receive one request, generate a response.
    uint32_t rescode = 0;
    uint32_t wlen = 0;
    int32_t err = do_request(&conn->rbuf[4], len, &rescode, &conn->wbuf[4 + 4], &wlen);
    if (err)
    {
        conn->state = STATE_END;
        return (false);
    }

    printf("rescode: %d\n", rescode);

    wlen += 4;
    memcpy(&conn->wbuf[0], &wlen, 4);
    memcpy(&conn->wbuf[4], &rescode, 4);
    conn->wbuf_size = 4 + wlen;

    // Remove the request from the buffer.
    // note: Frequent memmove is inefficient.
    // !TODO: Better handling for production code.
    size_t remain = conn->rbuf_size - 4 - len;
    if (remain)
    {
        memmove(conn->rbuf, &conn->rbuf[4 + len], remain);
    }
    conn->rbuf_size = remain;

    // change state
    conn->state = STATE_RES;
    state_res(conn);

    // continue the outer loop if the request was fully processed
    return (conn->state == STATE_REQ);
}

// State machine: Writer.
bool try_flush_buffer(Conn *conn)
{
    ssize_t rv = 0;
    do
    {
        size_t remain = conn->wbuf_size - conn->wbuf_sent;
        rv = write(conn->fd, &conn->wbuf[conn->wbuf_sent], remain);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0 && errno == EAGAIN)
    {
        // got EAGAIN, stop.
        return false;
    }
    if (rv < 0)
    {
        msg("write() error");
        conn->state = STATE_END;
        return false;
    }
    conn->wbuf_sent += (size_t)rv;
    assert(conn->wbuf_sent <= conn->wbuf_size);
    if (conn->wbuf_sent == conn->wbuf_size)
    {
        // response was fully sent, change state back
        conn->state = STATE_REQ;
        conn->wbuf_sent = 0;
        conn->wbuf_size = 0;
        return false;
    }
    // still got some data in wbuf, could try to write again
    return true;
}

void state_res(Conn *conn)
{
    while (try_flush_buffer(conn))
    {
    }
}

// State machine for client connections.
void connection_io(Conn *conn)
{
    if (conn->state == STATE_REQ)
    {
        state_req(conn);
    }
    else if (conn->state == STATE_RES)
    {
        state_res(conn);
    }
    else
    {
        assert(0); // not expected
    }
}
