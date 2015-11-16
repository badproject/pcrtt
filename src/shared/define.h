/**
 * Posix compliant version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2015 NEngine Team
 * 
 * This file is part of CSA-Rainbow-Table-Tool.
 * 
 * CSA-Rainbow-Table-Tool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSA-Rainbow-Table-Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSA-Rainbow-Table-Tool.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * This software is based on the windows version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2012  Colibri <colibri.dvb@googlemail.com>  
 * http://colibri.net63.net/ alias http://colibri-dvb.info/
 * 
 */
#if !defined(PCRTT_SRC_SHARED_DEFINE_HEADER_H_)
#define PCRTT_SRC_SHARED_DEFINE_HEADER_H_

#define BLOCKS_RB 0x200
#define THREADS_RB 0x80

#define RAINBOW_CHAIN_LEN 0x10000
#define RAINBOW_CHAIN_LEN_PER_KERNEL 0x100

// Chain file name syntax:
// part1: algo
// part2: plain
// part3: chain len
// part4: start value prefix
// file name example: CSA_B8hx00h_10000h_12345Ch.rbc
#define FILE_PART1_ALGO	"CSA"
#define FILE_PART2_PLAIN "B8hx00h"

// sorted by start values
#define FILE_EXT_RB_CHAINS	"rbc"
// sorted by end values
#define FILE_EXT_RB_TABLE	"rbt"
// end CW cache file
#define FILE_EXT_END_CW		"ecw"

// goal: 768 MByte per chain file (= 0x4000000 * (6+6))
#define MAX_ELEMENTS_PER_CHAIN_FILE		0x4000000

#define COUNT_OF_VERIFICATION_CHECKS_WITH_CPU_PER_CHAIN_FILE	10

#define SEARCH_CRYPT8_IN_TS_MAX_FILE_SIZE_MB	4096

#define TS_SIZE		188
#define SYNC_BYTE	0x47

#define DVBCSA_DATA_SIZE	8
#define DVBCSA_KEYSBUFF_SIZE	56

typedef struct _DEBUG_STRUCT { unsigned char Dummy; } DEBUG_STRUCT;

typedef uint8_t			dvbcsa_block_t[DVBCSA_DATA_SIZE];
typedef uint8_t			dvbcsa_keys_t[DVBCSA_KEYSBUFF_SIZE];

/**
 * Get nibble from A and B 40 bits registers
 */
#define	NBGET(r, n)	(r>>(n * 4))

/**
 * CSA algorithm is using X, Y, Z, D, E, F 4 bits registers
 * and P, Q, C 1 bits registers. these registers are stored
 * in `pqzyx' and `cfed' variables to improve speed.
 */
#define GETX(x)	(x)
#define GETY(x)	((x)>>4)
#define GETZ(x)	((x)>>8)
#define TSTP(x)	((x)&0x1000)
#define TSTQ(x)	((x)&0x2000)
#define GETD(x)	(x)
#define GETE(x)	((x)>>4)
#define GETF(x)	((x)>>8)
#define GETC(x)	((x)>>12)

#if defined(COPY_TO_SHARED_sbox)
#	define COPY_CONST_TO_SHARED_sbox \
		memcpy(sbox, const_sbox, sizeof(uint16_t) * 7 * 32);
#else
#	define COPY_CONST_TO_SHARED_sbox
#endif

#if defined(COPY_TO_SHARED_csa_key_perm)
#	define COPY_CONST_TO_SHARED_csa_key_perm \
		memcpy(csa_key_perm, const_csa_key_perm, sizeof(uint8_t) * 64);
#else
#	define COPY_CONST_TO_SHARED_csa_key_perm
#endif

#if defined(COPY_TO_SHARED_csa_block_perm)
#	define COPY_CONST_TO_SHARED_csa_block_perm \
		memcpy(csa_block_perm, const_csa_block_perm, sizeof(uint8_t) * 256);
#else
#	define COPY_CONST_TO_SHARED_csa_block_perm
#endif

#if defined(COPY_TO_SHARED_block_sbox)
#	define COPY_CONST_TO_SHARED_block_sbox \
		memcpy(block_sbox,const_block_sbox,sizeof(uint8_t)*256);
#else
#	define COPY_CONST_TO_SHARED_block_sbox
#endif

