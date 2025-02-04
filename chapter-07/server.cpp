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

static int32_t parse_req(const uint8_t *data, size_t size, std::vector<std::string> &out) {
    const uint8_t *end = data + size;
    uint32_t nstr = 0;
    if (!read_u32(data, end, nstr)){
        return -1;
    }
    if (nstr > k_max_args){
        return -1; // safety limit
    }

    while(out.size() < nstr){
        uint32_t len = 0;
        if (!read_u32(data, end, len)){
            return -1;
        }
        out.push_back(std::string());
        if (!read_str(data, end, len, out.back())){
            return -1;
        }
    }
    if (data != end){
        return -1; // trailing garbage
    }
    return 0;
}

// Response::status
enum {
    RES_OK = 0,
    RES_ERR = 1, // error
    RES_NX = 2,  // key not found
};

// +--------+---------+
// | status | data... |
// +--------+---------+

struct Response {
    uint32_t status = 0;
    std::vector<uint8_t> data;
};

// placeholder; implemented later
static std::map<std::string, std::string> g_data;

static void do_request(std::vector<std::string> &cmd, Response &out){
    if (cmd.size() == 2 && cmd[0] == "get"){
        auto it = g_data.find(cmd[1]);
        if (it == g_data.end()){
            out.status = RES_NX; // not found
            return;
        }
        const std::string &val = it->second;
        out.data.assign(val.begin(), val.end());
    } else if (cmd.size() == 3 && cmd[0] == "set"){
        g_data[cmd[1]].swap(cmd[2]);
    } else if (cmd.size() == 2 && cmd[0] == "del"){
        g_data.erase(cmd[1]);
    } else {
        out.status = RES_ERR; // unrecognized command
    }
}

static void make_response(const Response &resp, std::vector<uint8_t> &out) {
    uint32_t resp_len = 4 + (uint32_t)resp.data.size();
    buf_append(out, (const uint8_t *)&resp_len, 4);
    buf_append(out, (const uint8_t *)&resp.status, 4);
    buf_append(out, resp.data.data(),resp.data.size());
}

// process 1 request if there is enough data
static bool try_one_request(Conn *conn) {
    // try to parse the protocol: message header
    if (conn->incoming.size() < 4) {
        return false;   // want read
    }
    uint32_t len = 0;
    memcpy(&len, conn->incoming.data(), 4);
    if (len > k_max_msg) {
        msg("too long");
        conn->want_close = true;
        return false;   // want close
    }
    // message body
    if (4 + len > conn->incoming.size()) {
        return false;   // want read
    }
    const uint8_t *request = &conn->incoming[4];

    // got one request, do some application logic
    std::vector<std::string> cmd;
    if (parse_req(request, len, cmd) < 0) {
        msg("bad request");
        conn->want_close = true;
        return false;   // want close
    }
    Response resp;
    do_request(cmd, resp);
    make_response(resp, conn->outgoing);

    // application logic done! remove the request message.
    buf_consume(conn->incoming, 4 + len);
    // Q: Why not just empty the buffer? See the explanation of "pipelining".
    return true;        // success
}

