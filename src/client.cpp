#include "./../header/client.h"

// Splitting Query function implementing our protocol into send_req and read_res.
int32_t send_req(int fd, const std::vector<std::string> &cmd)
{
    uint32_t len = 4;
    for (const std::string &s : cmd)
    {
        len += 4 + s.size();
    }

    if (len > k_max_msg)
    {
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4); // assume little endian
    uint32_t n = cmd.size();
    memcpy(&wbuf[4], &n, len);
    size_t cur = 8;

    for (const std::string &s : cmd)
    {
        uint32_t p = (uint32_t)s.size();
        memcpy(&wbuf[cur], &p, 4);
        memcpy(&wbuf[cur + 4], s.data(), s.size());
        cur += 4 + s.size();
    }
    return (write_all(fd, wbuf, 4 + len));
}

int32_t read_res(int fd)
{
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err)
    {
        if (errno == 0)
        {
            msg("EOF");
        }
        else
        {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // assume little endian
    if (len > k_max_msg)
    {
        msg("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &rbuf[4], len);
    if (err)
    {
        msg("read() error");
        return err;
    }

    // print the result
    uint32_t rescode = 0;
    if (len < 4)
    {
        msg("bad response");
        return -1;
    }
    memcpy(&rescode, &rbuf[4], 4);
    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
}
