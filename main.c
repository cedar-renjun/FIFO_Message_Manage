#include <stdio.h>
#include <string.h>
#include "FIFO.h"

char tmp0[] = {
    "Hellorld123456789#**    \
        Heorld123456789#** \
            Helld123456789#**"
};

char tmp1[] = {
    "Hello6789#**       \
        Hel6789#fs*s            \
            d89#**"
};

char tmp2[] = {
    "125jfalkjwegmwghwjeommsgkowem**   \
        Hello worldgaewgwgewg*ogewgewworld123afaegew456789#**"
};

char tmp_Test[] = {
    "125jfalkjwegmwghwjeommsgkowem**   \
        Hello worldgaewgwgewg*  \
            ogewgewworld123afaegew456789#**"
};

char mn[200];

int main(void)
{
    StatusCode state = OK;
    FIFOInit();
    
    /////////////////////////////////////////////
    
    state = FIFOMsgStart();
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    
    for(int i =0; tmp0[i] != '\0'; i++)
    {
        state = FIFOMsgBytePut(tmp0[i]);
        if(state != OK)
        {
            printf("ERROR\r\n");
            printf("ErrorCode:%d\r\n", state);
        }
    } 
    
    state = FIFOMsgEnd();
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    
    
    /////////////////////////////////////////////
    state = FIFOMsgStart();
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    
    for(int i =0; tmp1[i] != '\0'; i++)
    {
        state = FIFOMsgBytePut(tmp1[i]);
        if(state != OK)
        {
            printf("ERROR\r\n");
            printf("ErrorCode:%d\r\n", state);
        }
    } 
    
    state = FIFOMsgEnd();
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    
    /////////////////////////////////////////////
    state = FIFOMsgGet(mn, 200);
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    
    /////////////////////////////////////////////
    state = FIFOMsgStart();
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    
    for(int i =0; tmp2[i] != '\0'; i++)
    {
        state = FIFOMsgBytePut(tmp2[i]);
        if(state != OK)
        {
            printf("ERROR\r\n");
            printf("ErrorCode:%d\r\n", state);
        }
    } 
    
    state = FIFOMsgEnd();
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    ////////////////////////////////////////////
    //check it
    
    memset(mn, '\0', 200);
    state = FIFOMsgGet(mn, 200);
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    state = strcmp(mn, tmp1);
    if(state != 0)
    {
        printf("compare failure\r\n");
    }
    
    
    memset(mn, '\0', 200);
    state = FIFOMsgGet(mn, 200);
    if(state != OK)
    {
        printf("ERROR\r\n");
        printf("ErrorCode:%d\r\n", state);
    }
    state = strcmp(mn, tmp2);
    if(state != 0)
    {
        printf("compare failure\r\n");
    }
    
    return (0);
}
