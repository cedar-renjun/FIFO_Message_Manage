#ifndef _FIFO_H_
#define _FIFO_H_
    
#define BUF_SIZE                200

#define OK                      0
#define OVERFLOW                1
#define NOMSG                   2
#define BUFSMALL                3
#define FULL                    4


#if 1
typedef unsigned int  uint32;
typedef signed   int  int32;
typedef signed   char int8;
typedef unsigned char uint8;
#endif

typedef uint32 StatusCode;








void FIFOInit(void);

void FIFOFlush(void);

StatusCode FIFOStatus(void);

uint32 FIFOMsgNum(void);

StatusCode FIFOMsgGet(char * RecBuf, uint32 Size);

StatusCode FIFOMsgStart(void);

StatusCode FIFOMsgBytePut(char ch);

StatusCode FIFOMsgEnd(void);

#endif

