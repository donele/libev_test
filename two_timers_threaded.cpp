#include <ev.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

void onesec_timer_cb(EV_P_ ev_timer *w, int revents)
{
    int* cnt = (int*)(w->data);
    char buf[16];
    sprintf(buf, "cb1 %d", *cnt);
    cout << buf << endl;
    if (((*cnt)--) <= 0) {
        ev_break(EV_A_ EVBREAK_ALL);
    }
}

void threesec_timer_cb(EV_P_ ev_timer *w, int revents)
{
    int* cnt = (int*)(w->data);
    char buf[16];
    sprintf(buf, "cb3 %d", *cnt);
    cout << buf << endl;
    if (((*cnt)--) <= 0) {
        ev_break(EV_A_ EVBREAK_ALL);
        sleep(3);
    }
}

void timer_thread() {
    int call_count = 6; /* timeout after call_count seconds */
    struct ev_loop *loop = ev_loop_new(0);
    ev_timer timer = { 0 };
    timer.data = &call_count;
    ev_timer_init(&timer, threesec_timer_cb, 1., 3.);
    ev_timer_start(loop, &timer);
    ev_run(loop, 0);
}

int main() {
    thread th(timer_thread);

    int call_count = 15; /* timeout after call_count seconds */
    struct ev_loop *loop = ev_loop_new(0);
    ev_timer timer = { 0 };
    ev_timer_init(&timer, onesec_timer_cb, 1., 1.);
    timer.data = &call_count;
    ev_timer_start(loop, &timer);
    ev_run(loop, 0);

    th.join();

    return 0;
}

