/**
 * Posix compliant version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2015 NEngine Team
 * 
 * This file is part of CSA-Rainbow-Table-Tool.
 * 
 * CSA-Rainbow-Table-Tool is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSA-Rainbow-Table-Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSA-Rainbow-Table-Tool.
 * If not, see <http://www.gnu.org/licenses/>.
 * 
 * This software is based on the windows version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2012  Colibri <colibri.dvb@googlemail.com>  
 * http://colibri.net63.net/ alias http://colibri-dvb.info/
 * 
 */
#include <config.h>

#include <stdint.h>
#include <string.h> // memcpy
#include <strings.h> // ffs

#if defined(__MMX__) || (defined(__SSE__) || defined(__SSE2__))
#include <mmintrin.h>
#endif	// __MMX__

#if defined(__3DNOW__)
#include <mm3dnow.h>
#endif	// __MMX__

#if defined(__SSE__) || defined(__SSE2__)
#include <xmmintrin.h>
#endif	// __SSE__

#if defined(__SSE2__)
#include <emmintrin.h>
#endif	// __SSE2__

#if defined(__SSE3__)
#include <pmmintrin.h>
#endif	// __SSE3__

static void* _op_memcpy_c(void *a, const void *b, unsigned int c)
{ return memcpy(a,b,c); }

static void* _op_memset_c(void *a, int b, unsigned int c)
{ return memset(a,b,c); }

static int _op_memcmp_c(const void* a, const void* b, unsigned int c)
{ return memcmp(a,b,c); }

void* (*_op_memset_)(void *dest, int c, unsigned int size) = 
 _op_memset_c;

/**
 * courtesy of :
 * https://mischasan.wordpress.com/2013/02/13/sse2-beats-sse4-2-in-memcmp/
 */

#if defined(HAS_CORE_I7) && defined(__SSE42__)

#if 0
int CopyMemSSE4(int* piDst, int* piSrc, unsigned long SizeInBytes)
{
	// Initialize pointers to start of the USWC memory

	_asm
	{
		mov esi, piSrc
		mov edx, piSrc

		// Initialize pointer to end of the USWC memory
		add edx, SizeInBytes

		// Initialize pointer to start of the cacheable WB buffer
		mov edi, piDst

		// Start of Bulk Load loop
		inner_start:
		// Load data from USWC Memory using Streaming Load
		MOVNTDQA xmm0, xmmword ptr [esi]
		MOVNTDQA xmm1, xmmword ptr [esi+16]
		MOVNTDQA xmm2, xmmword ptr [esi+32]
		MOVNTDQA xmm3, xmmword ptr [esi+48]

		// Copy data to buffer
		MOVDQA xmmword ptr [edi], xmm0
		MOVDQA xmmword ptr [edi+16], xmm1
		MOVDQA xmmword ptr [edi+32], xmm2
		MOVDQA xmmword ptr [edi+48], xmm3

		// Increment pointers by cache line size and test for end of loop
		add esi, 040h
		add edi, 040h
		cmp esi, edx
		jne inner_start
	}
	// End of Bulk Load loop

	return 0;
}
#endif // 0

typedef __m128i XMM;

static inline XMM xmload(void const*p)
{ return _mm_load_si128((XMM const*)p); }

static inline XMM xmloud(void const*p)
{ return (XMM)_mm_loadu_pd((double const*)p); }

static inline unsigned xmdiff(XMM a, XMM b)
{ return 0xFFFF ^ _mm_movemask_epi8(_mm_cmpeq_epi8(a, b)); }

static inline int cmp(int mask, uint8_t const*src, uint8_t const*dst)
{ return (mask = ffs(mask) - 1) < 0 ? 0 : (int)src[mask] - dst[mask]; }

static int xmcmp(const void* _src, const void* _dst, unsigned int len)
{
	uint8_t const *src = (uint8_t const*)_src;
	uint8_t const *dst = (uint8_t const*)_dst;
	int ret, srcoff = 15 & (intptr_t)src;

	if (srcoff) {
		if ((ret = xmdiff(xmloud(src), xmloud(dst))))
			return cmp(ret, src, dst);

		src += 16 - srcoff;
		dst += 16 - srcoff;
		len -= 16 - srcoff;
	}

	for (; len > 15; src += 16, dst += 16, len -=16) {
		_mm_prefetch(src+512-64, _MM_HINT_NTA);
		_mm_prefetch(dst+512-64, _MM_HINT_NTA);
		if ((ret = xmdiff(xmload(src), xmloud(dst))))
			return cmp(ret, src, dst);
	}

	ret = xmdiff(xmloud(src), xmloud(dst)) & ~(-1 << len);
	return cmp(ret,  src, dst);
}

