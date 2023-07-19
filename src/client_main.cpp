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

    // Code to test the query function.
    const char* query_list[3] = {"hello1", "hello2", "hello3"};
    for(size_t i = 0; i < 3; i++) {
        int32_t err = send_req(fd, query_list[i]);
        if(err) {
            close(fd);
            return(0);
        }
    }

    for(size_t i = 0; i < 3; i++) {
        int32_t err = read_res(fd);
        if(err) {
            close(fd);
            return(0);
        }
    }

    close(fd);
    return 0;
}