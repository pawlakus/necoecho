#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdio.h> 
#include <inttypes.h>
#include <arpa/inet.h>
#include "neco/neco.h"

// Global
char *program = NULL;

// alloc/realloc/free
void *xmalloc(size_t nbytes) {
    void *ptr = malloc(nbytes);
    if (!ptr) {
        perror("malloc");
        abort();
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t nbytes) {
    ptr = realloc(ptr, nbytes);
    if (!ptr) {
        perror("realloc");
        abort();
    }
    return ptr;
}

void xfree(void *ptr) {
    if (ptr) {
        free(ptr);
    }
}

// Handler for new clients
void client(int argc, void *argv[]) {
    int conn = *(int*)argv[0];
    int total = 0;
    ssize_t nbytes;
    ssize_t ret;
    char buf[4096];
    int64_t read_deadline;
    int64_t write_deadline;
    int64_t id = neco_getid();
    fprintf(stderr, "coro %" PRIx64 " started\n", id);
    read_deadline = neco_now() + 5 * NECO_SECOND;
    while (1) {
        // Read into buf
        nbytes = neco_read_dl(conn, buf, sizeof(buf), read_deadline);
        if (nbytes <= 0) {
            break;
        }
        total += (int)nbytes;

        // write back
        write_deadline = neco_now() + 1 * NECO_SECOND;
        ret = neco_write_dl(conn, buf, nbytes, write_deadline);
        if (ret < 0) {
            break;
        }

        // too much data? disconnect the client
        if (total >= 4096) {
            fprintf(stderr, "coro %" PRIx64 " too much data\n", id);
            break;
        }
    }
    fprintf(stderr, "coro %" PRIx64 " finished, bytes: %d\n", id, total);
    close(conn);
}

// Usage
void usage() {
    fprintf(stdout, "Usage: %s localhost 9000\n\n", program);
    exit(1);
}

// main()
int neco_main(int argc, char *argv[]) {
    int64_t id = neco_getid();
    size_t len = 0;
    char *name = NULL;
    char *port = NULL;
    char *listens = NULL;
    int err;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    char ipstr[INET6_ADDRSTRLEN];
 
    // parsing arguments
    program = argv[0];
    if (argc != 3) {
        usage();
    }
    name = argv[1];
    port = argv[2];
    len = strlen(name) + strlen(port) + 2;
    listens = xmalloc(len);
    snprintf(listens, len, "%s:%s", name, port);

    // listener
    int ln = neco_serve("tcp", listens);
    if (ln == -1) {
        perror("neco_serve");
        exit(1);
    }
    fprintf(stderr, "listening at %s\n", listens);
    while (1) {
        int conn = neco_accept(ln, (struct sockaddr *)&addr, &addrlen);
        if (conn > 0) {
            // check client IP address
            if (addr.ss_family == AF_INET) {
                // IPv4 address
                struct sockaddr_in *s = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &(s->sin_addr), ipstr, sizeof(ipstr));
            } else if (addr.ss_family == AF_INET6) {
                // IPv6 address
                struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &(s->sin6_addr), ipstr, sizeof(ipstr));
            }
            fprintf(stderr, "coro %" PRIx64 " accepted client %s\n", id, ipstr);
            // spawn coroutine handler
            err = neco_start(client, 1, &conn);
            if (err != NECO_OK) {
                fprintf(stderr, "fatal: unable to start coroutine: %s\n", neco_strerror(err));
                close(conn);
            }
        }
    }
    close(ln);
    xfree(listens);
    return 0;
}