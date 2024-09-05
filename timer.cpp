// a single header file is required
#include <ev.h>
#include <stdio.h> // for puts

void onesec_timer_cb(EV_P_ ev_timer *w, int revents)
{
    puts ("cb");
    int* cnt = (int*)(w->data);
    if (((*cnt)--) <= 0) {
        ev_break(EV_A_ EVBREAK_ALL);
    }
}

int main() {
    int call_count = 5; /* timeout after 30 seconds */

    /* create the loop variable by using the default */
    struct ev_loop *loop = EV_DEFAULT;

    /* create the timer event object */
    ev_timer timer_watcher = { 0 };

    /* initialize the timer to update each second and invoke callback */
    /* here the callback name is onesec_timer_cb */
    ev_timer_init(&timer_watcher, onesec_timer_cb, 1., 1.);

    /* set the data pointer for the callback to use it */
    timer_watcher.data = &call_count;

    /* start the timer */
    ev_timer_start(loop, &timer_watcher);
    ev_run(loop, 0);

    return 0;
}
