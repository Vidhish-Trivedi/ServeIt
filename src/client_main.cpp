#include "./../header/client.h"

int main(int argc, char **argv)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("socket(): unsuccessful");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("connect(): unsuccessful");
    }

    // Code to test the query function.
    std::vector<std::string> command;
    for(size_t i = 1; i < argc; ++i) {
        command.push_back(argv[i]);
    }

    int32_t err = send_req(fd, command);
    if(err) {
        goto L_DONE;
    }

    err = read_res(fd);
    if(err) {
        msg("e2");
        printf("%d\n", err);
        goto L_DONE;
    }
    

L_DONE:
    close(fd);
    return 0;
}