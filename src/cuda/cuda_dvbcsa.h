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

    Parallel bitslice implementation based on FFdecsa,
     Copyright (C) 2003-2004 fatih89r

    (c) 2006-2008 Alexandre Becoulet <alexandre.becoulet@free.fr>

*/
#ifndef LIBDVBCSA_H_
#define LIBDVBCSA_H_

#include "cuda.h"

#ifdef __cplusplus
extern "C" 
{
#endif

/* csa control word */
typedef unsigned char		dvbcsa_cw_t[8];

/***********************************************************************
	Single packet CSA implemetation API
 */

/* single packet implementation key context */
typedef struct dvbcsa_key_s	dvbcsa_key_t;

/** allocate a new csa key context */
__device__
struct dvbcsa_key_s * my_dvbcsa_key_alloc();

/** free a csa key context */

__device__
void my_dvbcsa_key_free(struct dvbcsa_key_s *key);

/** setup a csa key context to use the given control word */

__device__
void my_dvbcsa_key_set (const dvbcsa_cw_t cw, struct dvbcsa_key_s *key);

/** decrypt a packet payload */

__device__
void my_dvbcsa_decrypt_FirstBlockOnly (const struct dvbcsa_key_s *key, unsigned char *data);
__device__
void my_dvbcsa_decrypt_FirstBlockOnly_Flat(const uchar *Cw8, int LastCwByte, unsigned char *BitArrayOkOut_d);

__device__
void my_dvbcsa_decrypt (const struct dvbcsa_key_s *key, unsigned char *data, unsigned int len);
void my_dvbcsa_decrypt2(const struct dvbcsa_key_s *key, uchar *SameCntOut, DEBUG_STRUCT *Debug);

/** encrypt a packet payload */

__device__
void my_dvbcsa_encrypt (const struct dvbcsa_key_s *key,
		     unsigned char *data, unsigned int len);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBDVBCSA_H_
