#include <string.h>
#include "commandHelpers.h"

void change_name(uv_stream_t* handle, char* alias) {
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  if (findusr != NULL){
      if (strlen(alias) > 256) { //theres no reason to have that long a name
          yell_at_user(handle, "Choose a shorter name (255 characters or less)");
      }
      // findusr->info.name = realloc(findusr->info.name, strlen(alias));
      strcpy(findusr->info.name, alias);
  }
}

void change_channel(uv_stream_t* handle, char* channel) {
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  if (findusr != NULL){
      //in theory we should validate but in reality idc
      strcpy(findusr->channel, channel);
  }
}

void add_user_info(uv_stream_t* handle, uuid_t uuid, char* alias) {
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  if (findusr != NULL){
      uuid_copy(findusr->info.uuid, uuid);
      change_name(handle, alias);
      //idk strings are scary
      return;
  }
}

void set_default(char* channelName){
  Channel* finddefault;
  Channel* newdefault;

  int def = 1;
  HASH_FIND_INT(channellist, &def, finddefault);
  HASH_FIND_STR(channellist, channelName, newdefault);
  if ((finddefault != NULL) && (newdefault != NULL)){
      finddefault->default_channel = 0;
      newdefault->default_channel = 1;
  } else if (newdefault != NULL){
      newdefault->default_channel = 1;
  } else {
    //fail case
  }
}

//channelname should never include a ,
void new_channel(char* channelName, int def /*, char** req_permission */){
  Channel* newchannel; 
  int nameLen = strlen(channelName);
  // fprintf(stderr, "%d\n",nameLen);
  // HASH_FIND(hh, channellist, &channelName, strlen(channelName),newchannel); 	//maybe if the other doesnt work
  HASH_FIND_STR(channellist, channelName, newchannel);
  if (newchannel == NULL && nameLen < 256){
      newchannel = (Channel*)malloc(sizeof(Channel));
      newchannel->name = channelName;
      //this is where I say that we need to add in the permissions stuff
      HASH_ADD_PTR(channellist, name, newchannel);
      // HASH_ADD_KEYPTR(hh, channellist, &channelName, nameLen, newchannel);
      if (def != 0) {
          set_default(channelName);
      }
  }
}

void rm_channel(char* channelName){
  Channel* findchannel;
  HASH_FIND_STR(channellist, channelName, findchannel);
  if (findchannel != NULL){
      if (findchannel->default_channel != 0){
          HASH_DEL(channellist, findchannel);
          Channel* tmp, *tmptwo;
          HASH_ITER(hh, channellist, tmp, tmptwo);
          tmp->default_channel = 1;
          return;
      }
      HASH_DEL(channellist, findchannel);
  }
}

void list_channels(uv_stream_t* handle){
  Channel* walker, *tmp;
  HASH_ITER(hh, channellist, walker, tmp){
    yell_at_user(handle, walker->name);
  }
}
// void die(uv_signal_t* handle, int sig_num){
//   User* walker, *tmp;
//   HASH_ITER(hh, userlist, walker, tmp) {
//     rm_user(walker->user_handle);
//   }


void rename_channel(char* channelName, char* newName);

void change_channel_perms();  
void add_user_role();
void rm_user_role();

void yell_at_user(uv_stream_t* handle, char* msg){
  User* findusr;

  HASH_FIND_PTR(userlist, &handle, findusr);
  fprintf(stdout, "yelling %s at user %s", msg, findusr->info.name);

  write_req_t* req = (write_req_t*) malloc(sizeof(write_req_t));
  size_t len = strlen(msg);
  req->buf = uv_buf_init(msg,len);
  uv_write((uv_write_t*) req, findusr->user_handle, &req->buf,1,echo_write);
}
