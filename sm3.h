
#define SM3_DIGEST_SIZE		32
/**
 * \brief          SM3 context structure
 */
/*writen by tjs in 2011-4-19*/

#ifndef _SM3_
#define _SM3_

#include <inttypes.h>

typedef struct
{
  uint32_t total_bytes_High;
  uint32_t total_bytes_Low;
  uint32_t vector[8];
  uint8_t  buffer[64];     /* 64 byte buffer                            */
  
  unsigned char ipad[64];     /*!< HMAC: inner padding        */
  unsigned char opad[64];     /*!< HMAC: outer padding        */
} sm3_context;


#define rol(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))
/*
inline int rol(uint32_t operand, uint8_t width){ 
	 asm volatile("rol %%cl, %%eax" 
               : "=a" (operand) 
               : "a" (operand), "c" (width) 
               ); 
}
*/
#define P0(x) ((x^(rol(x,9))^(rol(x,17))))
#define P1(x) ((x^(rol(x,15))^(rol(x,23))))

#define CONCAT_4_BYTES( w32, w8, w8_i)            \
{                                                 \
    (w32) = ( (uint32_t) (w8)[(w8_i)    ] << 24 ) |  \
            ( (uint32_t) (w8)[(w8_i) + 1] << 16 ) |  \
            ( (uint32_t) (w8)[(w8_i) + 2] <<  8 ) |  \
            ( (uint32_t) (w8)[(w8_i) + 3]       );   \
}

#define SPLIT_INTO_4_BYTES( w32, w8, w8_i)        \
{                                                 \
    (w8)[(w8_i)] = (uint8_t) ( (w32) >> 24 );    \
    (w8)[(w8_i) + 1] = (uint8_t) ( (w32) >> 16 );    \
    (w8)[(w8_i) + 2] = (uint8_t) ( (w32) >>  8 );    \
    (w8)[(w8_i) + 3] = (uint8_t) ( (w32)       );    \
}

#if 1
static uint8_t SM3_padding[64] =
{
 (uint8_t) 0x80, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
 (uint8_t)    0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
 (uint8_t)    0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0,
 (uint8_t)    0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0
};
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* sm3_starts											                    */
/* \brief			��ʼ�� SM3 ������                                   */
/* \param ctx		SM3 ������                                          */
/************************************************************************/
int sm3_starts( sm3_context *ctx );

/************************************************************************/
/* sm3_update															*/
/* \brief			������ϢժҪ										*/
/* \param ctx		SM3 ������                                          */
/* \param input		���뻺����											*/
/* \param ilen		���뻺������С										*/
/************************************************************************/
int sm3_update(sm3_context *index, const unsigned char *chunk_data, int chunk_length);

/************************************************************************/
/* sm3_finish															*/
/* \brief			��ȡժҪֵ											*/
/* \param ctx		SM3 ������                                          */
/* \param output	���������											*/
/* \remark			�������ȡ����� SM3_DIGEST_SIZE ��С���ڴ�			*/
/************************************************************************/
int sm3_finish( sm3_context *index, unsigned char output[SM3_DIGEST_SIZE] );

/************************************************************************/
/* sm3																	*/
/* \brief			���㲢��ȡ��ϢժҪֵ								*/
/* \param input		���뻺����											*/
/* \param ilen		���뻺������С										*/
/* \param output	���������											*/
/* \remark			�������ȡ����� SM3_DIGEST_SIZE ��С���ڴ�			*/
/************************************************************************/
void sm3( const unsigned char *input, int ilen, unsigned char output[SM3_DIGEST_SIZE] );

int sm3_file( const char *path, unsigned char output[SM3_DIGEST_SIZE] );

#ifdef __cplusplus
}
#endif
