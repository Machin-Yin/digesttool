#define _GNU_SOURCE


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "sm3.h"

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

int sm3_starts(sm3_context *index)
{
  if ( index == NULL )
  {
    return -1;
  }

  index->total_bytes_High = 0;
  index->total_bytes_Low = 0;
  index->vector[0] = 0x7380166f;
  index->vector[1] = 0x4914b2b9;
  index->vector[2] = 0x172442d7;
  index->vector[3] = 0xda8a0600;
  index->vector[4] = 0xa96f30bc;
  index->vector[5] = 0x163138aa;
  index->vector[6] = 0xe38dee4d;
  index->vector[7] = 0xb0fb0e4e;

  return 0;
}

static void SM3_CF(sm3_context *index, uint8_t *byte_64_block )
{
uint32_t j,temp,W[68];

#ifdef DEBUG
  for(j=0;j<64;j++)
	printf("%d:%8x\n",j,byte_64_block[j]);
  printf("\n--------------------\n");
#endif

uint32_t A,B,C,D,E,F,G,H,SS1,SS2,TT1,TT2;
  CONCAT_4_BYTES( W[0],  byte_64_block,  0 );
  CONCAT_4_BYTES( W[1],  byte_64_block,  4 );
  CONCAT_4_BYTES( W[2],  byte_64_block,  8 );
  CONCAT_4_BYTES( W[3],  byte_64_block, 12 );
  CONCAT_4_BYTES( W[4],  byte_64_block, 16 );
  CONCAT_4_BYTES( W[5],  byte_64_block, 20 );
  CONCAT_4_BYTES( W[6],  byte_64_block, 24 );
  CONCAT_4_BYTES( W[7],  byte_64_block, 28 );
  CONCAT_4_BYTES( W[8],  byte_64_block, 32 );
  CONCAT_4_BYTES( W[9],  byte_64_block, 36 );
  CONCAT_4_BYTES( W[10], byte_64_block, 40 );
  CONCAT_4_BYTES( W[11], byte_64_block, 44 );
  CONCAT_4_BYTES( W[12], byte_64_block, 48 );
  CONCAT_4_BYTES( W[13], byte_64_block, 52 );
  CONCAT_4_BYTES( W[14], byte_64_block, 56 );
  CONCAT_4_BYTES( W[15], byte_64_block, 60 );
#ifdef DEBUG
  for(j=0;j<16;j++)
	{
	printf("%d:%8x\n",j,W[j]);
	}
#endif
  for(j=16;j<68;j++)
	{
// waitting to modified 
// there is something strange here,"P1(W[j-16]^W[j-9]^rol(W[j-3],15))" will get a error result
	temp = W[j-16]^W[j-9]^rol(W[j-3],15);
	W[j] = P1(temp)^rol(W[j-13],7)^(W[j-6]);
//	W[j] = P1((W[j-16]^W[j-9]^rol(W[j-3],15)))^rol(W[j-13],7)^(W[j-6]);

#ifdef DEBUG

	printf("%d::(*)=%8x :temp=%8x :P1((*))=%8x :P1(*)=%8x :p1(temp)=%8x\n",
		j,(W[j-16]^W[j-9]^rol(W[j-3],15)),P1((W[j-16]^W[j-9]^rol(W[j-3],15))),temp,P1(W[j-16]^W[j-9]^rol(W[j-3],15)),P1(temp));

#endif
	}
  A = index->vector[0];
  B = index->vector[1];
  C = index->vector[2];
  D = index->vector[3];
  E = index->vector[4];
  F = index->vector[5];
  G = index->vector[6];
  H = index->vector[7];
#ifdef DEBUG
	printf(" :A:%08x,B:%08x,C:%08x,D:%08x,E:%08x,F:%08x,G:%08x,H:%08x\n",A,B,C,D,E,F,G,H);
#endif

#define T 0x79cc4519
#define FF(X,Y,Z) (X^Y^Z)
#define GG(X,Y,Z) (X^Y^Z)

  for(j=0;j<16;j++)
	{
	SS1 = rol(rol(A,12) + E + rol(T,j),7);
	SS2 = SS1^(rol(A,12));
	TT1 = FF(A,B,C) + D + SS2 + (W[j]^W[j+4]);
	TT2 = GG(E,F,G) + H + SS1 + W[j];
	D = C;
	C = rol(B,9);
	B = A;
	A = TT1;
	H = G;
	G = rol(F,19);
	F = E;
	E = P0(TT2);
#ifdef DEBUG
	printf("%d: A:%08x,B:%08x,C:%08x,D:%08x,E:%08x,F:%08x,G:%08x,H:%08x\n",j,A,B,C,D,E,F,G,H);
#endif
	}

#undef T
#undef FF 
#undef GG


#define T 0x7a879d8a 
#define FF(X,Y,Z) ((X&Y)|(X&Z)|(Y&Z))
#define GG(X,Y,Z) ((X&Y)|(~X&Z))


  for(j=16;j<64;j++)
	{
	SS1 = rol(rol(A,12) + E + rol(T,j),7);
	SS2 = SS1^(rol(A,12));
	TT1 = FF(A,B,C) + D + SS2 + (W[j]^W[j+4]);
	TT2 = GG(E,F,G) + H + SS1 + W[j];
	D = C;
	C = rol(B,9);
	B = A;
	A = TT1;
	H = G;
	G = rol(F,19);
	F = E;
	E = P0(TT2);
#ifdef DEBUG
	printf("%d: A:%08x,B:%08x,C:%08x,D:%08x,E:%08x,F:%08x,G:%08x,H:%08x\n",j,A,B,C,D,E,F,G,H);
#endif
	}
#undef T
#undef FF 
#undef GG

index->vector[0] ^= A;
index->vector[1] ^= B;
index->vector[2] ^= C;
index->vector[3] ^= D;
index->vector[4] ^= E;
index->vector[5] ^= F;
index->vector[6] ^= G;
index->vector[7] ^= H;

}

