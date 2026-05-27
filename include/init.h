#ifndef init
#define init
#include "types.h"
//check to make sure the handle is unique with HASH_FIND
void add_user(uv_tcp_t* handle);
void read_channels_from_file(char* filename);
void write_channels_to_file(char* filename);

#endif // !init
