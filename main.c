#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdio.h> 
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
    printf("client connected\n");
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
        if (total > 4096) {
            printf("too much data\n");
            break;
        }
    }
    printf("client disconnected, bytes: %d\n", total);
    close(conn);
}

// Usage
void usage() {
    printf("Usage: %s localhost 9000\n\n", program);
    exit(1);
}

// main()
int neco_main(int argc, char *argv[]) {
    size_t len = 0;
    char *name = NULL;
    char *port = NULL;
    char *listens = NULL;
 
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
    printf("listening at %s\n", listens);
    while (1) {
        int conn = neco_accept(ln, 0, 0);
        if (conn > 0) {
            neco_start(client, 1, &conn);
        }
    }
    close(ln);
    xfree(listens);
    return 0;
}