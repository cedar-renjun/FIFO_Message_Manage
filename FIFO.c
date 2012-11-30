#include <string.h>
#include "FIFO.h"

//Buffer
static char FIFOBuf[BUF_SIZE] = {0};

//Status code
static uint32 Status = OK;

//Message Count
static uint32 MsgCnt = 0;

//Char Count
static uint32 ByteCnt = 0;

//Read Index
static uint32 ReadIndex = 0;

//Write Index
static uint32 WriteIndex = 0;

//example: your message is storage in FIFO like this
//     
//     BEGIN_FLAG   your message  END_FLAG
//     -----------------------------------
//     **BEGIN**    Hello world   **END**
//

//Message Storage Frame Header Flag
static char MsgFrameHeader[] = {"**BEGIN**"};
static uint32 MsgFrameHeaderLen = sizeof(MsgFrameHeader)-1;

//Message Storage Frame Tail Flag
static char MsgFrameTail[] = {"**END**"};
static uint32 MsgFrameTailLen = sizeof(MsgFrameTail)-1;

/*--------------------------------------------------------------------------*-
 *                          User API 
-*--------------------------------------------------------------------------*/

void FIFOInit(void);
void FIFOFlush(void);
uint32 FIFOMsgNum(void);
StatusCode FIFOStatus(void);
StatusCode FIFOMsgGet(char * RecBuf, uint32 Size);
StatusCode FIFOMsgStart(void);
StatusCode FIFOMsgBytePut(char ch);
StatusCode FIFOMsgEnd(void);



/*--------------------------------------------------------------------------*-
 *                          Private Function
-*--------------------------------------------------------------------------*/

//Get the free room of the FIFO
static uint32 FIFOByteAvailable(void)
{
    return (BUF_SIZE - ByteCnt);
}

static int32 StrFind(char * str, uint32 len)
{
   uint32 _ByteCnt   = ByteCnt;
   uint32 _ReadIndex = ReadIndex;
   char * SubIndex   = str;
   
   if(_ByteCnt < len)
   {
       return (-1);
   }

   while(_ByteCnt != 0)
   {        
        while((0 != _ByteCnt) && (FIFOBuf[_ReadIndex] != *SubIndex))
        {
            _ByteCnt -= 1;
            _ReadIndex = (_ReadIndex + 1) % BUF_SIZE;
        }

        //Can not find the str in FIFO
        if(0 == _ByteCnt)
        {
            return (-1);
        }
        else
        {            
            uint32 IndexTmp = _ReadIndex;
            uint32 CntTmp   = _ByteCnt;
            do
            {
                CntTmp -= 1;
                IndexTmp = (IndexTmp + 1) % BUF_SIZE;
                SubIndex += 1;
            }
            while((0 != CntTmp) && (FIFOBuf[IndexTmp] == *SubIndex));
            
            //match it
            if((SubIndex - str) == len)
            {
                return (_ReadIndex);
            }
            else  //not match, restore string index
            {
                _ByteCnt -= 1;
                _ReadIndex = (_ReadIndex + 1) % BUF_SIZE;
                SubIndex = str;
            }
        }        
   }

   //Find the String
   return (_ReadIndex);
}

//Init the FIFO, this function must be called before any other function
void FIFOInit(void)
{
    //Init Status code
    Status = OK;

    //Init Message Count
    MsgCnt = 0;

    //Init Char Count
    ByteCnt = 0;

    //Init Read Index
    ReadIndex = 0;

    //Init Write Index
    WriteIndex = 0;
}

//Flush the FIFO
void FIFOFlush(void)
{
    //Fill the Buffer with '\0'
    memset(FIFOBuf, '\0', BUF_SIZE);
    
    FIFOInit();
}

//Get the number of message stored in FIFO
uint32 FIFOMsgNum(void)
{
    return (MsgCnt);
}

//Show FIFO status
//return code:
//  OK----------> FIFO work correctly
//  OVERFLOW----> FIFO overflow,may be cause by
//                receice too much information
StatusCode FIFOStatus(void)
{
    return (Status);
}

//Put a char into FIFO message
//return code:
//OK-------------> successful put a char into Ringbuf
//FULL-----------> Ringbuf is full, can not put char any more
StatusCode FIFOMsgBytePut(char ch)
{
    if(FIFOByteAvailable() <= MsgFrameTailLen)  //Buffer is full ?
    {
        Status = FULL;
        return (FULL);
    }
    else
    {
        FIFOBuf[WriteIndex] = ch;
        WriteIndex = (WriteIndex + 1) % BUF_SIZE;
        ByteCnt += 1;
        Status = OK;
        return (OK);
    }
}