int (*_op_memcmp_)(const void* src, const void* dst, unsigned int len) = 
 xmcmp;

#elif defined(HAS_64BIT)

// from http://www.picklingtools.com/study.pdf

/* So we  have  sized  integers  */
int fast_memcmp8( const void* s1, const void* s2, unsigned int len ) 
{
	if (s1==s2) return 0;

	/*  optimization  */
#if defined(INTS_NEED_ALIGNED )
	/* Check and see if we can do "memory  aligned" instructions :
	** This  is where we get the  speed: most  architectures  are MUCH
	** faster  when  the do  aligned  memory  accesses  */
	intptr_t  lp = (intptr_t)s1;
	intptr_t  rp = (intptr_t)s2;

	if ((lp | rp) & 0x3 )
	{
		/**
		 * Nope , bottom  three (two) bits  set on one of the two, SIGH: use
		 * memcmp
		 */
		return memcmp( s1, s2, len );
	}
#endif

	/**
	 * Assertion: pointers  are aligned , can do  compares  ACROSS 8 bytes
	 */
	uint64_t  *li = (uint64_t*)s1;
	uint64_t  *ri = (uint64_t*)s2;
	int  greater_len  = len >>3;

	// large  granularity : 8 byte  chunks
	int  lesser_len   = len & (0x7);

	// small  granularity : <4 bytes

	/* Major pass , 8 bytes at a time  */
	int ii;
	for (ii=0; ii<greater_len ; ii++)
	{
		uint64_t  lc = *li++;
		uint64_t  rc = *ri++;
		if (lc != rc)
		{
#if __BYTE_ORDER  ==  __BIG_ENDIAN
			return lc < rc ? -1 : 1;
#else
			li -=1; ri -=1;
			/* back  up and  address  where the  problem  was */
			lesser_len  = 8;
			/*  Force to  look  at next 8 bytes to find  problem  */
			break;
#endif
		}
	}
	/* Minor pass , last  1..7  bytes */
	unsigned char *l = (unsigned char*)li;
	unsigned char *r = (unsigned char*)ri;

	for (ii=0; ii<lesser_len ; ii++)
	{
		unsigned  char  lc = *l++;
		unsigned  char  rc = *r++;
		if (lc != rc) {
			return lc < rc ? -1 : 1;
		}
	}

	/* Made  it all the way: yep, they  are  same  */
	return 0;
}

static void* SSE_memcpy(void *dest, const void *src, uint nbytes)
{
#if defined( __LINUX__ ) && defined( __SSE__ )

	uchar* srcp = (uchar*)src;
	uchar* dstp = (uchar*)dest;
	uint i;
	for(i=0; i<nbytes/16; ++i)
	{
		__asm__ __volatile__ (
			"movups (%0), %%xmm0\n"
			"movntdq %%xmm0, (%1)\n"
			:
			: "r"(srcp), "r"(dstp)
			: "memory"
		);

		srcp += 16;
		dstp += 16;
	}

	if(nbytes & 7)
	{
		nbytes = nbytes&7;

		int d0, d1, d2;
		__asm__ __volatile__(
			"rep\n"
			"movsl\n"
			"testb $2,%b4\n"
			"je 1f\n"
			"movsw\n"
			"1:\n"
			"testb $1,%b4\n"
			"je 2f\n"
			"movsb\n"
			"2:"
			: "=&c" (d0), "=&D" (d1), "=&S" (d2)
			: "0" (nbytes/4), "q" (nbytes),"1" ((long) dest),"2" ((long) src)
			: "memory"
		);
	}
	return (dest);
#else
	return ::memcpy(dest,src,nbytes);
#endif
}

void* (*_op_memcpy_)(void *dest, const void *src, unsigned int nbytes) =
 SSE_memcpy;

int (*_op_memcmp_)(const void* src, const void* dst, unsigned int len) = 
 fast_memcmp8;

#else

void* (*_op_memcpy_)(void *dest, const void *src, unsigned int nbytes) =
 _op_memcpy_c;
 
int (*_op_memcmp_)(const void* src, const void* dst, unsigned int len) = 
 _op_memcmp_c;

#endif // HAS_CORE_I7 / __SSE42__
