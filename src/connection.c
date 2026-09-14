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
        free(currentData->rawData);
        // free(currentData->partHeader)
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
  goto end;
  finishedPacket:
  decode_Packet(packet);
  end:
}

//packet gives me header, client*, and data*
void decode_Packet(packetInfo* packet){
  //redundant but its easier to have this "alias"
  enum packetType type = (enum packetType)packet->header->type;
  User* currentUsr; HASH_FIND_PTR(userlist, &packet->client, currentUsr);
  if (currentUsr != NULL) {
  //i _think_ it makes sense to have it here unless its only neeeded for resending messages
  char* name = currentUsr->info.name;

  switch (type) {
    case RECEIVE_MESSAGE: //we redistrubute to everyone (possibly even the sender to keep input messages seperate, one-source of truth?)
      //dont need the connection because we are resending it to all users (cheaper than the check)
      broadcast_Message(packet->header, name, packet->data);
      break;
    case CHANNEL_JOIN:
      //update the channel that the usr is registered to.
      //optionally send ack
      break;
    case CHANNEL_LIST:
      //packet containing all of the information
      listChannel chan; 
      listChannel.text = malloc(sizeof(char) * 4096);// add in resize fixing later for massive things
      Channel* walker;
      for (walker = userlist; walker != NULL; walker = (User*)(walker->hh.next)){
        memcpy(chan.text, walker->name, walker->namelen);

      }
      break;
    case CHANNEL_NEW:
      //packet saying the name of new channel
      break;
    case CHANNEL_RENAME:
      //packet confirming the name
      break;
    case USER_GET:
      //getting info on a specific usr
      //send the info packet
      break;
    case USER_LEAVE:
      //remove user from pool
      //no responce needed
      break;
    case USER_UPDATE:
      //change info on user
      //maybe just USER_NEWNAME, USER_NEWBIO, USER_NEWPERM
      //send copy of the userdata? or maybe ack
      break;
    default:
      fprintf(stderr, "whoops I haven't implemented that packet type yet\n");
    }
  }
}

void echo_write (uv_write_t* req, int staus){
  writeReq* wr = (writeReq*)req;
  (*wr->refs)--;
  if (*wr->refs == 0){
    free(wr->data);
    free(wr->refs);
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
  if(!fullsize){return;}//agh crash
  char* message = (char*)malloc(fullsize);
  //its snprintf in case i DO want to format
  memcpy(message, name, strlen(name));
  memcpy(message + strlen(name), buf, info->payloadLen);

  User* walker;

  for (walker = userlist; walker != NULL; walker = (User*)(walker->hh.next)){
    writeReq* req = (writeReq*)malloc(sizeof(writeReq));
    req->buf = uv_buf_init(message, fullsize);
    req->data = message;
    req->refs = refs;
    (*refs)++;

    uv_write((uv_write_t* )&req->req, walker->user_handle, &req->buf,1,echo_write);
  }
}
