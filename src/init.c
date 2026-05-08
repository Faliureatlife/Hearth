#include "init.h"

//check to make sure the handle is unique with HASH_FIND
void add_user(uv_tcp_t* handle){
  //to check for existence
  User* newusr;
  //check for existence already (should be impossible)
  HASH_FIND_PTR(userlist, &handle, newusr);
  if (newusr == NULL){
      newusr = (User*) malloc(sizeof(User));
      newusr->user_handle = (uv_stream_t*) handle;
      HASH_ADD_PTR(userlist, user_handle, newusr);
  }
}

