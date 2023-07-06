#include "./../header/client.h"

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("socket(): unsuccessful");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("connect(): unsuccessful");
    }

    char msg[] = "hello";
    ssize_t r = write(fd, msg, strlen(msg));
    if (r < 0)
    {
        die("write(): unsuccessful");
    }

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        die("read(): unsuccessful");
    }
    printf("server says: %s\n", rbuf);

    // Code to test the query function.
    while (true)
    {
        int32_t err = query(fd, "hello1");
        if (err)
        {
            break;
        }
        err = query(fd, "hello2");
        if (err)
        {
            break;
        }
        err = query(fd, "hello3");
        if (err)
        {
            break;
        }
        break;
    }

    close(fd);
    return 0;
}