#define COPY_CONST_TO_SHARED \
 COPY_CONST_TO_SHARED_sbox \
 COPY_CONST_TO_SHARED_csa_key_perm \
 COPY_CONST_TO_SHARED_csa_block_perm \
 COPY_CONST_TO_SHARED_block_sbox

/**
 * conditional rotation used in stream round
 */
#define csa_stream_rotate_code_block \
	{ return TSTP(pqzyx) ? ((x << 1) | ((x >> 3) & 1)) & 0xf : x; }

/**
 * process bit selection, permutation and sboxes
 */
#define csa_stream_sboxes_code_block \
{ \
  uint32_t		res; \
  uint64_t		t; \
  \
  t = A & 0x2018004200LL; \
  res =  sbox[1][ ((t >> 37) ^ (t >> 27) ^ (t >> 25) ^ (t >> 11) ^ (t >>  5)) \
		& 0x1f ]; \
 \
  t = A & 0x4201480000LL; \
  res |= sbox[4][ ((t >> 38) ^ (t >> 32) ^ (t >> 22) ^ (t >> 16) ^ (t >> 18)) \
		& 0x1f ]; \
 \
  t = A & 0x8040122000LL; \
  res |= sbox[5][ ((t >> 39) ^ (t >> 29) ^ (t >> 18) ^ (t >> 14) ^ (t >>  9)) \
		& 0x1f ]; \
 \
  t = A & 0x1082010040LL; \
  res |= sbox[0][ ((t >> 36) ^ (t >> 30) ^ (t >> 23) ^ (t >>  3) ^ (t >> 12)) \
		& 0x1f ]; \
 \
  t = A & 0x0004a00180LL; \
  res |= sbox[2][ ((t >> 26) ^ (t >> 22) ^ (t >> 19) ^ (t >>  5) ^ (t >>  3)) \
		& 0x1f ]; \
 \
  t = A & 0x0100048820LL; \
  res |= sbox[3][ ((t >> 32) ^ (t >> 17) ^ (t >>  9) ^ (t >>  2) ^ (t >> 11)) \
		& 0x1f ]; \
 \
  t = A & 0x0c20001400LL; \
  res |= sbox[6][ ((t >> 35) ^ (t >> 33) ^ (t >> 27) ^ (t >>  9) ^ (t >>  6)) \
		& 0x1f ]; \
 \
  return res; \
}

/**
 * bit selection and XOR from B
 */
#define csa_stream_B_sel_code_block \
{ \
  uint32_t	t = B >> 9; \
 \
  /* 10000000 01000010000000000001000000000000 */ \
  /* 00000010 00000001000001001000000000000000 */ \
  /* 00000100 00000000101000010000000000000000 */ \
  /* 01000001 00001000000000000010000000000000 */ \
 \
  return \
    (((t      )                         ^ (t >> 27)) & 0x8) ^ \
    (((t >> 18)                                    ) & 0x9) ^ \
    ((            (t >> 22)             ^ (t >>  7)) & 0x4) ^ \
    (((t >> 4 )                                    ) & 0x5) ^ \
    ((            (t >> 24) ^ (t >>  6) ^ (t >> 11)) & 0x2) ^ \
    (((t >> 29) ^                         (t >> 23)) & 0x1) ^ \
    (((t >> 13)                                    ) & 0xe); \
}

/**
 * cfed precomputed operations
 */
#define csa_stream_cfed_code_block \
	{ return ((cfed & 0x0f00) >> 4) | csa_stream_cdef[((cfed & 0x10ff) |  \
		(pqzyx & 0x2f00)) >> 4]; }

/**
 * stream cipher stream generation rounds
 */
#define csa_stream_round_code_block \
	{ \
		uint32_t tmp; \
		\
		*A <<= 4; \
		*A |= (NBGET(*A, 10) ^ GETX(*pqzyx)) & 0xf; \
		\
		tmp = (NBGET(*B, 6) ^	NBGET(*B, 9) ^ GETY(*pqzyx)) & 0xf; \
		\
		*B <<= 4; \
		*B |= csa_stream_rotate(*pqzyx, tmp); \
		\
		*cfed = csa_stream_cfed(*pqzyx, *cfed) ^ csa_stream_B_sel(*B); \
		\
		*pqzyx = csa_stream_sboxes(*A); \
	}

