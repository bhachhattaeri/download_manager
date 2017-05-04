#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <signal.h>
#include <sys/time.h>

#include <libwebsockets.h>

//---------
int close_testing = 1;
int force_exit = 0;

struct lws_context *context;

//---------
enum demo_protocols {
    /* always first */
    PROTOCOL_DUMB_INCREMENT = 0,

    /* always last */
    DEMO_PROTOCOL_COUNT
    };

//-----------
struct per_session_data__dumb_increment {
        int number;
};
//********************
int  callback_dumb_increment(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len)
{
        unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512];
        struct per_session_data__dumb_increment *pss =
                        (struct per_session_data__dumb_increment *)user;
        unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
        int n, m;

        switch (reason) {

        case LWS_CALLBACK_ESTABLISHED:
                pss->number = 0;
                break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
                n = sprintf((char *)p, "%d", pss->number++);
                m = lws_write(wsi, p, n, LWS_WRITE_TEXT);
                if (m < n) {
                        lwsl_err("ERROR %d writing to di socket\n", n);
                        return -1;
                }
                if (close_testing && pss->number == 50) {
                        lwsl_info("close tesing limit, closing\n");
                        return -1;
                }
                break;

        case LWS_CALLBACK_RECEIVE:
                if (len < 6)
                        break;
                if (strcmp((const char *)in, "reset\n") == 0)
                        pss->number = 0;
                if (strcmp((const char *)in, "closeme\n") == 0) {
                        lwsl_notice("dumb_inc: closing as requested\n");
                        lws_close_reason(wsi, LWS_CLOSE_STATUS_GOINGAWAY,
                                         (unsigned char *)"seeya", 5);
                        return -1;
                }
                break;

        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
                break;

        /*
         * this just demonstrates how to handle
         * LWS_CALLBACK_WS_PEER_INITIATED_CLOSE and extract the peer's close
         * code and auxiliary data.  You can just not handle it if you don't
         * have a use for this.
         */
        case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
                lwsl_notice("LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: len %d\n",
                            len);
                for (n = 0; n < (int)len; n++)
                        lwsl_notice(" %d: 0x%02X\n", n,
                                    ((unsigned char *)in)[n]);
                break;

        default:
                break;
        }

        return 0;
}//callback_dumb_increment

//-----------
static struct lws_protocols protocols[] = {
        /* first protocol must always be HTTP handler */
        {
                "dumb-increment-protocol",
                callback_dumb_increment,
                sizeof(struct per_session_data__dumb_increment),
                10,
        },
        { NULL, NULL, 0, 0 } /* terminator */
};//protocols

//**************************
void sighandler(int sig)
{
        force_exit = 1;
        lws_cancel_service(context);
}


//*********************************************************************
int main(int argc, char **argv)
{
  struct lws_context_creation_info info;
  int opts = 0;
  int n = 0;
  unsigned int ms, oldms = 0;   
  
  memset(&info, 0, sizeof info);
  info.port = 9999;
  info.protocols = protocols;
  info.ssl_cert_filepath = NULL;
  info.ssl_private_key_filepath = NULL;
  info.extensions = lws_get_internal_extensions();

  info.gid = -1;
  info.uid = -1;
  info.max_http_header_pool = 1;
  info.options = opts;
    
  signal(SIGINT, sighandler);
  
        context = lws_create_context(&info);
        if (context == NULL) {
                lwsl_err("libwebsocket init failed\n");
                return -1;
        }
  
  n = 0;
        while (n >= 0 && !force_exit) {
                struct timeval tv;

                gettimeofday(&tv, NULL);

                /*
                 * This provokes the LWS_CALLBACK_SERVER_WRITEABLE for every
                 * live websocket connection using the DUMB_INCREMENT protocol,
                 * as soon as it can take more packets (usually immediately)
                 */

                ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
                if ((ms - oldms) > 50) {
                        lws_callback_on_writable_all_protocol(context,
                                &protocols[PROTOCOL_DUMB_INCREMENT]);
                        oldms = ms;
                }

                /*
                 * If libwebsockets sockets are all we care about,
                 * you can use this api which takes care of the poll()
                 * and looping through finding who needed service.
                 *
                 * If no socket needs service, it'll return anyway after
                 * the number of ms in the second argument.
                 */

                n = lws_service(context, 50);
        }//while n>=0
  
  lws_context_destroy(context);
  
  return 0;
  
}//main

