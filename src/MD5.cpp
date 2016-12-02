//
//  MD5.cpp
//
//  modified by jiahuan.liu
//  10/30/2016
//
#include "MD5.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
    
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
    
    
unsigned char CMD5::PADDING[64] =
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void CMD5::md5init (MD5 *context)
{
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;    /* load magic initialization constants.*/
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void CMD5::md5_process( MD5 *md5, const unsigned char data[64] )
{
    unsigned long X[16], A, B, C, D;
    
    GET_ULONG_LE( X[ 0], data,  0 );
    GET_ULONG_LE( X[ 1], data,  4 );
    GET_ULONG_LE( X[ 2], data,  8 );
    GET_ULONG_LE( X[ 3], data, 12 );
    GET_ULONG_LE( X[ 4], data, 16 );
    GET_ULONG_LE( X[ 5], data, 20 );
    GET_ULONG_LE( X[ 6], data, 24 );
    GET_ULONG_LE( X[ 7], data, 28 );
    GET_ULONG_LE( X[ 8], data, 32 );
    GET_ULONG_LE( X[ 9], data, 36 );
    GET_ULONG_LE( X[10], data, 40 );
    GET_ULONG_LE( X[11], data, 44 );
    GET_ULONG_LE( X[12], data, 48 );
    GET_ULONG_LE( X[13], data, 52 );
    GET_ULONG_LE( X[14], data, 56 );
    GET_ULONG_LE( X[15], data, 60 );
    
#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))
    
#define P(a,b,c,d,k,s,t)                                \
{                                                       \
a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
}
    
    A = md5->state[0];
    B = md5->state[1];
    C = md5->state[2];
    D = md5->state[3];
    
#define F(x,y,z) (z ^ (x & (y ^ z)))
    
    P( A, B, C, D,  0,  7, 0xD76AA478 );
    P( D, A, B, C,  1, 12, 0xE8C7B756 );
    P( C, D, A, B,  2, 17, 0x242070DB );
    P( B, C, D, A,  3, 22, 0xC1BDCEEE );
    P( A, B, C, D,  4,  7, 0xF57C0FAF );
    P( D, A, B, C,  5, 12, 0x4787C62A );
    P( C, D, A, B,  6, 17, 0xA8304613 );
    P( B, C, D, A,  7, 22, 0xFD469501 );
    P( A, B, C, D,  8,  7, 0x698098D8 );
    P( D, A, B, C,  9, 12, 0x8B44F7AF );
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P( B, C, D, A, 11, 22, 0x895CD7BE );
    P( A, B, C, D, 12,  7, 0x6B901122 );
    P( D, A, B, C, 13, 12, 0xFD987193 );
    P( C, D, A, B, 14, 17, 0xA679438E );
    P( B, C, D, A, 15, 22, 0x49B40821 );
    
#undef F
    
#define F(x,y,z) (y ^ (z & (x ^ y)))
    
    P( A, B, C, D,  1,  5, 0xF61E2562 );
    P( D, A, B, C,  6,  9, 0xC040B340 );
    P( C, D, A, B, 11, 14, 0x265E5A51 );
    P( B, C, D, A,  0, 20, 0xE9B6C7AA );
    P( A, B, C, D,  5,  5, 0xD62F105D );
    P( D, A, B, C, 10,  9, 0x02441453 );
    P( C, D, A, B, 15, 14, 0xD8A1E681 );
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P( A, B, C, D,  9,  5, 0x21E1CDE6 );
    P( D, A, B, C, 14,  9, 0xC33707D6 );
    P( C, D, A, B,  3, 14, 0xF4D50D87 );
    P( B, C, D, A,  8, 20, 0x455A14ED );
    P( A, B, C, D, 13,  5, 0xA9E3E905 );
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P( C, D, A, B,  7, 14, 0x676F02D9 );
    P( B, C, D, A, 12, 20, 0x8D2A4C8A );
    
#undef F
    
#define F(x,y,z) (x ^ y ^ z)
    
    P( A, B, C, D,  5,  4, 0xFFFA3942 );
    P( D, A, B, C,  8, 11, 0x8771F681 );
    P( C, D, A, B, 11, 16, 0x6D9D6122 );
    P( B, C, D, A, 14, 23, 0xFDE5380C );
    P( A, B, C, D,  1,  4, 0xA4BEEA44 );
    P( D, A, B, C,  4, 11, 0x4BDECFA9 );
    P( C, D, A, B,  7, 16, 0xF6BB4B60 );
    P( B, C, D, A, 10, 23, 0xBEBFBC70 );
    P( A, B, C, D, 13,  4, 0x289B7EC6 );
    P( D, A, B, C,  0, 11, 0xEAA127FA );
    P( C, D, A, B,  3, 16, 0xD4EF3085 );
    P( B, C, D, A,  6, 23, 0x04881D05 );
    P( A, B, C, D,  9,  4, 0xD9D4D039 );
    P( D, A, B, C, 12, 11, 0xE6DB99E5 );
    P( C, D, A, B, 15, 16, 0x1FA27CF8 );
    P( B, C, D, A,  2, 23, 0xC4AC5665 );
    
#undef F
    
#define F(x,y,z) (y ^ (x | ~z))
    
    P( A, B, C, D,  0,  6, 0xF4292244 );
    P( D, A, B, C,  7, 10, 0x432AFF97 );
    P( C, D, A, B, 14, 15, 0xAB9423A7 );
    P( B, C, D, A,  5, 21, 0xFC93A039 );
    P( A, B, C, D, 12,  6, 0x655B59C3 );
    P( D, A, B, C,  3, 10, 0x8F0CCC92 );
    P( C, D, A, B, 10, 15, 0xFFEFF47D );
    P( B, C, D, A,  1, 21, 0x85845DD1 );
    P( A, B, C, D,  8,  6, 0x6FA87E4F );
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P( C, D, A, B,  6, 15, 0xA3014314 );
    P( B, C, D, A, 13, 21, 0x4E0811A1 );
    P( A, B, C, D,  4,  6, 0xF7537E82 );
    P( D, A, B, C, 11, 10, 0xBD3AF235 );
    P( C, D, A, B,  2, 15, 0x2AD7D2BB );
    P( B, C, D, A,  9, 21, 0xEB86D391 );
    
#undef F
    
    md5->state[0] += A;
    md5->state[1] += B;
    md5->state[2] += C;
    md5->state[3] += D;
}

void CMD5::md5update(MD5 *md5, unsigned char *input, unsigned int ilen )
{
    unsigned int fill;
    unsigned long left;
    if( ilen <= 0 )
        return;
    left = md5->count[0] & 0x3F;
    fill = 64 - (unsigned int)left;
    md5->count[0] += ilen;
    md5->count[0] &= 0xFFFFFFFF;
    if( md5->count[0] < (unsigned long) ilen )
        md5->count[1]++;
    if( left && ilen >= fill )
    {
        memcpy( (void *) (md5->buffer + left),(void *) input, fill );
        md5_process( md5, md5->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }
    while( ilen >= 64 )
    {
        md5_process( md5, input );
        input += 64;
        ilen  -= 64;
    }
    if( ilen > 0 )
    {
        memcpy( (void *) (md5->buffer + left),(void *) input, ilen );
    }
}


void CMD5::md5final (unsigned char output[16], MD5 *md5)
{
    unsigned long last, padn;
    unsigned long high, low;
    unsigned char msglen[8];
    
    high = ( md5->count[0] >> 29 )
    | ( md5->count[1] <<  3 );
    low  = ( md5->count[0] <<  3 );
    
    PUT_ULONG_LE( low,  msglen, 0 );
    PUT_ULONG_LE( high, msglen, 4 );
    
    last = md5->count[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
    
    md5update( md5, (unsigned char *) PADDING, (unsigned int)padn );
    md5update( md5, msglen, 8 );
    
    PUT_ULONG_LE( md5->state[0], output,  0 );
    PUT_ULONG_LE( md5->state[1], output,  4 );
    PUT_ULONG_LE( md5->state[2], output,  8 );
    PUT_ULONG_LE( md5->state[3], output, 12 );
}

void CMD5::encode (unsigned char *output,UINT4 *input,unsigned int len)
{
    unsigned int i, j;
    
    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

void CMD5::decode (UINT4 *output,unsigned char *input,unsigned int len)
{
    unsigned int i, j;
    
    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = ((UINT4)input[j])
        | (((UINT4)input[j+1]) << 8)
        | (((UINT4)input[j+2]) << 16)
        | (((UINT4)input[j+3]) << 24);
    }
}

void CMD5::md5_memcpy (POINTER output,POINTER input,unsigned int len)
{
    unsigned int i;
    
    for (i = 0; i < len; i++)
    {
        output[i] = input[i];
    }
}

void CMD5::md5_memset (POINTER output,int value,unsigned int len)
{
    unsigned int i;
    
    for (i = 0; i < len; i++)
    {
        ((char *)output)[i] = (char)value;
    }
}

string CMD5::md5bin(const string &sString)
{
    unsigned char sBuf[16];
    
    MD5 context;
    md5init (&context);
    md5update (&context, (unsigned char*)sString.c_str(), (unsigned int)sString.length());
    md5final (sBuf, &context);
    
    string sRet;
    
    sRet.assign((const char *)sBuf, 16);
    
    return sRet;
}

string CMD5::md5str(const string &sString)
{
    string s = md5bin(sString);
    return bin2str((const void *)s.data(), s.length(), "");
}

string CMD5::bin2str(const void *buf, size_t len, const string &sSep)
{
    if(buf == NULL || len <=0 )
    {
        return "";
    }
    
    string sRet;
    char sBuf[255];
    const char *p = (const char *) buf;
    
    for (size_t i = 0; i < len; ++i, ++p)
    {
        snprintf(sBuf, sizeof(sBuf), "%02x%s", (unsigned char) *p, sSep.c_str());
        sRet += sBuf;
    }
    
    return sRet;
}
