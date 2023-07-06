#include "./../header/server.h"

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

    while (true)
    {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0)
        {
            continue; // error
        }

        // Perform operations.
        // read_write_test(connfd);

        // only serves one client connection at once
        while (true) {
            int32_t err = one_request(connfd);
            if (err) {
                break;
            }
        }

        close(connfd);
    }

    return 0;
}
