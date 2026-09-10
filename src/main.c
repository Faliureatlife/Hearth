#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#include "admin.h"
#include "init.h"
#include "commandHelpers.h"
#include "types.h"
#include "connection.h"


#define DEFAULT_PORT 7000//choose something better when not testing
#define BACKLOG 128
// #define MAX_MSG_LEN 4096

uv_signal_t sigint;
User* userlist = NULL;
Channel* channellist = NULL;
inProgress* packetlist = NULL;

//THE GLOBAL VARIABLES THAT WE DO NEED(well not need but want, i already wrote the code)
uv_loop_t* loop;
struct sockaddr_in addr;


void free_write_req(uv_write_t* req){
  write_req_t* wr = (write_req_t*) req;
  free(wr->buf.base);
  free(wr);
}

//used as default read allocation, suggested_size is 65536 typically
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf){
  buf->base = malloc(suggested_size + 1);
  buf->len = suggested_size;
}

//MAYBE: SPAWN THREAD TO DO THIS INSTEAD
void rm_user(uv_stream_t* handle){
  User* searcher;
  HASH_FIND_PTR(userlist, &handle, searcher);
  if (searcher != NULL){
    HASH_DEL(userlist, searcher);
    free(searcher->info.name);
    free(searcher->channel);
    free(searcher);
    return;
  }
}

void die(uv_signal_t* handle, int sig_num){
  User* walker, *tmp;
  HASH_ITER(hh, userlist, walker, tmp) {
    rm_user(walker->user_handle);
  }
  write_channels_to_file("channels.mml");
  uv_signal_stop(handle);
  uv_stop(loop);
}

void on_close(uv_handle_t* handle){
  rm_user((uv_stream_t*)handle); //uv_stream_t
  free(handle);
}

// void echo_write(uv_write_t* req, int status){
//   if (status) fprintf(stderr, "Write error %s\n",uv_strerror(status));
//   // free_write_req(req);
//   write_req_t* wr = (write_req_t*) req;
//   free(wr->buf.base);
//   free(req);
// }

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf){
  if (nread > 0) {
    write_req_t* req = (write_req_t*) malloc(sizeof(write_req_t));
    req->buf = uv_buf_init(buf->base, nread);
    uv_write((uv_write_t*) req, client, &req->buf, 1, echo_write);
    return;
  }
  if (nread < 0){
    if (nread != UV_EOF)
      fprintf(stderr, "Read error %s\n",uv_strerror(nread));

    uv_close((uv_handle_t*) client, on_close);
  }
  free(buf->base);
}

//not important to be const, it is just being asserted by the cb definition 
void listening(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf){
  //check to make sure valid message (read and in buffer)
  if (nread > 0 && buf->len != 0){
    //go on to send the message out
    receive_Packet(client, nread, buf);
    return;
  }
  //error case
  if (nread < 0){
    uv_close((uv_handle_t*) client, on_close);
    return;
  }
  free(buf->base);
}

/*
 * The process:
 * New connection is made
 * Client sends an automated message that introduces themself
 * Send UUID, name, and any other acc info
 */
void on_new_connection(uv_stream_t *server, int status){
  if (status < 0){
    fprintf(stderr, "Error on connection: %s\n", uv_strerror(status));
    return;
  }

  uv_tcp_t* client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  //accept the connecion, handle initialized, server + client must be same loop
  if (uv_accept(server, (uv_stream_t*) client) == 0){
    fprintf(stderr, "newusr\n");
    add_user(client);
    // fprintf(stderr, "A user is already active under this UUID\n If this causes issues to you then yell at the dev");

    //read from client, to the allocation callback (arg 2) and uses the function in (arg 3)
    uv_read_start((uv_stream_t*) client, alloc_buffer, listening);
  }
}



/* TODO:
 * --------------------
 *  Server
 *
 * - User ID
 * - Basic commands
 *
 * - Channels this rn tbh just add the channel to the beginning of the string
 *   thix iz actually client tbh
 *
 * - Admin commands
 * - User profile storage
 * - Buffered Chat history
 * - User Roles
 *
 * ------------
 * Client
 *
 * - Give Profile name
 * - Text Customization
 * - Test replacement command
 * - Emotes
 * - Role permissions
 * - Connect to specified server
 */
int main(int argc, char* argv[]){
  //messages formatted as MarkDown, add LaTeX support later
  loop = uv_default_loop();
  uv_tcp_t server;

  uv_tcp_init(loop, &server);
  //server created with given loop
  uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);
  //bound to IP addr + port
  // uv_ip6_addr("0.0.0.0", DEFAULT_PORT + 1,)

  //bind the server to the address(it now 'exists')
  uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

  //listen at the socket given, using server casted to generic stream, with given acceptable backlog
  //what is done upon connection is determined by on_new_connection
  //returns non-0 if errored
  int r = uv_listen((uv_stream_t*) &server, BACKLOG, on_new_connection);
  if (r) {
    fprintf(stderr, "Listen error %s\n",uv_strerror(r));
    return 1;
  }
  
  read_channels_from_file("channels.mml");
  uv_signal_init(loop, &sigint);
  uv_signal_start(&sigint, die, SIGINT);
  printf("Listening on %d\n",DEFAULT_PORT);
  return uv_run(loop, UV_RUN_DEFAULT); 
}
  
