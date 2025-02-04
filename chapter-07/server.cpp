// stdlib
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// system
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
// C++
#include <string>
#include <vector>
#include <map>

static void msg(const char *msg){
    fprintf(stderr, "%s\n", msg);
}

static void msg_errno(const char *msg){
    fprintf(stderr, "[%d] %s\n", errno, msg);
}

static void die(const char *msg){
    fprintf(stderr, "[%d] %s\n", msg);
    abort();
}

static void fd_set_nb(int fd){
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno){
        die("fcntl(F_GETFL) error");
        return;
    }

    flags |= O_NONBLOCK;

    errno = 0;
    (void)fcntl(fd, F_SETFL, flags);
    if (errno){
        die("fcntl(F_SETFL) error");
    }
}

const size_t k_max_msg = 32 << 20; // 32MB, hopefully larger than linux kernel buffer

struct Conn{
    int fd = -1;
    // application's intention, for the event loop
    bool want_read = false;
    bool wan_write = false;
    bool want_close = false;
    // buffered input and output
    std::vector<uint8_t> incoming;   // data to be parsed by the application
    std::vector<uint8_t> outgoing;  // responses generated by the application
};

// append to the back
static void buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len){
    buf.insert(buf.end(), data, data + len);
}

// remove from the front
static void buf_consume(std::vector<uint8_t> &buf, size_t n){
    buf.erase(buf.begin(), buf.begin() + n);
}

// application callback when the listening socket is ready
static Conn *handle_accept(int fd){
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr*)&client_addr, &socklen);
    if (connfd < 0){
        msg_errno("accept() error");
        return NULL;
    }
    uint32_t ip = client_addr.sin_addr.s_addr;
    fprintf(stderr, "new client from %u.%u.%u.%u\n",
            ip & 255,
            (ip >> 8) & 255,
            (ip >> 16) & 255,
            (ip >> 24),
            ntohl(client_addr.sin_port)
    );

    // set the new connection fd to non-blocking
    fd_set_nb(connfd);

    // create a struct `Conn`
    Conn *conn = new Conn;
    conn->fd = connfd;
    conn->want_read = true;
    return conn;
}

const size_t k_max_args = 200 * 1000; // 200KB

static bool read_u32(const uint8_t *&cur, const uint8_t *end, uint32_t &out) {
    if (cur + 4 > end){
        return false;
    }
    memcpy(&out, cur, 4);
    cur += 4;
    return true;
}

static bool read_str(const uint8_t *&cur, const uint8_t *end, size_t n, std::string &out) {
    if (cur + n > end){
        return false;
    }
    out.assign(cur, cur + n);
    cur += n;
    return true;
}

// +------+-----+------+-----+------+-----+-----+------+
// | nstr | len | str1 | len | str2 | ... | len | strn |
// +------+-----+------+-----+------+-----+-----+------+

