#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <ev.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

static pthread_barrier_t barrier;
static pthread_t thread1;
int status;
char buffer [1024];

void Reverse (char *S)
{
        int i,j,l;
        char t;
        l=strlen(S);
        i = 0;
        j=l-1;
        while (i<j)
        {
                t=S[i];
                S[i]=S[j];
                S[j]=t;
                i++;j--;
        }
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
        size_t r = recv(watcher->fd, buffer, 1024, MSG_NOSIGNAL);
        if (r<0) {
                return;
        } else if(r == 0) {
                ev_io_stop(loop, watcher);
                free(watcher);
                return;
        } else {
                status = pthread_barrier_wait (&barrier);
                sleep (1);
                send(watcher->fd, buffer, r, MSG_NOSIGNAL);
                bzero(&buffer, sizeof(buffer));
        }
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
        int client_sd = accept(watcher->fd, 0, 0);
        struct ev_io *w_client = (struct ev_io*)
                                         malloc(sizeof(struct ev_io));
        ev_io_init(w_client, read_cb, client_sd, EV_READ);
        ev_io_start(loop, w_client);
}

void thread_func (void* port) {
        struct ev_loop *loop = ev_default_loop(0);
        int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = port;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sd, (struct sockaddr *)&addr, sizeof(addr));
        listen(sd, SOMAXCONN);
        struct ev_io w_accept;
        ev_io_init(&w_accept, accept_cb, sd, EV_READ);
        ev_io_start(loop, &w_accept);
        ev_loop(loop, 0);
}

static void my_cb (struct ev_loop *loop, struct ev_io *watcher, int revents) {
        status = pthread_barrier_init (&barrier, NULL, 2);
        status = pthread_barrier_wait (&barrier);
        Reverse(buffer);
}

int main(int argc, char* argv[]) {
        int port = htons(atoi(argv[1]));
        pthread_create (&thread1, NULL, thread_func, port);
        struct ev_loop *loop_new = ev_loop_new(0);
        struct ev_io stdin_watcher;
        ev_init (&stdin_watcher, my_cb);
        ev_io_set (&stdin_watcher, STDIN_FILENO, EV_WRITE);
        ev_io_start(loop_new, &stdin_watcher);
        ev_loop(loop_new, 0);
}
~                          