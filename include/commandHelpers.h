#ifndef commandHelpers
#define commandHelpers

#include "types.h"

void change_name(uv_stream_t* handle, char* alias);
void change_channel(uv_stream_t* handle, char* channel);
void add_user_info(uv_stream_t* handle, uuid_t uuid, char* alias);

void new_channel();

void rm_channel();

void rename_channel();

void change_channel_perms();

void add_user_role();

void rm_user_role();
#endif // !commandHelpers
