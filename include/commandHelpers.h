#ifndef commandHelpers
#define commandHelpers

#include "types.h"

void change_name(uv_stream_t* handle, char* alias);

void change_channel(uv_stream_t* handle, char* channel);

void add_user_info(uv_stream_t* handle, uuid_t uuid, char* alias);

void new_channel(char* channelName, int def /*, char** req_permission */);

void rm_channel(char* channelName);

void rename_channel(char* channelName, char* newName);

void list_channels(uv_stream_t* handle);

void change_channel_perms();
void add_user_role();
void rm_user_role();

void yell_at_user(uv_stream_t* handle, char* msg);

#endif // !commandHelpers
