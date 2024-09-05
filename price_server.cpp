#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <ev.h>
#include <strings.h>
#include <iostream>
#include <thread>
#include <mutex>

#define PORT_NO 8080
#define BUFFER_SIZE 1024
std::mutex m;

struct Bar {
    Bar():na(-1e6){init();}
    double na;
    double open;
    double high;
    double low;
    double close;
    double tclose;
    void init() {
        open = na;
        high = na;
        low = na;
        close = na;
        tclose = 0;
    }
    void print_bar() {
        printf("ohlc %.2f %.2f %.2f %.2f\n", open, high, low, close);
    }
    void update_price(double price) {
        if(open == na)
            open = price;
        if(high == na || price > high)
            high = price;
        if(low == na || price < low)
            low = price;
        close = price;
    }
};

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void timer_cb(struct ev_loop *loop, struct ev_timer *watcher, int revents);
void timer_thread(Bar* bar);

int main()
{
    Bar bar;
    std::thread th(timer_thread, &bar);

    struct ev_loop *loop = ev_loop_new(0);
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    struct ev_io w_accept;

    // Create server socket
    if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
    {
        perror("socket error");
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NO);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (::bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
    {
        perror("bind error");
    }

    // Start listing on the socket
    if (listen(sd, 2) < 0)
    {
        perror("listen error");
        return -1;
    }

    // Initialize and start a watcher to accepts client requests
    ev_io_init(&w_accept, accept_cb, sd, EV_READ);
    w_accept.data = &bar;
    ev_io_start(loop, &w_accept);

    // Start infinite loop
    //while (1)
    {
        std::cout << "start loop" << std::endl;
        ev_run(loop, 0);
    }

    th.join();
    return 0;
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sd;
    struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

    if(EV_ERROR & revents)
    {
        perror("got invalid event");
        return;
    }

    // Accept client request
    client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_sd < 0)
    {
        perror("accept error");
        return;
    }

    // Initialize and start watcher to read client requests
    ev_io_init(w_client, read_cb, client_sd, EV_READ);
    w_client->data = watcher->data;
    ev_io_start(loop, w_client);
}

// Read client message
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents){
    char buffer[BUFFER_SIZE];
    ssize_t read;

    if(EV_ERROR & revents)
    {
        perror("got invalid event");
        return;
    }

    // Receive message from client socket
    read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

    if(read < 0)
    {
        perror("read error");
        return;
    }

    if(read == 0)
    {
        // Stop and free watchet if client socket is closing
        ev_io_stop(loop,watcher);
        free(watcher);
        perror("peer might be closing");
        return;
    }
    else
    {
        // Update bar
        double price = atof(buffer);
        Bar* bar = (Bar*)(watcher->data);
        {
            std::unique_lock<std::mutex> lock(m);
            bar->update_price(price);
        }
    }

    // Send message bach to the client
    // send(watcher->fd, buffer, read, 0);
    bzero(buffer, read);
}

void timer_thread(Bar* bar) {
    double interval = 5.;
    struct ev_loop *loop = ev_loop_new(0);
    ev_timer timer = { 0 };
    ev_timer_init(&timer, timer_cb, interval, interval);
    timer.data = bar;
    ev_timer_start(loop, &timer);
    ev_run(loop, 0);
}

void timer_cb(EV_P_ ev_timer *w, int revents)
{
    std::cout << "timer_cb" << std::endl;
    Bar* bar = (Bar*)(w->data);
    {
        std::unique_lock<std::mutex> lock(m);
        bar->print_bar();
        std::cout.flush();
        bar->init();
    }
}
