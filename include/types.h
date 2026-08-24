#ifndef TYPES
#define TYPES

#include <uv.h>
#include "uthash/src/uthash.h"

//ew portability
#if defined(_WIN32)
  #include <rpc.h>
#elif defined(__APPLE__) || defined(__linux__)
  #include <uuid/uuid.h>
#endif

#define MAX_MSG_LEN 4096

typedef struct{
  char*           name; //the key
  int             default_channel; //a bool 
  UT_hash_handle  hh;
  char**          required_permission; //tbh im not going to use this for a while
} Channel;

typedef struct {
  uv_write_t      req;
  uv_buf_t        buf;
} write_req_t;

typedef struct {
  uuid_t          uuid;
  char*           bio;
  char*           name;
  //any other user-specific information (role?)
} Userinfo;

typedef struct User User;
struct User {
  User*           next;
  User*           last;
  Userinfo        info;
  uv_stream_t*    user_handle; //the key
  char*           channel; //only needed for if we are trying to save bandwidth
  UT_hash_handle  hh;
};
//we are hashing the user handle pointers, this allows us to O(1) lookup information about a user

extern User* userlist;
extern Channel* channellist;
extern void echo_write(uv_write_t* req, int status);

#endif
