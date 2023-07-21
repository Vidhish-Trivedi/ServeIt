#include "./../header/server.h"
#include <vector>
#include <iostream>
#include <poll.h>
#include "./../header/conn.h"

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    // Allow server to bind to the same address if restarted.
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("bind(): unsuccessful");
    }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv)
    {
        die("listen(): unsuccessful");
    }

    // map from fd to connection.
    std::vector<struct Conn *> fd_to_conn;

    // Set the listen fd to non-blocking mode.
    fd_set_nb(fd);

    // The event loop.
    std::vector<struct pollfd> poll_args;

    while (true)
    {
        // Prepare arguments of poll.
        poll_args.clear();
        struct pollfd pfd = {fd, POLLIN, 0}; // Listening fd is in first position.
        poll_args.push_back(pfd);

        // Connection fds.
        for (struct Conn *conn : fd_to_conn)
        {
            if (!conn)
            {
                continue;
            }
            struct pollfd pfd = {};
            pfd.fd = conn->fd;
            pfd.events = (conn->state == STATE_REQ) ? POLLIN : POLLOUT;
            pfd.events = pfd.events | POLLERR;
            poll_args.push_back(pfd);
        }

        // Poll for active fds.
        int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), 1000);    // Ignore timeout for now.
        if (rv < 0)
        {
            die("poll(): unsuccessful");
        }

        // Process active connections.
        for (size_t i = 1; i < poll_args.size(); ++i)
        {
            if (poll_args[i].revents)
            {
                Conn *conn = fd_to_conn[poll_args[i].fd];
                connection_io(conn);

                if (conn->state == STATE_END)
                {
                    // client closed normally, or something bad happened.
                    // destroy this connection
                    fd_to_conn[conn->fd] = NULL;
                    (void)close(conn->fd);
                    free(conn);
                }
            }
        }

        // Check and accept a new connection if the listening fd is active.
        if (poll_args[0].revents)
        {
            (void)accept_new_conn(fd_to_conn, fd);
        }
    }

    return 0;
}
