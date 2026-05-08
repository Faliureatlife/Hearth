#include <string.h>
#include "commandHelpers.h"

void change_name(uv_stream_t* handle, char* alias) {
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  if (findusr != NULL){
      strcpy(findusr->info.name, alias);
  }
}

void change_channel(uv_stream_t* handle, char* channel) {
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  if (findusr != NULL){
      strcpy(findusr->channel, channel);
  }
}

void add_user_info(uv_stream_t* handle, uuid_t uuid, char* alias) {
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  if (findusr != NULL){
      uuid_copy(findusr->info.uuid, uuid);
      strcpy(findusr->info.name, alias);
      //idk strings are scary
      return;
  }
}

void new_channel();

void rm_channel();

void rename_channel();

void change_channel_perms();

void add_user_role();

void rm_user_role();