#define KeySchedule07_Xor_code_block										  \
{																			  \
	/* contains cw0..7 */													  \
	const uchar *cw = Cw8;													  \
																			  \
	ksfull[0] = 0 ^ (((cw[5] & 0x10) >> 4) | ((cw[6] & 0x40) >> 5) | ((cw[7]  \
		& 0x2) << 1) | ((cw[0] & 0x1) << 3) | (cw[1] & 0x10) |				  \
		((cw[7] & 0x10) << 1) | (cw[4] & 0x40) | ((cw[1] & 0x20) << 2));      \
	ksfull[1] = 0 ^ (((cw[2] & 0x10) >> 4) | (cw[6] & 0x2) | (cw[5] & 0x4) |  \
		((cw[7] & 0x80) >> 4) | (cw[6] & 0x10) | (cw[3] & 0x20) |			  \
		((cw[4] & 0x1) << 6) | ((cw[1] & 0x2) << 6));						  \
	ksfull[2] = 0 ^ (((cw[6] & 0x8) >> 3) | (cw[0] & 0x2) |					  \
		((cw[5] & 0x8) >> 1) | ((cw[3] & 0x80) >> 4) | ((cw[4] & 0x4) << 2)   \
		| ((cw[3] & 0x1) << 5) | ((cw[7] & 0x4) << 4) | ((cw[2] & 0x4)<<5));  \
	ksfull[3] = 0 ^ (((cw[3] & 0x2) >> 1) | ((cw[0] & 0x40) >> 5) |			  \
		((cw[2] & 0x80) >> 5) | (cw[0] & 0x8) | ((cw[7] & 0x20) >> 1)		  \
		| ((cw[3] & 0x8) << 2) | ((cw[4] & 0x10) << 2) | ((cw[3] &0x10)<<3)); \
	ksfull[4] = 0 ^ ((cw[2] & 0x1) | ((cw[5] & 0x1) << 1) |					  \
		((cw[2] & 0x20) >> 3) | ((cw[5] & 0x80) >> 4) | ((cw[4] & 0x20) >> 1) \
		| ((cw[4] & 0x80) >> 2) | ((cw[1] & 0x4) << 4) | ((cw[5] & 0x2)<<6)); \
	ksfull[5] = 0 ^ (((cw[4] & 0x8) >> 3) | ((cw[0] & 0x4) >> 1) |			  \
		(cw[6] & 0x4) | ((cw[0] & 0x10) >> 1) | ((cw[1] & 0x80) >> 3) |		  \
		((cw[6] & 0x1) << 5) | ((cw[2] & 0x8) << 3) | ((cw[3] & 0x40) << 1)); \
	ksfull[6] = 0 ^ (((cw[0] & 0x20) >> 5) | ((cw[3] & 0x4) >> 1) |			  \
		((cw[7] & 0x40) >> 4) | ((cw[2] & 0x2) << 2) | ((cw[6] & 0x20) >> 1)  \
		| ((cw[7] & 0x8) << 2) | ((cw[5] & 0x20) << 1) | ((cw[1] & 0x1)<<7)); \
	ksfull[7] = 0 ^ (((cw[4] & 0x2) >> 1) | ((cw[2] & 0x40) >> 5) |			  \
		((cw[1] & 0x8) >> 1) | ((cw[6] & 0x80) >> 4) | ((cw[7] & 0x1) << 4) | \
		((cw[1] & 0x40) >> 1) | ((cw[0] & 0x80) >> 1) | ((cw[5] & 0x40)<<1)); \
																			  \
	ksfull[8] = 1 ^ (((cw[3] & 0x80) >> 7) | ((cw[0] & 0x8) >> 2) |			  \
		((cw[1] & 0x1) << 2) | (cw[2] & 0x8) | ((cw[6] & 0x40) >> 2) |		  \
		((cw[1] & 0x2) << 4) | ((cw[4] & 0x20) << 1) | ((cw[7] & 0x4) << 5)); \
	ksfull[9] = 1 ^ (((cw[4] & 0x2) >> 1) | ((cw[1] & 0x40) >> 5) |			  \
		((cw[5] & 0x2) << 1) | ((cw[3] & 0x1) << 3) | ((cw[5] & 0x20) >> 1) | \
		((cw[1] & 0x8) << 2) | (cw[7] & 0x40) | ((cw[7] & 0x20) << 2));		  \
	ksfull[10] = 1 ^ (((cw[1] & 0x20) >> 5) | ((cw[3] & 0x8) >> 2) |		  \
		((cw[2] & 0x2) << 1) | ((cw[4] & 0x40) >> 3) | ((cw[5] & 0x1) << 4) | \
		((cw[5] & 0x80) >> 2) | ((cw[4] & 0x4) << 4) | ((cw[6] & 0x8) << 4)); \
	ksfull[11] = 1 ^ (((cw[3] & 0x40) >> 6) | ((cw[4] & 0x10) >> 3) | ((cw[2] \
		& 0x1) << 2) | ((cw[5] & 0x40) >> 3) | ((cw[5] & 0x4) << 2) | ((cw[7] \
		& 0x80) >> 2) | ((cw[1] & 0x10) << 2) | ((cw[1] & 0x4) << 5));		  \
	ksfull[12] = 1 ^ (((cw[0] & 0x80) >> 7) | ((cw[2] & 0x80) >> 6) | ((cw[4] \
		& 0x8) >> 1) | (cw[5] & 0x8) | (cw[6] & 0x10) | (cw[6] &0x20)|((cw[4] \
		& 0x80) >> 1) | ((cw[7] & 0x8) << 4));								  \
	ksfull[13] = 1 ^ (((cw[2] & 0x4) >> 2) | ((cw[6] & 0x80) >> 6) | (cw[0]   \
		& 0x4) | ((cw[0] & 0x2) << 2) | (cw[3] & 0x10) | ((cw[5] & 0x10)<<1)  \
		| (cw[0] & 0x40) | ((cw[0] & 0x10) << 3));							  \
	ksfull[14] = 1 ^ (((cw[1] & 0x80) >> 7) | ((cw[4] & 0x1) << 1) | ((cw[7]  \
		& 0x2) << 1) | ((cw[3] & 0x20) >> 2) | ((cw[6] & 0x1) << 4) | (cw[0]  \
		& 0x20) | ((cw[0] & 0x1) << 6) | ((cw[7] & 0x1) << 7));				  \
	ksfull[15] = 1 ^ (((cw[3] & 0x4) >> 2) | ((cw[2] & 0x10) >> 3) | ((cw[2]  \
		& 0x20) >> 3) | ((cw[7] & 0x10) >> 1) | ((cw[6] & 0x2) << 3)|((cw[6]  \
		& 0x4) << 3) | (cw[2] & 0x40) | ((cw[3] & 0x2) << 6));				  \
																			  \
	ksfull[16] = 2 ^ (((cw[4] & 0x40) >> 6) | ((cw[5] & 0x40) >> 5) | ((cw[7] \
		& 0x1) << 2) | ((cw[0] & 0x40) >> 3) | ((cw[0] & 0x8) << 1) | (cw[7]  \
		& 0x20) | ((cw[6] & 0x10) << 2) | ((cw[4] & 0x4) << 5));			  \
	ksfull[17] = 2 ^ (((cw[3] & 0x4) >> 2) | ((cw[6] & 0x4) >> 1) | ((cw[7]   \
		& 0x8) >> 1) | ((cw[5] & 0x80) >> 4) | ((cw[0] & 0x1) << 4) | (cw[2]  \
		& 0x20) | ((cw[7] & 0x2) << 5) | ((cw[5] & 0x4) << 5));				  \
	ksfull[18] = 2 ^ (((cw[7] & 0x4) >> 2) | ((cw[7] & 0x80) >> 6) | ((cw[3]  \
		& 0x20) >> 3) | ((cw[4] & 0x20) >> 2) | ((cw[2] & 0x80) >> 3) |		  \
		((cw[5] & 0x8) << 2) | ((cw[5] & 0x1) << 6) | ((cw[1] & 0x20) << 2)); \
	ksfull[19] = 2 ^ (((cw[0] & 0x10) >> 4) | ((cw[1] & 0x10) >> 3) | ((cw[0] \
		& 0x80) >> 5) | ((cw[3] & 0x2) << 2) | ((cw[5] & 0x2) << 3) | ((cw[3] \
		& 0x1) << 5) | (cw[6] & 0x40) | (cw[4] & 0x80));					  \
	ksfull[20] = 2 ^ (((cw[2] & 0x40) >> 6) | ((cw[2] & 0x1) << 1) | (cw[2]	  \
		& 0x4) | ((cw[2] & 0x2) << 2) | ((cw[5] & 0x20) >> 1) | ((cw[6]&0x1)  \
		<< 5) | ((cw[6] & 0x20) << 1) | ((cw[0] & 0x20) << 2));				  \
	ksfull[21] = 2 ^ (((cw[6] & 0x8) >> 3) | ((cw[7] & 0x10) >> 3) | ((cw[6]  \
		& 0x80) >> 5) | (cw[3] & 0x8) | ((cw[1] & 0x4) << 2) | ((cw[3]&0x80)  \
		>> 2) | ((cw[4] & 0x10) << 2) | ((cw[0] & 0x2) << 6));				  \
	ksfull[22] = 2 ^ (((cw[3] & 0x10) >> 4) | ((cw[7] & 0x40) >> 5) | ((cw[1] \
		& 0x1) << 2) | (cw[1] & 0x8) | (cw[5] & 0x10) | ((cw[1] & 0x80) >> 2) \
		| ((cw[2] & 0x8) << 3) | ((cw[6] & 0x2) << 6));						  \
	ksfull[23] = 2 ^ ((cw[4] & 0x1) | (cw[4] & 0x2) | ((cw[4] & 0x8) >> 1)    \
		| ((cw[1] & 0x2) << 2) | ((cw[1] & 0x40) >> 2) | ((cw[0] & 0x4) << 3) \
		| ((cw[2] & 0x10) << 2) | ((cw[3] & 0x40) << 1));					  \
																			  \
	ksfull[24] = 3 ^ (((cw[4] & 0x20) >> 5) | (cw[3] & 0x2) | ((cw[6] & 0x2)  \
		<< 1) | ((cw[4] & 0x10) >> 1) | ((cw[5] & 0x40) >> 2) | ((cw[5]&0x4)  \
		<< 3) | ((cw[5] & 0x20) << 1) | ((cw[5] & 0x1) << 7));				  \
	ksfull[25] = 3 ^ ((cw[4] & 0x1) | ((cw[0] & 0x4) >> 1) | ((cw[0] & 0x20)  \
		>> 3) | (cw[5] & 0x8) | ((cw[2] & 0x8) << 1) | ((cw[4] & 0x8) << 2)	  \
		| ((cw[1] & 0x1) << 6) | ((cw[5] & 0x2) << 6));						  \
	ksfull[26] = 3 ^ (((cw[4] & 0x4) >> 2) | ((cw[3] & 0x1) << 1) | ((cw[1]   \
		& 0x8) >> 1) | ((cw[6] & 0x10) >> 1) | ((cw[2] & 0x1) << 4) | ((cw[2] \
		& 0x2) << 4) | ((cw[2] & 0x80) >> 1) | ((cw[7] & 0x4) << 5));		  \
	ksfull[27] = 3 ^ (((cw[0] & 0x2) >> 1) | ((cw[6] & 0x40) >> 5) | ((cw[2]  \
		& 0x40) >> 4) | ((cw[3] & 0x40) >> 3) | ((cw[7] & 0x8) << 1) |		  \
		((cw[5] & 0x80) >> 2) | ((cw[0] & 0x8) << 3) | ((cw[6] & 0x20) <<2)); \
	ksfull[28] = 3 ^ (((cw[2] & 0x10) >> 4) | ((cw[0] & 0x80) >> 6) | ((cw[6] \
		& 0x8) >> 1) | ((cw[3] & 0x20) >> 2) | ((cw[0] & 0x1) << 4) | ((cw[5] \
		& 0x10) << 1) | ((cw[6] & 0x1) << 6) | (cw[1] & 0x80));				  \
	ksfull[29] = 3 ^ (((cw[1] & 0x20) >> 5) | (cw[1] & 0x2) | ((cw[7] & 0x10) \
		>> 2) | ((cw[7] & 0x80) >> 4) | ((cw[4] & 0x80) >> 3) | ((cw[4] &	  \
		0x40) >> 1) | ((cw[1] & 0x10) << 2) | ((cw[3] & 0x8) << 4));		  \
	ksfull[30] = 3 ^ (((cw[1] & 0x4) >> 2) | (cw[7] & 0x2) | ((cw[7] & 0x1)	  \
		<< 2) | ((cw[2] & 0x20) >> 2) | ((cw[3] & 0x80) >> 3) |				  \
		((cw[3] & 0x10) << 1) | (cw[0] & 0x40) | ((cw[1] & 0x40) << 1));	  \
	ksfull[31] = 3 ^ (((cw[7] & 0x40) >> 6) | ((cw[3] & 0x4) >> 1) | (cw[2]	  \
		& 0x4) | ((cw[7] & 0x20) >> 2) | ((cw[6] & 0x4) << 2) | ((cw[6] &	  \
		0x80) >> 2) | ((cw[4] & 0x2) << 5) | ((cw[0] & 0x10) << 3));		  \
																			  \
	ksfull[32] = 4 ^ (((cw[6] & 0x10) >> 4) | ((cw[3] & 0x40) >> 5) | ((cw[1] \
		&0x40) >> 4) | ((cw[1] & 0x10) >> 1) | ((cw[3] & 0x2) << 3) | ((cw[5] \
		&0x2) << 4) | ((cw[0] & 0x1) << 6) | (cw[2] & 0x80));				  \
	ksfull[33] = 4 ^ (((cw[7] & 0x40) >> 6) | ((cw[6] & 0x80) >> 6) | ((cw[1] \
		& 0x80) >> 5) | ((cw[2] & 0x2) << 2) | ((cw[0] & 0x40) >> 2)|((cw[2]  \
		& 0x4) << 3) | ((cw[7] & 0x1) << 6) | ((cw[7] & 0x8) << 4));		  \
	ksfull[34] = 4 ^ ((cw[5] & 0x1) | ((cw[5] & 0x80) >> 6) | ((cw[2] & 0x20) \
		>> 3) | ((cw[5] & 0x20) >> 2) | ((cw[0] & 0x80) >> 3) | (cw[3]&0x20)  \
		| ((cw[2] & 0x1) << 6) | ((cw[4] & 0x4) << 5));						  \
	ksfull[35] = 4 ^ (((cw[3] & 0x8) >> 3) | ((cw[0] & 0x8) >> 2) | ((cw[2] & \
		0x10) >> 2) | ((cw[0] & 0x10) >> 1) | ((cw[0] & 0x20) >> 1) | ((cw[5] \
		& 0x8) << 2) | (cw[5] & 0x40) | ((cw[6] & 0x1) << 7));				  \
	ksfull[36] = 4 ^ (((cw[4] & 0x2) >> 1) | ((cw[2] & 0x40) >> 5) | ((cw[1]  \
		& 0x20) >> 3) | (cw[1] & 0x8) | ((cw[2] & 0x8) << 1) | ((cw[3]&0x80)  \
		>> 2) | ((cw[5] & 0x10) << 2) | ((cw[3] & 0x10) << 3));				  \
	ksfull[37] = 4 ^ (((cw[7] & 0x4) >> 2) | ((cw[7] & 0x20) >> 4) | ((cw[1]  \
		& 0x2) << 1) | ((cw[3] & 0x1) << 3) | ((cw[6] & 0x20) >> 1) | (cw[4]  \
		& 0x20) | (cw[6] & 0x40) | (cw[7] & 0x80));							  \
	ksfull[38] = 4 ^ (((cw[4] & 0x80) >> 7) | ((cw[1] & 0x1) << 1) | ((cw[6]  \
		& 0x2) << 1) | (cw[4] & 0x8) | ((cw[4] & 0x40) >> 2) | ((cw[1] & 0x4) \
		<< 3) | ((cw[4] & 0x10) << 2) | ((cw[6] & 0x4) << 5));				  \
	ksfull[39] = 4 ^ (((cw[7] & 0x2) >> 1) | ((cw[4] & 0x1) << 1) | ((cw[6]   \
		& 0x8) >> 1) | ((cw[5] & 0x4) << 1) | ((cw[0] & 0x4) << 2) | ((cw[7]  \
		& 0x10) << 1) | ((cw[3] & 0x4) << 4) | ((cw[0] & 0x2) << 6));		  \
																			  \
	ksfull[40] = 5 ^ (((cw[5] & 0x20) >> 5) | ((cw[0] & 0x10) >> 3) | (cw[6]  \
		&0x4) | ((cw[6] & 0x40) >> 3) | ((cw[3] & 0x40) >> 2) | ((cw[7] &0x8) \
		<< 2) | ((cw[2] & 0x8) << 3) | ((cw[2] & 0x1) << 7));				  \
	ksfull[41] = 5 ^ (((cw[7] & 0x2) >> 1) | ((cw[7] & 0x10) >> 3) | ((cw[3]  \
		&0x10) >> 2) | ((cw[3] & 0x20) >> 2) | (cw[4] & 0x10) | ((cw[6] &0x8) \
		<< 2) | ((cw[6] & 0x2) << 5) | ((cw[0] & 0x20) << 2));				  \
	ksfull[42] = 5 ^ (((cw[2] & 0x80) >> 7) | ((cw[5] & 0x8) >> 2) | ((cw[4]  \
		& 0x8) >> 1) | ((cw[0] & 0x1) << 3) | ((cw[2] & 0x40) >> 2) | ((cw[1] \
		& 0x8) << 2) | ((cw[0] & 0x80) >> 1) | ((cw[5] & 0x1) << 7));		  \
	ksfull[43] = 5 ^ (((cw[7] & 0x80) >> 7) | ((cw[5] & 0x40) >> 5) | ((cw[4] \
		& 0x2) << 1) | ((cw[0] & 0x2) << 2) | ((cw[1] & 0x80) >> 3) | ((cw[2] \
		& 0x2) << 4) | ((cw[3] & 0x2) << 5) | ((cw[5] & 0x10) << 3));		  \
	ksfull[44] = 5 ^ (((cw[3] & 0x4) >> 2) | ((cw[2] & 0x10) >> 3) | (cw[7]	  \
		& 0x4) | ((cw[2] & 0x20) >> 2) | ((cw[0] & 0x40) >> 2) | ((cw[4]	  \
		& 0x40) >> 1) | ((cw[3] & 0x80) >> 1) | ((cw[1] & 0x4) << 5));		  \
	ksfull[45] = 5 ^ (((cw[4] & 0x4) >> 2) | ((cw[5] & 0x4) >> 1) | ((cw[7]	  \
		&0x20) >> 3) | ((cw[5] & 0x80) >> 4) | ((cw[6] & 0x1) << 4) | ((cw[6] \
		& 0x10) << 1) | ((cw[0] & 0x8) << 3) | ((cw[3] & 0x1) << 7));		  \
	ksfull[46] = 5 ^ (((cw[6] & 0x20) >> 5) | ((cw[7] & 0x1) << 1) | ((cw[1]  \
		& 0x40) >> 4) | ((cw[2] & 0x4) << 1) | ((cw[4] & 0x20) >> 1) |((cw[4] \
		& 0x80) >> 2) | ((cw[1] & 0x10) << 2) | ((cw[0] & 0x4) << 5));		  \
	ksfull[47] = 5 ^ ((cw[1] & 0x1) | ((cw[7] & 0x40) >> 5) | ((cw[1] & 0x20) \
		>> 3) | ((cw[5] & 0x2) << 2) | ((cw[6] & 0x80) >> 3) | ((cw[1] & 0x2) \
		<< 4) | ((cw[4] & 0x1) << 6) | ((cw[3] & 0x8) << 4));				  \
																			  \
	ksfull[48] = 6 ^ cw[0]; ksfull[49] = 6 ^ cw[1]; ksfull[50] = 6 ^ cw[2];	  \
	ksfull[51] = 6 ^ cw[3]; ksfull[52] = 6 ^ cw[4]; ksfull[53] = 6 ^ cw[5];	  \
	ksfull[54] = 6 ^ cw[6]; ksfull[55] = 6 ^ cw[7];							  \
}

#define block_encrypt_dataOnly_ulong_code_block \
	{ \
		uint i=0; \
		/* #pragma unroll 2 */ \
		_Pragma("unroll 2") \
		while (i < DVBCSA_KEYSBUFF_SIZE) \
		{ \
			ulong	L; \
			ulong	S; \
	 \
			S = block_sbox[key[i] ^ data[7]]; \
	 \
			L = data[1]; \
	 \
			data[1] = data[2] ^ data[0]; \
			data[2] = data[3] ^ data[0]; \
			data[3] = data[4] ^ data[0]; \
			data[4] = data[5]; \
			data[5] = data[6] ^ csa_block_perm[S]; \
			data[6] = data[7]; \
			data[7] = data[0] ^ S; \
	 \
			data[0] = L; \
	 \
			++i; \
		} \
	}

#endif // PCRTT_SRC_SHARED_DEFINE_HEADER_H_
