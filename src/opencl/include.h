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
#if !defined(PCRTT_SRC_OPENCL_INCLUDE_HEADER_H_)
#define PCRTT_SRC_OPENCL_INCLUDE_HEADER_H_

#ifdef __cplusplus

#include <shared/define.h>

namespace PCRTT {
namespace OpenCL {

#define KEYS_PER_THREAD 0x100
#define KEYS_PER_KERNEL (BLOCKS*THREADS*KEYS_PER_THREAD)
#define MAX_KEY_HITS_PER_KERNEL 100 
//first 4 bytes as int / rest for cw8 matches
#define MAX_ODATA_SIZE (4+(8*(MAX_KEY_HITS_PER_KERNEL)))
#define COPY_TO_SHARED_csa_key_perm
#define COPY_TO_SHARED_csa_block_perm
#define COPY_TO_SHARED_my_dvbcsa_block_sbox
#define RAINBOW_CHAIN_LEN 0x10000
#define RAINBOW_CHAIN_LEN_PER_KERNEL 0x100
#define BLOCKS_RB 0x200
#define THREADS_RB 0x80

int init();
int destroy();

int CsaRainbow1(unsigned char *Cw6EndArray_h, const unsigned char *Cw6StartArray_h,
	ulong StartRoundNr);

int CsaRainbowDoXRounds(unsigned char *Cw6Array_h, unsigned long *CurRoundNrArray_h,
	ulong *RemRoundsArray_h);

int CsaRainbowFindA(unsigned char *Cw6EndArray_h, unsigned long *RoundNrArray_h,
	unsigned char *CryptArray_h);

} // end of namespace OpenCL
} // end of namespace PCRTT

#endif // __cplusplus

#endif // PCRTT_SRC_OPENCL_INCLUDE_HEADER_H_
