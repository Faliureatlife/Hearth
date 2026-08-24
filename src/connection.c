//creating a protocol that establishes information packets
/*
 * User-initiated
 * - Usr_getInfo
 * - Usr_updateInfo
 * - Send_message
 * - Req_newChannel
 *
 *
 * Server-initiated
 * - Broadcast_message
 * - Broadcast_usrUpdate 
 * - Broadcast_newUsr
 * - Broadcast_newChannel
 *
 *
 *  Server will hear packets and place in some sort of holding structure in order to deal with sequentially or dispatch processes to complete
 *
 *  general format 
 *
 *  Header
 *  =======
 *
 *  packetType
 *  packetId (iterative ID to reference for ACK and whatnot)
 *  dataSize
 *
 *
 */
typedef struct {
  uint16_t channelID; //id of where it was sent
  uint16_t length;
  char* text; //message body
} receiveMessage;

typedef struct { //dont need pretty sure
  uint16_t channelID; //id of where it was sent
  uint8_t nameLen;
  char* name;
  uint16_t length;
  char* text; //message body
} sendMessage;

typedef struct {
  uint16_t channelID; //id of new channel 
  uint16_t length; 
  char* text; //name of channel
} newChannel;

typedef struct {
  uint16_t length;
  char* text; //the list of all channels
} listChannel;

typedef struct {
  uint16_t lengthName;
  char* name; 
  uint16_t lengthBio;
  char* bio;
  //future pfp data?
} infoUser;

typedef struct {
  uint16_t lengthName;
  char* name; //whatever is not getting updated will be null or smth
  uint16_t lengthBio;
  char* bio;
} updateUser;

//god only knows what types i am forgetting but this is enough to have an idea at least I hope
//using naming from the server's POV 
enum packetType {
  HELLO = 0x01,

  SEND_MESSAGE    = 0x10,
  RECEIVE_MESSAGE = 0x11, //
  SEND_PIC        = 0x12,

  CHANNEL_NEW     = 0x20, //notify users of new channel
  CHANNEL_LIST    = 0x21, //send list of all channels
  CHANNEL_JOIN    = 0x22, //use if we want to avoid client caching
  CHANNEL_REQ_NEW = 0x23, //usr request new channel

  USER_GET        = 0x30, //request for user information
  USER_UPDATE     = 0x31, //update selected user field
  USER_INFO       = 0x32, //all of a users information
  USER_LEAVE      = 0x33,
};



typedef struct {
  uint8_t type; //need to cast just to be safe
  uint8_t version;
  uint32_t id;
  uint32_t payloadLen;
} packetHeader;

//no clue if this is useful
//pretty sure not 23 Aug 2026
typedef struct {
  packetHeader header;
  uv_stream_t* client;
  const char* data;
} packetInfo;

//for packets that have two len and two data
void encode_Packet(packetHeader* header, const char* data, uv_stream_t* client ){
}

//this will be called by listening, accumulate portions of packets in a buffer and then passes full packets to decode_Packet()
void receive_Packet(uv_stream_t* client, ssize_t nread, uv_buf_t* buf){ //need to check that char* is what I actually want here 
  //accumulate the data into a linked list of buffers that hold packets (shouldnt need more than a few entries at a time)
  //when the data makes up (header + payloadlen) we will make it into a packet and process with decode_Packet which will handle it according to type
  //going to put all the packets into a LL that will contain packetInfo objects 
  //i need to make sure i am freeing the uv_buf_t properly somewhere 
  decode_Packet(packet);
}

//packet gives me header, client*, and data*
void decode_Packet(packetInfo* packet){
  //redundant but its easier to have this "alias"
  packetType type = (packetType)(packetHeader.type);
    
  switch (type) {
    case RECEIVE_MESSAGE:
      //dont need the connection because we are resending it to all users (cheaper than the check)
      broadcast_Message(packet->header, packet->data);
      break;
    case CHANNEL_JOIN:
      break;
    case USER_GET:
      break;
    case USER_LEAVE:
      break;
    case USER_UPDATE:
      break;
    }
}

//uses the pointers from the packet created in receive_Packet, packetInfo->data
//should eventually make alternative that selects for roles
void broadcast_Message(packetHeader info, char* buf){
  // User* currentUsr; HASH_FIND_PTR(userlist, &client, currentUsr);
  //redundant?
  char* name = currentUsr->info.name;
  char* message = (char*)malloc(len); //plus a few probably
  message = snprintf(message, len, "");
}
