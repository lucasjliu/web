//
//  MD5.h
//
//  modified by jiahuan.liu
//  10/30/2016
//
#ifndef __CMD5_H
#define __CMD5_H

#include <string>

using namespace std;

#ifndef GET_ULONG_LE
#define GET_ULONG_LE(n,b,i)                             \
{                                                       \
(n) = ( (unsigned long) (b)[(i)    ]       )        \
| ( (unsigned long) (b)[(i) + 1] <<  8 )        \
| ( (unsigned long) (b)[(i) + 2] << 16 )        \
| ( (unsigned long) (b)[(i) + 3] << 24 );       \
}
#endif
    
#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                             \
{                                                       \
(b)[(i)    ] = (unsigned char) ( (n)       );       \
(b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
(b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
(b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif
    
class CMD5
{
    typedef unsigned char *POINTER;
    typedef unsigned short int UINT2;
    //typedef unsigned long int UINT4;
    typedef uint32_t UINT4;
    
    typedef struct
    {
        /**
         * state (ABCD)
         */
        //unsigned long state[4];
        UINT4 state[4];
        
        /**
         * number of bits, modulo 2^64 (lsb first)
         */
        //unsigned long count[2];
        UINT4 count[2];
        
        /**
         * input buffer
         */
        unsigned char buffer[64];
    }MD5;
    
public:
    /* 
     * modified by jiahuan.liu
     * @return 16 chars string
     */
    static string md5bin(const string &sString);
    
    /*
     * modified by jiahuan.liu
     * @return 32 chars string
     */
    static string md5str (const string &sString);
    
protected:
    
    static string bin2str(const void *buf, size_t len, const string &sSep);
    
    /**
     * @brief MD5 init.
     */
    static void md5init(MD5 *context);
    
    /**
     * @brief MD5 block update operation，Continues an MD5
     * message-digest operation, processing another message block,
     * and updating the context
     */
    static void md5update (MD5 *context, unsigned char *input,unsigned int inputLen);
    
    /**
     * @brief  MD5 finalization，Ends an MD5 message-digest
     * operation, writing the message digest and zeroizing the
     * context
     */
    static void md5final (unsigned char digest[16], MD5 *context);
    
    /**
     * @brief  MD5 basic transformation，Transforms state based on
     *   	  block
     */
    static void md5_process(MD5 *ctx, const unsigned char data[64]);
    
    /**
     * @brief  Encodes input (UINT4) into output (unsigned
     *   	  char)，Assumes len is a multiple of 4
     */
    static void encode (unsigned char *output,UINT4 *input,unsigned int len);
    
    /**
     * @brief Decodes input (unsigned char) into output (UINT4)，
     * Assumes len is a multiple of 4
     */    
    static void decode (UINT4 *output,unsigned char *input,unsigned int len);
    
    /** 
     * @brief replace "for loop" with standard memcpy if possible. 
     *
     */    
    static void md5_memcpy (POINTER output,POINTER input,unsigned int len);
    
    /** 
     * @brief replace "for loop" with standard memset if possible. 
     *
     */ 
    static void md5_memset (POINTER output,int value,unsigned int len);
    
    /**
     * @padding value
     */
    static unsigned char PADDING[64];
};

#endif
