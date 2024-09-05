#include <ev.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void onesec_timer_cb(EV_P_ ev_timer *w, int revents)
{
    int* cnt = (int*)(w->data);
    cout << "cb1 " << *cnt << endl;
    if (((*cnt)--) <= 0) {
        ev_break(EV_A_ EVBREAK_ALL);
    }
}

void threesec_timer_cb(EV_P_ ev_timer *w, int revents)
{
    cout << "cb3" << endl;
}

int main() {
    int call_count = 15; /* timeout after call_count seconds */

    /* create the loop variable by using the default */
    struct ev_loop *loop = EV_DEFAULT;

    /* create the timer event object */
    ev_timer timer_1 = { 0 };
    ev_timer timer_2 = { 0 };

    /* initialize the timer to update each second and invoke callback */
    /* here the callback name is onesec_timer_cb */
    ev_timer_init(&timer_1, onesec_timer_cb, 1., 1.);
    ev_timer_init(&timer_2, threesec_timer_cb, 1., 3.);

    /* set the data pointer for the callback to use it */
    timer_1.data = &call_count;

    /* start the timer */
    ev_timer_start(loop, &timer_1);
    ev_timer_start(loop, &timer_2);
    ev_run(loop, 0);

    return 0;
}

