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

const size_t k_max_msg = 4096; // 4KB

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

static int32_t one_request(int connfd){
    // 4 bytes header
    char rbuf[4 + k_max_msg];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err){
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    uint32_t len;
    memcpy(&len, rbuf, 4); // assume little indian
    if (len > k_max_msg){
        msg("too long");
        return -1;
    }

    // request boddy
    err = read_full(connfd, &rbuf[4], len);
    if (err){
        msg("read() error");
        return err;
    }

    // do something
    printf("client says: %.*s\n", len, &rbuf[4]);
    
    // reply using the same protocol
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(wbuf + 4, reply, len);
    return write_all(connfd, wbuf, 4 + len);
}

const int PORT = 12345;

int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0){
        die("socket() error");
    }

    // this is needed for most server applications
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = ntohl(0); // 0.0.0.0
    int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));
    if (rv < 0){
        die("bind() error");
    }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv){
        die("listen() error");
    }

    // accept
    while(true){
       struct sockaddr_in client_addr = {};
       socklen_t socklen = sizeof(client_addr);
       int connfd = accept(fd, (struct sockaddr*)&client_addr, &socklen);     
       if (connfd < 0){
           msg("accept() error");
           continue;
       }   

       // handle multiple requests from the same client 
       while(true){
          // here the server only serves one client connection at once 
          int32_t err = one_request(connfd);
          if (err){
            msg("one_request() error");
            break;
          } 
       }
       close(connfd);    
    }

    return 0;
}