//Get out of a message from the FIFO
//return code:
//  OK ---------> Successful
//  NOCOTENT----> No message
//  BUFSMALL----> User Receive Buf is small
StatusCode FIFOMsgGet(char * RecBuf, uint32 Size)
{    
    uint32 MsgLen = 0;
    int32  MsgEndIndex = 0;
    //char * MsgEnd = NULL;

    
    //Is there a message in FIFO?
    if(0 == MsgCnt)
    {
        return (NOMSG);
    }

    //Find the Message
    MsgEndIndex = StrFind(MsgFrameTail, MsgFrameTailLen);
    if(MsgEndIndex == -1)
    {
        return (NOMSG);
    }

    //message frame tail is the same direction ?
    //like this mode:
    //FrameHeader content FrameTail
    //---------------------------->
    if(MsgEndIndex > ReadIndex) 
    {       

        //caluate the message content length
        MsgLen = MsgEndIndex - ReadIndex - MsgFrameHeaderLen;

        //User receive buf is enough for message content?
        if(Size < MsgLen)
        {
            //User buf is too small
            return (BUFSMALL);
        }

        //Skip message frame header
        ReadIndex += MsgFrameHeaderLen;
        ReadIndex %= BUF_SIZE;
        ByteCnt -= MsgFrameHeaderLen;
        
        //Get the message content
        memcpy(RecBuf, &FIFOBuf[ReadIndex], MsgLen);

        //Skip the message
        ReadIndex += MsgLen;
        ReadIndex %= BUF_SIZE;
        
        ByteCnt   -= MsgLen;
        
        //Skip the message frame tail
        ReadIndex += MsgFrameTailLen;
        ReadIndex %= BUF_SIZE;
        
        ByteCnt   -= MsgFrameTailLen;

        //Update the message count
        MsgCnt -= 1;
        
        return (OK);
    }
    
    //message frame tail is in the other direction
    //like this mode:
    // content  FrameTail  ...      FrameHeader content
    //                              ------------------>
    // ----------------->  
    if(MsgEndIndex < ReadIndex) 
    {       

        //caluate the message content length
        MsgLen = MsgEndIndex + BUF_SIZE - ReadIndex - MsgFrameHeaderLen;

        //User receive buf is enough for message content?
        if(Size < MsgLen)
        {
            //User buf is too small
            return (BUFSMALL);
        }

        //Skip message frame header
        ReadIndex += MsgFrameHeaderLen;
        ReadIndex %= BUF_SIZE;
        
        ByteCnt   -= MsgFrameHeaderLen;
        
        //Get the message content
        if(ReadIndex < MsgEndIndex)
        {
            memcpy(RecBuf, &FIFOBuf[ReadIndex], (MsgEndIndex - ReadIndex));
        }
        else
        {
            MsgLen = 0;
            MsgLen += (BUF_SIZE - ReadIndex);
            memcpy(RecBuf, &FIFOBuf[ReadIndex], MsgLen);
            memcpy(RecBuf + MsgLen, &FIFOBuf[0], MsgEndIndex);
        }
        
        ByteCnt += MsgLen + MsgEndIndex + MsgFrameTailLen;
        
        //Skip the message frame tail
        ReadIndex = MsgEndIndex + MsgFrameTailLen;
        ReadIndex %= BUF_SIZE;

        //Update the message count
        MsgCnt -= 1;
        
        return (OK);
    }
    
    return (NOMSG);
}


//Inform FIFO to new add a message
//OK-------------> Successful put a message header into FIFO
//FULL-----------> FIFO is full, can not put a message
StatusCode FIFOMsgStart(void)
{
    uint32 i = 0;
    //check there is enough room for a new message
    //at least can be put a byte
    //for example:
    //FrameHeader MessageContent FrameTail
    //**BEGIN**   H              **END**
    if(FIFOByteAvailable() <= (MsgFrameHeaderLen + MsgFrameTailLen + 1))
    {
        Status = FULL;
        return (FULL);
    }
    else
    {
        //Add a new message header to FIFO
        while(i < MsgFrameHeaderLen)
        {
            FIFOBuf[WriteIndex] = MsgFrameHeader[i++];
            WriteIndex = (WriteIndex + 1) % BUF_SIZE;
        }

        ByteCnt += MsgFrameHeaderLen;
        Status = OK;
        return (OK);
    }
}

//Inform FIFO that message is end
//OK-------------> Successful put a message tail into FIFO
//FULL-----------> FIFO is full, can not put message any more
StatusCode FIFOMsgEnd(void)
{
    uint32 i = 0;
    
    if(FIFOByteAvailable() < MsgFrameTailLen)  //Buffer is full ?
    {
        Status = FULL;
        return (FULL);
    }
    else
    {
        //Add a new message tail to FIFO
        while(i < MsgFrameTailLen)
        {
            FIFOBuf[WriteIndex] = MsgFrameTail[i++];
            WriteIndex = (WriteIndex + 1) % BUF_SIZE;
        }
        
        ByteCnt += MsgFrameTailLen;        
        //Update Message count
        MsgCnt += 1;
        Status = OK;

        return (OK);
    }
}

