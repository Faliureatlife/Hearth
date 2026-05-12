#include "init.h"
#include "types.h"

//check to make sure the handle is unique with HASH_FIND
void add_user(uv_tcp_t* handle){
  //to check for existence
  User* newusr;
  //check for existence already (should be impossible)
  HASH_FIND_PTR(userlist, &handle, newusr);
  if (newusr == NULL){
      newusr = (User*) malloc(sizeof(User));
      newusr->user_handle = (uv_stream_t*) handle;
      // newusr->channel = "GenPop"; //its a placeholder okey
      newusr->channel = (char*) malloc(sizeof(char) * 256); 
      strcpy(newusr->channel, "GenPop");
      newusr->info.name = (char*) malloc(sizeof(char) * 256); 
      newusr->info.name[0] = '\0'; //just in case
      HASH_ADD_PTR(userlist, user_handle, newusr);
  }
}

void read_channels_from_file(char* filename){
  //idea only 
  int depth = 0; //depth of the current parse
  //loop right amt of times
  fscanf(outfile, "{\n name=%s\n default=%d\n perms={\n }}\n"/* VARIABLES */); //i havent actually written in the perms yet
}

/*
 * {
 *   name=               //string
 *   default=            //bool
 *   perms{
 *                     //string
 *                     //string
 *   }
 * }
 */ 
void write_channels_to_file(char* filename){
  FILE* outfile = fopen(filename, "w");
  Channel* walker, *tmp;
  HASH_ITER(hh, channellist, walker, tmp){ //check to make sure that this method of write actually works
    fprintf(outfile, "{\n name=%s\n default=%d\n perms={\n }}\n",walker->name,walker->default_channel); //i havent actually written in the perms yet
    // fwrite(strcat(walker->name, ","), strlen(walker->name) + 1, 1, outfile);
  }
}
