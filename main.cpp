#include <cstdint>
#include <event2/event_struct.h>
#include <random>
#include <string.h>
#include <unistd.h>
#include <meow.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <evhttp.h>

std::random_device dev;
std::mt19937 rng(dev());

uint16_t randomInt(uint16_t min, uint16_t max)
{
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}

void send_next_chunk(evutil_socket_t fd, short events, void* args)
{
    struct evhttp_request* req = (struct evhttp_request*)args;
    if(req->evcon == nullptr)
        return;

    char buf[256];
    short meowCount = randomInt(1, 15);
    memset(buf, 0, sizeof(buf));

    for(int i = 0; i < meowCount; i++)
    {
        generateMeowString(buf, randomInt);
    }

    struct evbuffer* chunk = evbuffer_new();
    evbuffer_add_printf(chunk, "%s", buf);
    evhttp_send_reply_chunk(req, chunk);
    evbuffer_free(chunk);

    struct timeval tv = {.tv_sec = 0, .tv_usec = 1000 * randomInt(50, 300)};
    evtimer_add(event_new(evhttp_connection_get_base(req->evcon), -1, 0, send_next_chunk, req), &tv);
}

void meow_handler(struct evhttp_request* req, void* arg)
{
    if(evhttp_request_get_command(req) == EVHTTP_REQ_MEOW)
    {
        struct evbuffer* buf = evbuffer_new();

        evbuffer_add_printf(buf, "MEOW\n");

        evhttp_add_header(req->output_headers, "Content-Type", "text/plain");
        evhttp_send_reply_start(req, HTTP_OK, "OK");

        struct event_base* base = evhttp_connection_get_base(req->evcon);
        if(base == nullptr)
        {
            evbuffer_free(buf);
            return;
        }
        struct event* timer = evtimer_new(base, send_next_chunk, req);

        struct timeval tv = {.tv_sec = 0, .tv_usec = 1000 * randomInt(50, 300)};
        evtimer_add(timer, &tv);

        evbuffer_free(buf);
    }
    else
    {
        evhttp_send_error(req, HTTP_BADMETHOD, "Only MEOW allowed");
    }
}

int main(int argc, char** argv)
{
    struct event_base* base = event_base_new();
    struct evhttp* http = evhttp_new(base);

    evhttp_bind_socket(http, "0.0.0.0", 8080);
    evhttp_set_gencb(http, meow_handler, NULL);

    event_base_dispatch(base);

    evhttp_free(http);
    event_base_free(base);
    return 0;
}
