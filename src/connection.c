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


#include "types.h"
#include "connection.h"
// typedef struct {
//   uint16_t channelID; //id of where it was sent
//   uint16_t length;
//   char* text; //message body
// } receiveMessage;
//
// typedef struct { //dont need pretty sure
//   uint16_t channelID; //id of where it was sent
//   uint8_t nameLen;
//   char* name;
//   uint16_t length;
//   char* text; //message body
// } sendMessage;
//
// typedef struct {
//   uint16_t channelID; //id of new channel 
//   uint16_t length; 
//   char* text; //name of channel
// } newChannel;
//
// typedef struct {
//   uint16_t length;
//   char* text; //the list of all channels
// } listChannel;
//
// typedef struct {
//   uint16_t lengthName;
//   char* name; 
//   uint16_t lengthBio;
//   char* bio;
//   //future pfp data?
// } infoUser;
//
// typedef struct {
//   uint16_t lengthName;
//   char* name; //whatever is not getting updated will be null or smth
//   uint16_t lengthBio;
//   char* bio;
// } updateUser;
//
// //god only knows what types i am forgetting but this is enough to have an idea at least I hope
// //using naming from the server's POV 
// enum packetType {
//   HELLO = 0x01,
//
//   SEND_MESSAGE    = 0x10,
//   RECEIVE_MESSAGE = 0x11, //
//   SEND_PIC        = 0x12,
//
//   CHANNEL_NEW     = 0x20, //notify users of new channel
//   CHANNEL_LIST    = 0x21, //send list of all channels
//   CHANNEL_JOIN    = 0x22, //use if we want to avoid client caching
//   CHANNEL_REQ_NEW = 0x23, //usr request new channel
//
//   USER_GET        = 0x30, //request for user information
//   USER_UPDATE     = 0x31, //update selected user field
//   USER_INFO       = 0x32, //all of a users information
//   USER_LEAVE      = 0x33,
// };
//
//
// typedef struct {
//   uint8_t type; //need to cast just to be safe
//   uint8_t version;
//   uint32_t id;
//   uint32_t payloadLen;
// } packetHeader;
//
// //no clue if this is useful
// //pretty sure not 23 Aug 2026
// typedef struct {
//   packetHeader* header;
//   uv_stream_t* client;
//   const char* data;
// } packetInfo;
//
// typedef struct{
//   packetHeader*   partHeader;
//   uint32_t        rawSize;
//   char*           rawData;         //this is the data that we work with
//   uv_stream_t*    client;
//   UT_hash_handle  hh;
// } inProgress;

//for packets that have two len and two data
void encode_Packet(packetHeader* header, const char* data, uv_stream_t* client ){
}

