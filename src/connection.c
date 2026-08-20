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
  RECIEVE_MESSAGE = 0x11, //

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
  uint16_t id;
  uint32_t payloadLen;
} packetHeader;

void encode_Packet(packetHeader* header){

}

void receive_Packet(uv_stream_t* client, ssize_t nread, char* buf){ //need to check that char* is what I actually want here 
  //nom nom packet 
  
  //header enuff?
  //decode nom
  //send data
}

void decode_Packet(ssize_t nread, const uv_buf_t* buf){
  if (nread > 0 && buf->len != 0) {
    uint8_t tempType = buf->base[0];
    packetType type = (packetType)tempType;
    switch (type) {
      case RECIEVE_MESSAGE:
        break;
      case CHANNEL_JOIN:
        break;
      case USER_GET:
        break;
      case USER_LEAVE:
        break;
      }
  }

}

int send_Packet(connection, &msg){
}
