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
enum packetType {
  HELLO = 0x01,

  SEND_MESSAGE = 0x10,
  RECIEVE_MESSAGE = 0x11, //

  CHANNEL_NEW = 0x20, //notify users of new channel
  CHANNEL_LIST = 0x21, //send list of all channels
  CHANNEL_JOIN = 0x22, //use if we want to avoid client caching

  USER_GET = 0x30,
  USER_UPDATE = 0x31, //update selected user field
  USER_INFO = 0x32, //all of a users information
  USER_LEAVE = 0x33,
};



typedef struct {
  uint8_t version;
  uint32_t id;
  uint8_t type; //need to cast just to be safe
  uint32_t payloadLen;
} packetHeader;

void encode_Packet(){

}

void decode_Packet(){

}

int send_Packet(connection, &msg){
}
