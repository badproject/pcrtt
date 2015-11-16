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
#if !defined(PCRTT_SRC_CUDA_CUDA_HEADER_H_)
#define PCRTT_SRC_CUDA_CUDA_HEADER_H_

#include "../core/types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct _DEBUG_STRUCT { uchar Dummy; } DEBUG_STRUCT;

#define KEYS_PER_THREAD 0x100

#define KEYS_PER_KERNEL (BLOCKS * THREADS * KEYS_PER_THREAD)

#define MAX_KEY_HITS_PER_KERNEL 100 

// first 4 bytes as int / rest for cw8 matches
#define MAX_ODATA_SIZE (4+(8*(MAX_KEY_HITS_PER_KERNEL)))

//#define COPY_TO_SHARED_csa_key_perm
//#define COPY_TO_SHARED_csa_block_perm
#define COPY_TO_SHARED_block_sbox

// copy of ../shared/define.h
#define RAINBOW_CHAIN_LEN 0x10000
#define RAINBOW_CHAIN_LEN_PER_KERNEL 0x100

#define BLOCKS_RB 0x200
#define THREADS_RB 0x80

int CsaCudaRainbow1(uchar *Cw6EndArray_h, const uchar *Cw6StartArray_h,
	ulong StartRoundNr, DEBUG_STRUCT *Debug_h);
int CsaCudaRainbowFindA(uchar *Cw6EndArray_h, ulong *RoundNrArray_h,
	uchar *CryptArray_h, DEBUG_STRUCT *Debug_h);
int CsaCudaRainbowDoXRounds(uchar *Cw6Array_h, ulong *CurRoundNrArray_h,
	ulong *RemRoundsArray_h, DEBUG_STRUCT *Debug_h);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PCRTT_SRC_CUDA_CUDA_HEADER_H_
