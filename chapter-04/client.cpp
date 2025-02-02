#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg){
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg){
    int err = errno;
    fprintf(stderr,"[%d] %s\n", err, msg);
    abort();
}

static int32_t read_full(int fd,char *buf, size_t n){
    while(n > 0){
        ssize_t rv = read(fd, buf,n);
        if (rv <= 0){
            return -1; // unexpected error
        }
        
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n){
    while (n>0){
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0){
            return -1; // unexpected error
        }

        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

const size_t k_max_msg = 4096; // 4KB

static int32_t query(int fd, const char *text){
    uint32_t len = (uint32_t)strlen(text);
    if (len > k_max_msg){
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4); // assume little indian
    memcpy(wbuf + 4, text, len);
    if (int32_t err = write_all(fd, wbuf, 4 + len)){
        return err;
    }

    // 4 bytes header
    char rbuf[4 + k_max_msg];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err){
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    memcpy(&len, rbuf, 4); // assume little indian
    if (len > k_max_msg){
        msg("too long");
        return -1;
    }

    // reply boddy
    err = read_full(fd, &rbuf[4], len);
    if (err){
        msg("read() error");
        return err;
    }

    // do something
    printf("server says: %.*s\n", len, &rbuf[4]);
    return 0;
}

const int PORT = 12345;

int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0){
        die("socket() error");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv < 0){
        die("connect() error");
    }

    // handle multiple requests
    int32_t err = query(fd, "hello");
    if (err){
        goto L_DONE;
    }
    err = query(fd, "hello 2");
    if (err){
        goto L_DONE;
    }
    err = query(fd, "hello 3");
    if (err){
        goto L_DONE;
    }


L_DONE:
    close(fd);
    return 0;
}