int sm3_update(sm3_context *index, const unsigned char *chunk_data, int chunk_length)
{

  uint32_t left, fill;
  uint32_t i;


  if ( (index == NULL) || (chunk_data == NULL) || (chunk_length < 1) )
       {
           return -1;
       }

  left = index->total_bytes_Low & 0x3F;
  fill = 64 - left;
  index->total_bytes_Low += chunk_length;
  index->total_bytes_Low &= 0xFFFFFFFF;

  if ( index->total_bytes_Low < chunk_length )
       {
           index->total_bytes_High++;
       }

  if ( (left > 0) && (chunk_length >= fill) )
       {
           for ( i = 0; i < fill; i++ )
  	     {
                 index->buffer[left + i] = chunk_data[i];
             }
           SM3_CF( index, index->buffer );
           chunk_length -= fill;
           chunk_data  += fill;
           left = 0;
        }

  while( chunk_length >= 64 )
        {
           SM3_CF( index, chunk_data );
           chunk_length -= 64;
           chunk_data  += 64;
        }

  if ( chunk_length > 0 )
        {
            for ( i = 0; i < chunk_length; i++ )
               {
                     index->buffer[left + i] = chunk_data[i];
               }
        }
  return 0;
  }

/*
int SM3_final(SM3_context *index, uint32_t *SM3_hash)
{

  uint32_t last, padn;
  uint32_t high, low;
  uint8_t  msglen[8];
  int   ret;
  if ( (index == NULL) || (SM3_hash == NULL) )
       {
           *SM3_hash = 0;
           return -1;
       }
  high = ( index->total_bytes_Low >> 29 ) | ( index->total_bytes_High <<  3 );
  low  = ( index->total_bytes_Low <<  3 );
  SPLIT_INTO_4_BYTES( high, msglen, 0 );
  SPLIT_INTO_4_BYTES( low,  msglen, 4 );

  last = index->total_bytes_Low & 0x3F;
  padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
  ret = SM3_update( index, SM3_padding, padn );
  ret = SM3_update( index, msglen, 8 );
  SM3_hash[0] = index->vector[0];
  SM3_hash[1] = index->vector[1];
  SM3_hash[2] = index->vector[2];
  SM3_hash[3] = index->vector[3];
  SM3_hash[4] = index->vector[4];
  SM3_hash[5] = index->vector[5];
  SM3_hash[6] = index->vector[6];
  SM3_hash[7] = index->vector[7];
  SM3_hash[8] = index->vector[8];
  return 0;

}
*/
//int sm3_final(SM3_context *index, unsigned char output[32])
int sm3_finish(sm3_context *index, unsigned char output[SM3_DIGEST_SIZE])
{

  uint32_t last, padn;
  uint32_t high, low;
  uint8_t  msglen[8];
  int   ret;
  if ( (index == NULL) || (output == NULL) )
       {
           *output = 0;
           return -1;
       }
  high = ( index->total_bytes_Low >> 29 ) | ( index->total_bytes_High <<  3 );
  low  = ( index->total_bytes_Low <<  3 );
  SPLIT_INTO_4_BYTES( high, msglen, 0 );
  SPLIT_INTO_4_BYTES( low,  msglen, 4 );

  last = index->total_bytes_Low & 0x3F;
  padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
  ret = sm3_update( index, SM3_padding, padn );
  ret = sm3_update( index, msglen, 8 );
  
  PUT_ULONG_BE( index->vector[0], output,  0 );
  PUT_ULONG_BE( index->vector[1], output,  4 );
  PUT_ULONG_BE( index->vector[2], output,  8 );
  PUT_ULONG_BE( index->vector[3], output, 12 );
  PUT_ULONG_BE( index->vector[4], output, 16 );
  PUT_ULONG_BE( index->vector[5], output, 20 );
  PUT_ULONG_BE( index->vector[6], output, 24 );
  PUT_ULONG_BE( index->vector[7], output, 28 );
  return 0;

}
/*
int main(int argc,char *argv[])
{
	int i;
	SM3_context context;
	unsigned char digest[32];
	SM3_init(&context);
	SM3_update(&context,"abc",3);
	SM3_final(&context,digest);
	for(i=0;i<32;i++)
		printf("%x",digest[i]);
	printf("\n");
}
*/