//this will be called by listening, accumulate portions of packets in a buffer and then passes full packets to decode_Packet()
void receive_Packet(uv_stream_t* client, ssize_t nread, uv_buf_t* buf){ //need to check that char* is what I actually want here 
  //accumulate the data into a linked list of buffers that hold packets (shouldnt need more than a few entries at a time)
  //when the data makes up (header + payloadlen) we will make it into a packet and process with decode_Packet which will handle it according to type
  //going to put all the packets into a LL that will contain packetInfo objects 
  //i need to make sure i am freeing the uv_buf_t properly somewhere 
   
  if (nread < 0) {return;} //TEMP ERROR HANDLING
  packetInfo* packet; 
  inProgress* currentData = NULL;
  HASH_FIND_PTR(packetlist, &client, currentData);


  if (currentData != NULL){ readdata:
    char* tempbuf = (char*)malloc(1024*64);
    memcpy(tempbuf, currentData->rawData, currentData->rawSize);
    memcpy(tempbuf + currentData->rawSize, buf->base, nread);
    int readdata = 0;
    int leftoverdata = nread + currentData->rawSize;

    //everything is allocated, just have to see what we have and add it 
    if (currentData->partHeader->type == 0) {goto Ype;}
    else if (currentData->partHeader->version == 0) {goto Ver;}
    else if (currentData->partHeader->id == 0) {goto Di;}
    else if (currentData->partHeader->payloadLen == 0) {goto Len;}
    else {goto Payload;}
    
    Ype:
      if (leftoverdata >= sizeof(uint8_t)) {//sizeof for portability?
        //moving the data in char 1
        currentData->partHeader->type = tempbuf[0 + readdata];
        leftoverdata = leftoverdata - sizeof(uint8_t);
        readdata = readdata + sizeof(uint8_t);
      } else goto Done;
    Ver:
      if(leftoverdata >= sizeof(uint8_t)){
        currentData->partHeader->version = tempbuf[0 + readdata];
        leftoverdata = leftoverdata - sizeof(uint8_t);
        readdata = readdata + sizeof(uint8_t);
      } else goto Done;
    Di:
      if(leftoverdata >= sizeof(uint32_t)){
        //heard this is better than memcpy for explicit ordering
        currentData->partHeader->id = 
          tempbuf[readdata] << 24 | tempbuf[readdata + 1] << 16 |
          tempbuf[readdata+2] << 8 | tempbuf[readdata + 3];
        leftoverdata = leftoverdata - sizeof(uint32_t);
        readdata = readdata + sizeof(uint32_t);
      } else goto Done;
    Len:
      if(leftoverdata >= sizeof(uint32_t)){
        currentData->partHeader->payloadLen = 
          tempbuf[readdata] << 24 | tempbuf[readdata + 1] << 16 |
          tempbuf[readdata+2] << 8 | tempbuf[readdata + 3];
        leftoverdata = leftoverdata - sizeof(uint32_t);
        readdata = readdata + sizeof(uint32_t);
      } else goto Done;
    Payload:
      if (leftoverdata == currentData->partHeader->payloadLen){
        char* tchar = (char*)malloc(currentData->partHeader->payloadLen);
        memcpy(tchar, tempbuf + readdata, currentData->partHeader->payloadLen);
        packet = (packetInfo*)malloc(sizeof(packetInfo));
        packet->header = currentData->partHeader;
        packet->client = client;
        packet->data = tchar;
        //check to see if i have to delete anything else
        HASH_DEL(packetlist,currentData);
        free(tempbuf);
        goto finishedPacket;
      }

    Done:
      memcpy(currentData->rawData, tempbuf + readdata, leftoverdata);
      currentData->rawSize = leftoverdata;
      free(tempbuf);

  } else {    
    //create the header
    packetHeader* processHeader = (packetHeader*)malloc(sizeof(packetHeader));
    processHeader->type = 0;
    processHeader->version = 0;
    processHeader->id = 0;
    processHeader->payloadLen = 0;

    //create the inProgress struct
    currentData = (inProgress*)malloc(sizeof(inProgress)); 
    currentData->partHeader = processHeader;
    currentData->rawSize = 0;
    currentData->rawData = (char*)malloc(1024*32);
    currentData->client = client;
    HASH_ADD_PTR(packetlist, client, currentData);

    goto readdata;
  }
  finishedPacket:
  decode_Packet(packet);
}

//packet gives me header, client*, and data*
void decode_Packet(packetInfo* packet){
  //redundant but its easier to have this "alias"
  enum packetType type = (enum packetType)packet->header->type;
  User* currentUsr; HASH_FIND_PTR(userlist, packet->client, currentUsr);
  //i _think_ it makes sense to have it here unless its only neeeded for resending messages
  char* name = currentUsr->info.name;

  switch (type) {
    case RECEIVE_MESSAGE: //we redistrubute to everyone (possibly even the sender to keep input messages seperate, one-source of truth?)
      //dont need the connection because we are resending it to all users (cheaper than the check)
      broadcast_Message(packet->header, name, packet->data);
      break;
    case CHANNEL_JOIN:
      break;
    case USER_GET:
      break;
    case USER_LEAVE:
      break;
    case USER_UPDATE:
      break;
    default:
      fprintf(stderr, "whoops I haven't implemented that packet type yet\n");
    }
}

void echo_write (uv_write_t* req, int staus){
  writeReq* wr = (writeReq*)req;
  (*wr->refs)--;
  if (msg->refs == 0){
    free(wr->data);
    free(we->refs);
  }
  free(wr);
//idk yet
}
//uses the pointers from the packet created in receive_Packet, packetInfo->data
//should eventually make alternative that selects for roles
void broadcast_Message(packetHeader* info, const char* name, const char* buf){
  // User* currentUsr; HASH_FIND_PTR(userlist, &client, currentUsr);
  //redundant?
  uint32_t* refs = (uint32_t*)malloc(sizeof(uint32_t));
  *refs = 0;
  uint32_t fullsize = info->payloadLen + strlen(name);
  char* message = (char*)malloc(fullsize);
  //its snprintf in case i DO want to format
  memcpy(message, name, strlen(name));
  memcpy(message + strlen(name), buf, info->payloadLen);

  User* walker;

  for (walker = userlist; walker != NULL; walker = (User*)(walker->hh.next)){
    writeReq* req = (writeReq*)malloc(sizeof(writeReq));
    req->buf = uv_buf_init(message, fullsize);
    req->data = message
    req->refs = refs;
    (*refs)++;

    uv_write((uv_write_t* )&req->req, walker->user_handle, &req->buf,1,echo_write);
  }
}
