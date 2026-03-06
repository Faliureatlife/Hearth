#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define DEFAULT_PORT 7000
#define BACKLOG 128
#define MAX_MSG_LEN 4096
uv_loop_t* loop;
struct sockaddr_in addr;

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

typedef struct User User;
struct User {
  User* next;
  User* last;
  uv_stream_t* user_handle;
} ;

User* userlist;
User* latestusr;

void free_write_req(uv_write_t* req){
  write_req_t* wr = (write_req_t*) req;
  free(wr->buf.base);
  free(wr);
}

//used as default read allocation, suggested_size is 65536 typically
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf){
  buf->base = (char*) malloc(suggested_size);
  buf->len = suggested_size;
}

void timer_callback(uv_timer_t* handle){
  printf("timer time\n");
}

void on_close(uv_handle_t* handle){
  free(handle);
}

void echo_write(uv_write_t* req, int status){
  if (status) fprintf(stderr, "Write error %s\n",uv_strerror(status));
  // free_write_req(req);
  // no free shared buffer
  free(req);
}

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

void scream(write_req_t* req){
  fprintf(stderr, "screaming rn \n");
  User* walker = userlist->next;
  while(walker != NULL){
    write_req_t* newreq = (write_req_t*) malloc(sizeof(write_req_t));
    newreq->buf = uv_buf_init(req->buf.base, req->buf.len);
    uv_write((uv_write_t*) newreq, walker->user_handle, &req->buf,1,echo_write);
    walker = walker->next;
  }
}

void disseminate(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf){
  //this is where we do the input validation and processing of the commands etc.
  write_req_t* req = (write_req_t*) malloc(sizeof(write_req_t));
  req->buf = uv_buf_init(buf->base, nread);
  if (nread > MAX_MSG_LEN){
    sprintf(buf->base, "ERR: message too long; %d and the max is %d\n", nread, MAX_MSG_LEN);
  }

  //checking the non-user portion
  {
    // char* exitcheck = (char*)calloc(1, MAX_MSG_LEN*sizeof(char));
    
    //repeat until all are parsed 
    // for(;;)
    char* commands[];
    char* tbuf = malloc(sizeof(char)*MAX_MSG_LEN);
    sscanf(req->buf.base, "%s~",tbuf);
    commands = (char*)malloc(sizeof(char) * (strlen(tbuf)+1));
    int n = 0;
    char* ttbuf = malloc(sizeof(char)*MAX_MSG_LEN);

    while (sscanf(tbuf, "%[^:]:%s",commands[n],ttbuf) == 2){
      n += 1;
      strncpy(tbuf, ttbuf, MAX_MSG_LEN); 
    }
    if (tbuf[0] != '\0'){
      strncpy (commands[n++], tbuf, MAX_MSG_LEN);
    }
    
    /* HERE WE HAVE A COMMANDS ARRAY*/
    /* HERE WE HAVE A COMMANDS ARRAY*/
    /* HERE WE HAVE A COMMANDS ARRAY*/

    //sscanf for parsing if needed 
    //read commands, delimited by `
    sscanf(, "%s`%s",);
    fprintf(stderr, "exitcheck is %s\n",exitcheck);
    //check to see if the beginning of the string is exit
    if (!strcmp("exit",exitcheck)){
      uv_close((uv_handle_t*) handle, on_close);
      free(exitcheck);
      free(buf->base);
      return;
    }
    free(exitcheck);
  }
  
  scream(req);
  free(buf->base);
}

void listening(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf){
  //check to make sure valid message (read and in buffer)
  if (nread > 0 && buf->len != 0){
    //go on to send the message out
    disseminate(client,nread,buf);
    return;
  }
  //error case
  if (nread < 0){
    uv_close((uv_handle_t*) client, on_close);
    return;
  }
  free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status){
  if (status < 0){
    fprintf(stderr, "Error on connection: %s\n", uv_strerror(status));
    return;
  }
  uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);
  //accept the connecion, handle initialized, server + client must be same loop
  if (uv_accept(server, (uv_stream_t*) client) == 0){
    fprintf(stderr, "newusr\n");
    add_user(client);

    //read from client, to the allocation callback (arg 2) and uses the function in (arg 3)
    uv_read_start((uv_stream_t*) client, alloc_buffer, listening);
  }
}

void add_user(uv_tcp_t* handle){
  User* newusr = (User*) malloc(sizeof(User));
  latestusr->next = newusr;
  newusr->last = latestusr;
  newusr->user_handle = (uv_stream_t*) handle;
  newusr->next = NULL;
  latestusr = newusr;
}

/* TODO:
 * --------------------
 *  Server
 *
 * - User ID
 * - Basic commands
 * - Channels
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
  //this is where we will enter into the server
  //need to create IP binds, and general logic
  //will begin with just an echo
  //messages formatted as MarkDown, add LaTeX support later
  //format: Commands [0..n] | Message | \r\n 
  // printf("libuv version %s\n",uv_version_string());
  loop = uv_default_loop();
  uv_tcp_t server;

  uv_tcp_init(loop, &server);
  //server created with given loop
  uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);
  //bound to IP addr + port
  // uv_ip6_addr("0.0.0.0", DEFAULT_PORT + 1,)

  //test timer
  uv_timer_t timer;
  uv_timer_init(loop, &timer);
  uv_timer_start(&timer, timer_callback, 1000, 0);

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


  userlist = (User*) malloc(sizeof(User));
  userlist->next = NULL; 
  userlist->last = NULL;
  latestusr = userlist;
  


  printf("Listening on %d\n",DEFAULT_PORT);
  return uv_run(loop, UV_RUN_DEFAULT); 
}
  