/*
 * output = SM3( input buffer )
 */
void sm3( const unsigned char *input, int ilen,
           unsigned char output[SM3_DIGEST_SIZE] )
{
    sm3_context ctx;

    sm3_starts( &ctx );
    sm3_update( &ctx, input, ilen );
    sm3_finish( &ctx, output );

    memset( &ctx, 0, sizeof( sm3_context ) );
}

/*
 * output = SM3( file contents )
 */
int sm3_file( const char *path, unsigned char output[SM3_DIGEST_SIZE] )
{
    FILE *f;
    size_t n;
    sm3_context ctx;
    unsigned char buf[1024];

    if( ( f = fopen( path, "rb" ) ) == NULL )
        return( 1 );

    sm3_starts( &ctx );

    while( ( n = fread( buf, 1, sizeof( buf ), f ) ) > 0 )
        sm3_update( &ctx, buf, (int) n );

    sm3_finish( &ctx, output );

    memset( &ctx, 0, sizeof( sm3_context ) );

    if( ferror( f ) != 0 )
    {
        fclose( f );
        return( 2 );
    }

    fclose( f );
    return( 0 );
}

/*
 * SM3 HMAC context setup
 */
void sm3_hmac_starts( sm3_context *ctx, const unsigned char *key, int keylen )
{
    int i;
    unsigned char sum[SM3_DIGEST_SIZE];

    if( keylen > 64 )
    {
        sm3( key, keylen, sum );
        keylen = SM3_DIGEST_SIZE;
		//keylen = ( is224 ) ? 28 : 32;
        key = sum;
    }

    memset( ctx->ipad, 0x36, 64 );
    memset( ctx->opad, 0x5C, 64 );

    for( i = 0; i < keylen; i++ )
    {
        ctx->ipad[i] = (unsigned char)( ctx->ipad[i] ^ key[i] );
        ctx->opad[i] = (unsigned char)( ctx->opad[i] ^ key[i] );
    }

    sm3_starts( ctx);
    sm3_update( ctx, ctx->ipad, 64 );

    memset( sum, 0, sizeof( sum ) );
}

/*
 * SM3 HMAC process buffer
 */
void sm3_hmac_update( sm3_context *ctx, const unsigned char *input, int ilen )
{
    sm3_update( ctx, input, ilen );
}

/*
 * SM3 HMAC final digest
 */
void sm3_hmac_finish( sm3_context *ctx, unsigned char output[SM3_DIGEST_SIZE] )
{
    int hlen;
    unsigned char tmpbuf[SM3_DIGEST_SIZE];

    //is224 = ctx->is224;
    hlen =  32;

    sm3_finish( ctx, tmpbuf );
    sm3_starts( ctx );
    sm3_update( ctx, ctx->opad, 64 );
    sm3_update( ctx, tmpbuf, hlen );
    sm3_finish( ctx, output );

    memset( tmpbuf, 0, sizeof( tmpbuf ) );
}

/*
 * output = HMAC-SM#( hmac key, input buffer )
 */
void sm3_hmac( const unsigned char *key, int keylen,
                const unsigned char *input, int ilen,
                unsigned char output[SM3_DIGEST_SIZE] )
{
    sm3_context ctx;

    sm3_hmac_starts( &ctx, key, keylen);
    sm3_hmac_update( &ctx, input, ilen );
    sm3_hmac_finish( &ctx, output );

    memset( &ctx, 0, sizeof( sm3_context ) );
}
