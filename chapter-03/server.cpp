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
    fprintf(stderr,"%s\n",msg);
}

static void die(const char *msg){
    int err = errno;
    fprintf(stderr, "[%d] %s \n", err, msg);
    abort();
}

static void do_something(int connfd){
    char rbuf[64] = {};
    ssize_t n = read(connfd,rbuf,sizeof(rbuf)-1);
    if (n<0){
        die("could not read");
    }

    fprintf(stderr, "client says: %s\n", rbuf);
    char wbuf[] = "world";
    write(connfd,wbuf,strlen(wbuf));
}

const int PORT = 1234;
int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd<0){
        die("could not create socket");
    }

    // this is needed for most server applications
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(PORT);
    addr.sin_addr.s_addr = ntohl(0); // 0.0.0.0
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));  
    if (rv){
        die("could not bind");
    }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv){
        die("could not listen");
    }

    // accept
    while(true){
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr,&socklen);
        if (connfd<0){
            msg("could not accept");
            continue;
        }

        do_something(connfd);
        close(connfd);
    }

    return 0;
}
