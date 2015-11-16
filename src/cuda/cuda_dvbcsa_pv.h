/*

    This file is part of libdvbcsa.

    libdvbcsa is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 2 of the License,
    or (at your option) any later version.

    libdvbcsa is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdvbcsa; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

    (c) 2006-2008 Alexandre Becoulet <alexandre.becoulet@free.fr>

*/
#ifndef DVBCSA_PV_H_
# define DVBCSA_PV_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "config.h"

#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#if HAVE_STRINGS_H
# include <strings.h>
#endif
#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#if !defined(DVBCSA_DEBUG) && defined(__GNUC__)
#define DVBCSA_INLINE __attribute__ ((always_inline))
#else
#define DVBCSA_INLINE
#endif

void worddump (const char *str, const void *data, my_size_t len, my_size_t ws);

#define DVBCSA_DATA_SIZE	8
#define DVBCSA_KEYSBUFF_SIZE	56

typedef uint8_t			dvbcsa_block_t[DVBCSA_DATA_SIZE];
typedef uint8_t			dvbcsa_keys_t[DVBCSA_KEYSBUFF_SIZE];

__device__
struct dvbcsa_key_s
{
  dvbcsa_cw_t		cw;
  dvbcsa_cw_t		cws;	/* nibble swapped CW */
  dvbcsa_keys_t		sch;
};

//__device__
//extern const uint8_t my_dvbcsa_block_sbox[256];

__device__
void my_dvbcsa_block_decrypt (const dvbcsa_keys_t key, const dvbcsa_block_t in, dvbcsa_block_t out);
__device__
void my_dvbcsa_block_encrypt (const dvbcsa_keys_t key, const dvbcsa_block_t in, dvbcsa_block_t out);
__device__
void my_dvbcsa_block_encrypt_data (const dvbcsa_keys_t key, uchar *data);
__device__
void my_dvbcsa_block_encrypt_data_ulong (const ulong *key, ulong *data);
__device__
void my_dvbcsa_block_encrypt_data_sharedKsfull(const dvbcsa_keys_t key, uchar *data);
__device__
void my_dvbcsa_block_encrypt_data_sharedKsfull_ulong(const ulong *key, ulong *data);
__device__
void my_dvbcsa_block_encrypt_dataOnlyDword(const dvbcsa_keys_t key, ulong *data);

__device__
void my_dvbcsa_stream_xor (const dvbcsa_cw_t cw, const dvbcsa_block_t iv,
			uint8_t *stream, unsigned int len);

__device__
DVBCSA_INLINE static my_inline void 
my_dvbcsa_xor_64 (uint8_t *b, const uint8_t *a)
{
#if defined(__i386__) || defined(__x86_64__)
  /* target support non aligned memory access */
  *(uint64_t*)b ^= *(uint64_t*)a;
#else
  unsigned int i;

  for (i = 0; i < 8; i++)
    b[i] ^= a[i];
#endif
}

__device__
DVBCSA_INLINE static my_inline uint32_t
my_dvbcsa_load_le32(const uint8_t *p)
{
#if defined(__i386__) || defined(__x86_64__)
  /* target support non aligned le memory access */
  return *(uint32_t*)p;
#else
  return ((uint32_t)p[3] << 24) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[1] << 8 ) |
          (uint32_t)p[0];
#endif
}

__device__
DVBCSA_INLINE static my_inline uint64_t
my_dvbcsa_load_le64(const uint8_t *p)
{
#if defined(__i386__) || defined(__x86_64__)
  /* target support non aligned le memory access */
  return *(uint64_t*)p;
#else
  return (uint64_t)( ((uint64_t)p[7] << 56) |
		     ((uint64_t)p[6] << 48) |
		     ((uint64_t)p[5] << 40) |
		     ((uint64_t)p[4] << 32) |
		     ((uint64_t)p[3] << 24) |
		     ((uint64_t)p[2] << 16) |
		     ((uint64_t)p[1] << 8 ) |
		      (uint64_t)p[0]
		     );
#endif
}

__device__
DVBCSA_INLINE static my_inline void
dvbcsa_store_le32(uint8_t *p, const uint32_t w)
{
#if defined(__i386__) || defined(__x86_64__)
  /* target support non aligned le memory access */
  *(uint32_t*)p = w;
#else
  p[3] = (w >> 24);
  p[2] = (w >> 16);
  p[1] = (w >> 8);
  p[0] = (w);
#endif
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DVBCSA_PV_H_


