#include <string.h>
#include "commandHelpers.h"

void change_name(uv_stream_t* handle, char* alias) {
  User* findusr;

  //check to see if name taken?
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
      newchannel->name = strdup(channelName);
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
  fprintf(stderr, "usr request channel list\n");
  HASH_ITER(hh, channellist, walker, tmp){
    fprintf(stderr, "%s\n",walker->name);
    yell_at_user(handle, walker->name);

  }
}


void rename_channel(char* channelName, char* newName);

void change_channel_perms();  
void add_user_role();
void rm_user_role();

void yell_at_user(uv_stream_t* handle, char* msg){
  User* findusr;
  char* cpymsg = strdup(msg);

  HASH_FIND_PTR(userlist, &handle, findusr);
  fprintf(stdout, "yelling %s at user %s", msg, findusr->info.name);

  //add newline here
  write_req_t* req = (write_req_t*) malloc(sizeof(write_req_t));
  size_t len = strlen(cpymsg);
  req->buf = uv_buf_init(cpymsg,len);
  uv_write((uv_write_t*) req, findusr->user_handle, &req->buf,1,echo_write);
}


// void disseminate(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf){
//   //this is where we do the input validation and processing of the commands etc.
//   if (nread > (ssize_t)MAX_MSG_LEN){
//     //should really be handled preemptively by client
//     fprintf(stderr, "ERR: message too long; %ld and the max is %d\n", nread, MAX_MSG_LEN);
//   }
//
//   //i should just allocate tchar here, just not rn for reasons i cannot deign
//   buf->base[nread] = '\0'; //just in case 
//   if (!strncmp(buf->base,"exit",4)) {
//       uv_close((uv_handle_t*) handle, on_close);
//   } else if (!strncmp(buf->base,"INFO~",5)){
//       //max len juuuust in case
//       char tchar1[MAX_MSG_LEN];
//       char tchar2[MAX_MSG_LEN];
//       uuid_t uuid;
//       sscanf(buf->base, "INFO~%[^~]~%[^~]",tchar1,tchar2);
//       //making sure that uuid is valid (0/false if valid)
//       if (uuid_parse(tchar1, uuid)){
//           fprintf(stderr, "Uh oh, invalid UUID\n");
//       } else {
//           add_user_info(handle, uuid, tchar2/*name*/);
//       }
//   } else if (!strncmp(buf->base, "NAME~",5)){
//       char tchar1[MAX_MSG_LEN];
//
//       sscanf(buf->base, "NAME~%[^~]",tchar1);
//       change_name(handle, tchar1);
//   } else if (!strncmp(buf->base, "CHANNEL~",8)){
//       char tchar1[MAX_MSG_LEN];
//
//       sscanf(buf->base, "CHANNEL~%[^~]~",tchar1);
//       change_channel(handle, tchar1);
//   } else if (!strncmp(buf->base, "NEWCHANNEL~",8)){
//       char tchar1[MAX_MSG_LEN];
//       char tchar2[MAX_MSG_LEN];
//
//       //permissions check goes here
//       sscanf(buf->base, "NEWCHANNEL~%[^~]~%[^~]~",tchar1,tchar2);
//       new_channel(tchar1,atoi(tchar2));
//   } else if (!strncmp(buf->base, "LIST~",5)){
//       //passing handle so we send the info to the right person
//       list_channels(handle);
//   } else {
//       User* currentusr; HASH_FIND_PTR(userlist, &handle, currentusr);
//       char* name = currentusr->info.name;
//       size_t outlen = strlen(currentusr->channel) + strlen(name) + 3 /*'@' + '~' + ':'*/ + nread + 1;
//       uv_buf_t* newbuf = (uv_buf_t*) malloc(sizeof(uv_buf_t));
//       newbuf->base = (char*) malloc(outlen);
//
//       //[channel,username,message]
//       snprintf(newbuf->base, outlen, "@%s~%s:%s",currentusr->channel, name, buf->base);
//       newbuf->len = outlen;
//       scream(newbuf);
//   }
//   free(buf->base);
// }

// void scream(uv_buf_t* buf){
//   fprintf(stdout, "%s", buf->base);
//   User* walker;
//   //we choose to reallocate every time because we dont know when the message will be dequeued
//   //to avoid stalling main thread we give every thread its own mem to write from
//   for (walker = userlist; walker != NULL; walker = (User*)(walker->hh.next)){
//     write_req_t* req = (write_req_t*) malloc(sizeof(write_req_t));
//     char* cpybuf = malloc(buf->len);
//     memcpy(cpybuf, buf->base, buf->len);
//     req->buf = uv_buf_init(cpybuf,buf->len); //we are regenerating cpybuf everytime because &req->buf is freed everytime (not req->buf)
//     uv_write((uv_write_t*) req, walker->user_handle, &req->buf,1,echo_write);
//   }
//   free(buf->base);
//   free(buf);
// }
