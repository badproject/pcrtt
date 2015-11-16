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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <cuda_runtime.h>
#include <device_functions.h>
#include <device_launch_parameters.h> 
#include <assert.h>
//#include "cuda_dvbcsa.h"
//#include "cuda_dvbcsa_pv.h"
#include "cuda.h"

#include "../shared/define.h"
#include "../shared/table.h"

/*#ifdef COPY_TO_SHARED_csa_stream_cdef
__shared__ uint16_t csa_stream_cdef[0x400];
__constant__ const uint16_t const_csa_stream_cdef[0x400] = 
#else
__constant__ const uint16_t csa_stream_cdef[0x400] = 
#endif
TABLE_CSA_STREAM_CDEF*/

/*#ifdef COPY_TO_SHARED_sbox
__shared__ uint16_t sbox[7][32];
__constant__ const uint16_t const_sbox[7][32] = 
#else
__constant__ const uint16_t sbox[7][32] = 
#endif
TABLE_SBOX*/

//#ifdef COPY_TO_SHARED_csa_stream_out
//__shared__  uint8_t csa_stream_out[16];
//__constant__  const uint8_t const_csa_stream_out[16] = 
//#else
//__constant__  const uint8_t csa_stream_out[16] = 
//#endif
//TABLE_CSA_STREAM_OUT

//#ifdef COPY_TO_SHARED_csa_key_perm
//__device__ __shared__ static uint8_t csa_key_perm[64];
//__device__ __constant__ static const uint8_t const_csa_key_perm[64] = 
//#else
//__device__ __constant__ static const uint8_t csa_key_perm[64] = 
//#endif
//TABLE_CSA_KEY_PERM

#ifdef COPY_TO_SHARED_csa_block_perm
__device__	__shared__ static uint8_t csa_block_perm[256];
__device__	__constant__ static const uint8_t const_csa_block_perm[256] = 
#else
__device__	__constant__ static const uint8_t csa_block_perm[256] = 
#endif
TABLE_CSA_BLOCK_PERM

#ifdef COPY_TO_SHARED_block_sbox
__device__ __shared__ uint8_t block_sbox[256];
__device__ __constant__ const uint8_t const_block_sbox[256] = 
#else
__device__ __constant__ const uint8_t block_sbox[256] = 
#endif
TABLE_BLOCK_SBOX

// CUDA kernel to pause for at least num_cycle cycles
__global__ void sleep(int64_t num_cycles)
{
    int64_t cycles = 0;
    int64_t start = clock64();
    while(cycles < num_cycles) {
        cycles = clock64() - start;
    }
}

// Returns number of cycles required for requested seconds
static int64_t get_cycles(float seconds)
{
    // Get device frequency in KHz
    int64_t Hz;
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    Hz = int64_t(prop.clockRate) * 1000;

    // Calculate number of cycles to wait
    int64_t num_cycles;
    num_cycles = (int64_t)(seconds * Hz);
  
    return num_cycles;
}

/*__device__ static uint8_t csa_stream_rotate(register uint32_t pqzyx,
	register uint32_t x)
csa_stream_rotate_code_block*/

/*__device__ static uint32_t csa_stream_sboxes(register uint64_t A)
csa_stream_sboxes_code_block*/

/*__device__ static uint32_t csa_stream_B_sel(register uint64_t B)
csa_stream_B_sel_code_block*/

/*__device__ static p_inline uint32_t csa_stream_cfed(uint32_t pqzyx,
	uint32_t cfed)
csa_stream_cfed_code_block*/

/*__device__ static void csa_stream_round(uint64_t *A,uint64_t* B,
	uint32_t* pqzyx, uint32_t *cfed)
csa_stream_round_code_block*/

__device__ void KeySchedule07_Xor(uchar *ksfull, const uchar *Cw8)
KeySchedule07_Xor_code_block

__device__ void block_encrypt_dataOnly_ulong(const dvbcsa_keys_t key,
	ulong *data)
block_encrypt_dataOnly_ulong_code_block


/** variante 1 */
__global__ void kernel_csa_rainbow_do_x_rounds(uchar *Cw6Array_d,
	ulong *CurRoundNrArray_d, ulong *RemRoundsArray_d, DEBUG_STRUCT *Debug_d)
{
	/**
	 * Get Cw6Array for single crypt value to cmp with rainbow table end values
	 * if _all_ values in RemRoundsArray_d is 0 then Cw6Array_d has the final
	 * value else the kernel must called again
	 */
	uchar Cw[8];
	//ulong dwCw;
	int Offset;
	Offset = (blockIdx.x * THREADS_RB) + threadIdx.x;

	if((BLOCKS_RB * THREADS_RB) != RAINBOW_CHAIN_LEN)
	{
		return;//error
	}

	// optimize by copy const to shared (for all treads)
	// till sync treads

	COPY_CONST_TO_SHARED

	/** sync */
	__syncthreads();

	uchar *ParCw6;
	ParCw6 = &Cw6Array_d[Offset * 6];
	ulong *ParCurRoundNr;
	ParCurRoundNr = &CurRoundNrArray_d[Offset];
	ulong *ParRemRounds;
	ParRemRounds = &RemRoundsArray_d[Offset];

	int i;
//	int j;
	int m;
//	int k;
	int RoundNr;

	ulong data[8];

#if 1

	for(m=0; m<RAINBOW_CHAIN_LEN_PER_KERNEL; m++)
	{
		RoundNr = *ParCurRoundNr;
		if(*ParRemRounds <= 0)
		{
			break;
		}

		//copy
		for(i=0; i<8; i++)
		{
			data[i] = ParCw6[i];
		}

		//checksum
		Cw[0] = data[0];
		Cw[1] = data[1];
		Cw[2] = data[2];
		Cw[3] = data[0] + data[1] + data[2];
		Cw[4] = data[3];
		Cw[5] = data[4];
		Cw[6] = data[5];
		Cw[7] = data[3] + data[4] + data[5];

		{
			uchar ksfull[7*8];
			KeySchedule07_Xor(ksfull, Cw);

			//plain input
			for(i=0; i<8; i++)
			{
				data[i] = 0;
			}

			//Enc
			for(i=0; i<23; i++)
			{
				block_encrypt_dataOnly_ulong(ksfull, data);
			}
		}

		//Reduction part 2 (XOR RoundNr)
		data[2] ^= ((RoundNr>>24)&0xFF);
		data[3] ^= ((RoundNr>>16)&0xFF);
		data[4] ^= ((RoundNr>>8)&0xFF);
		data[5] ^= (RoundNr&0xFF);


		*ParCurRoundNr = *ParCurRoundNr + 1;
		*ParRemRounds = *ParRemRounds - 1;

		//copy cw end
		for(i=0; i<6; i++)
		{
			ParCw6[i] = data[i];
		}

	}
#endif
}

/**  variante 1 */
__global__ void kernel_csa_rainbow_find_a(uchar *Cw6EndArray_d,
	ulong *RoundNrArray_d, uchar *CryptArray_d, DEBUG_STRUCT *Debug_d)
{
	/**
	 * Get Cw6Array for single crypt value to cmp with rainbow table end values
	 * if _all_ values in RoundNrArray is set to the last round nr then
	 * Cw6Array_d contains the Cw6 _end_ values
	 * else the kernel must called again
	 * input (first call):		RoundNrArray_d, CryptArray_d
	 * input (other calls):		Cw6EndArray_d,	RoundNrArray_d, CryptArray_d
	 * output (not last call):	Cw6EndArray_d,	RoundNrArray_d, CryptArray_d
	 * output (last call):		Cw6EndArray_d
	 */
	uchar Cw[8];
	//ulong dwCw;
	int Offset;
	Offset = (blockIdx.x * THREADS_RB) + threadIdx.x;

	if((BLOCKS_RB * THREADS_RB) != RAINBOW_CHAIN_LEN)
	{
		return;//error
	}

	// optimize by copy const to shared (for all treads)
	// till sync treads

	COPY_CONST_TO_SHARED

	/** sync */
	__syncthreads();

	uchar *ParCrypt;
	ParCrypt = &CryptArray_d[Offset * 8];
	ulong *ParRoundNr;
	ParRoundNr = &RoundNrArray_d[Offset];
	uchar *ParCw6End;
	ParCw6End = &Cw6EndArray_d[Offset * 6];

	int i;
//	int k;
//	int j;
	int m;
	int RoundNr;

	ulong data[8];

	for(m=0; m<RAINBOW_CHAIN_LEN_PER_KERNEL; m++)
	{
		RoundNr = *ParRoundNr;
		if(RoundNr >= (BLOCKS_RB * THREADS_RB))
		{
			break;
		}

		//copy crypt
		for(i=0; i<8; i++)
		{
			data[i] = ParCrypt[i];
		}

		//Reduction part 2 (XOR RoundNr)
		data[2] ^= ((RoundNr>>24)&0xFF);
		data[3] ^= ((RoundNr>>16)&0xFF);
		data[4] ^= ((RoundNr>>8)&0xFF);
		data[5] ^= (RoundNr&0xFF);

		//checksum
		Cw[0] = data[0];
		Cw[1] = data[1];
		Cw[2] = data[2];
		Cw[3] = data[0] + data[1] + data[2];
		Cw[4] = data[3];
		Cw[5] = data[4];
		Cw[6] = data[5];
		Cw[7] = data[3] + data[4] + data[5];

		*ParRoundNr = *ParRoundNr + 1;

		//copy cw end
		for(i=0; i<6; i++)
		{
			ParCw6End[i] = data[i];
		}

		if(1)//		if((RoundNr+1) < (BLOCKS_RB * THREADS_RB))
		{
			uchar ksfull[7*8];
			KeySchedule07_Xor(ksfull, Cw);

			//plain input
			for(i=0; i<8; i++)
			{
				data[i] = 0;
			}

			//Enc
			for(i=0; i<23; i++)
			{
				block_encrypt_dataOnly_ulong(ksfull, data);
			}
			
			for(i=0; i<8; i++)
			{
				ParCrypt[i] = data[i];
			}
		}
	}
}

__global__ void kernel_csa_rainbow_a(uchar *Cw6EndArray_d,
	const uchar *Cw6StartArray_d, ulong StartRoundNr, DEBUG_STRUCT *Debug_d)
{
	uchar Cw[8];
	//ulong dwCw;
	int Offset;
	Offset = (blockIdx.x * THREADS_RB) + threadIdx.x;

	// optimize by copy const to shared (for all treads)
	// till sync treads

	COPY_CONST_TO_SHARED

	/** sync */
	__syncthreads();

	/////////////////////////////////
	//6 -> 8
	const uchar *Cw6;
	Cw6 = &Cw6StartArray_d[Offset * 6];
	Cw[0] = Cw6[0];
	Cw[1] = Cw6[1];
	Cw[2] = Cw6[2];
	Cw[3] = Cw6[0] + Cw6[1] + Cw6[2];
	Cw[4] = Cw6[3];
	Cw[5] = Cw6[4];
	Cw[6] = Cw6[5];
	Cw[7] = Cw6[3] + Cw6[4] + Cw6[5];


	int i;
//	int k;
//	int j;
	int RoundNr;

	ulong data[8];

	for(RoundNr=StartRoundNr; RoundNr < (StartRoundNr + 
		RAINBOW_CHAIN_LEN_PER_KERNEL); RoundNr++)
	{
		uchar ksfull[7*8];
		KeySchedule07_Xor(ksfull, Cw);

		//plain input
		for(i=0; i<8; i++)
		{
			data[i] = 0;
		}

		//Enc
		for(i=0; i<23; i++)
		{
			block_encrypt_dataOnly_ulong(ksfull, data);
		}

		//Reduction part 2 (XOR RoundNr)
		data[2] ^= ((RoundNr>>24)&0xFF);
		data[3] ^= ((RoundNr>>16)&0xFF);
		data[4] ^= ((RoundNr>>8)&0xFF);
		data[5] ^= (RoundNr&0xFF);

		//checksum
		Cw[0] = data[0];
		Cw[1] = data[1];
		Cw[2] = data[2];
		Cw[3] = data[0] + data[1] + data[2];
		Cw[4] = data[3];
		Cw[5] = data[4];
		Cw[6] = data[5];
		Cw[7] = data[3] + data[4] + data[5];
	}


	for(i=0; i<6; i++)
	{
		Cw6EndArray_d[(Offset * 6) + i] = data[i];
	}

}

/** function exposed to the world ------------------------------------------ */

int CsaCudaRainbowDoXRounds(uchar *Cw6Array_h, ulong *CurRoundNrArray_h,
	ulong *RemRoundsArray_h, DEBUG_STRUCT *Debug_h)
{
	//Cw6Array_h [in/out]
	//CurRoundNrArray_h [in/out]
	//RemRoundsArray_h [in]
	//return 0=ok
	uchar *Cw6Array_d;
	ulong *CurRoundNrArray_d;
	ulong *RemRoundsArray_d;
	DEBUG_STRUCT *Debug_d;
	cudaError_t ce;

	//alloc device memory
	ce = cudaMalloc((void **) &Cw6Array_d, BLOCKS_RB * THREADS_RB * 6);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &CurRoundNrArray_d, BLOCKS_RB * THREADS_RB *
			sizeof(ulong));
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &RemRoundsArray_d, BLOCKS_RB * THREADS_RB * 
			sizeof(ulong));
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &Debug_d, sizeof(DEBUG_STRUCT));
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input Cw6Array
	ce = cudaMemcpy(Cw6Array_d, Cw6Array_h, BLOCKS_RB * THREADS_RB * 6, 
			cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input CurRoundNrArray
	ce = cudaMemcpy(CurRoundNrArray_d,CurRoundNrArray_h,BLOCKS_RB*THREADS_RB*
			sizeof(ulong), cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input RemRoundsArray
	ce = cudaMemcpy(RemRoundsArray_d, RemRoundsArray_h,BLOCKS_RB * THREADS_RB *
			sizeof(ulong), cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input Debug
	ce = cudaMemcpy(Debug_d, Debug_h, sizeof(DEBUG_STRUCT), cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	/////////////////
	//Start kernel
	kernel_csa_rainbow_do_x_rounds<<<BLOCKS_RB,THREADS_RB,0,0>>>
		(Cw6Array_d, CurRoundNrArray_d, RemRoundsArray_d, Debug_d);
	/////////////////

	//Sync
	while(cudaStreamQuery(0)==cudaErrorNotReady)
	{
	  int64_t num_cycles = get_cycles(5.0f);
	  //keep CPU utilization near 0% (thanks to old-man)
	  sleep<<< BLOCKS_RB, THREADS_RB, 0, 0 >>>(num_cycles);
	}

	ce = cudaStreamSynchronize(0);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output Cw6Array
	ce = cudaMemcpy(Cw6Array_h, Cw6Array_d, BLOCKS_RB * THREADS_RB * 6,
			cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output CurRoundNrArray
	ce = cudaMemcpy(CurRoundNrArray_h, CurRoundNrArray_d, BLOCKS_RB *
			THREADS_RB * sizeof(ulong), cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output RemRoundsArray
	ce = cudaMemcpy(RemRoundsArray_h, RemRoundsArray_d, BLOCKS_RB *THREADS_RB*
			sizeof(ulong), cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output Debug
	ce = cudaMemcpy(Debug_h, Debug_d, sizeof(DEBUG_STRUCT), 
			cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Sync
	ce = cudaStreamSynchronize(0);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//free device memory
	ce = cudaFree(Cw6Array_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(CurRoundNrArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(RemRoundsArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(Debug_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	return 0;//ok
}

int CsaCudaRainbowFindA(uchar *Cw6EndArray_h, ulong *RoundNrArray_h,
	uchar *CryptArray_h, DEBUG_STRUCT *Debug_h)
{
	//return 0=ok
	uchar *Cw6EndArray_d;
	uchar *CryptArray_d;
	ulong *RoundNrArray_d;
	DEBUG_STRUCT *Debug_d;
	cudaError_t ce;

	//alloc device memory
	ce = cudaMalloc((void **) &Cw6EndArray_d, BLOCKS_RB * THREADS_RB * 6);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &RoundNrArray_d, BLOCKS_RB * THREADS_RB * 
			sizeof(ulong));
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &CryptArray_d, BLOCKS_RB * THREADS_RB * 8);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &Debug_d, sizeof(DEBUG_STRUCT));
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input Cw6EndArray
	ce = cudaMemcpy(Cw6EndArray_d, Cw6EndArray_h, BLOCKS_RB * THREADS_RB * 6, 
			cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input RoundNrArray
	ce = cudaMemcpy(RoundNrArray_d, RoundNrArray_h, BLOCKS_RB * THREADS_RB * 
			sizeof(ulong), cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input CryptArray
	ce = cudaMemcpy(CryptArray_d, CryptArray_h, BLOCKS_RB * THREADS_RB * 8,
			cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input Debug
	ce = cudaMemcpy(Debug_d, Debug_h, sizeof(DEBUG_STRUCT), cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	/////////////////
	//Start kernel
	kernel_csa_rainbow_find_a<<<BLOCKS_RB,THREADS_RB,0,0>>>
		(Cw6EndArray_d,RoundNrArray_d, CryptArray_d, Debug_d);
	/////////////////

	//Sync
	while(cudaStreamQuery(0)==cudaErrorNotReady)
	{
	  int64_t num_cycles = get_cycles(5.0f);
	  //keep CPU utilization near 0% (thanks to old-man)
	  sleep<<< BLOCKS_RB, THREADS_RB, 0, 0 >>>(num_cycles);
	}

	ce = cudaStreamSynchronize(0);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output Cw6EndArray
	ce = cudaMemcpy(Cw6EndArray_h,Cw6EndArray_d,BLOCKS_RB * THREADS_RB * 6,
			cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output RoundNrArray
	ce = cudaMemcpy(RoundNrArray_h, RoundNrArray_d, BLOCKS_RB * THREADS_RB * 
			sizeof(ulong), cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output CryptArray
	ce = cudaMemcpy(CryptArray_h, CryptArray_d, BLOCKS_RB * THREADS_RB * 8, 
			cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output Debug
	ce = cudaMemcpy(Debug_h, Debug_d, sizeof(DEBUG_STRUCT), 
		cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Sync
	ce = cudaStreamSynchronize(0);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//free device memory
	ce = cudaFree(Cw6EndArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(RoundNrArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(CryptArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(Debug_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	return 0;//ok
}

int CsaCudaRainbow1(uchar *Cw6EndArray_h, const uchar *Cw6StartArray_h,
	ulong StartRoundNr, DEBUG_STRUCT *Debug_h)
{
	uchar *Cw6EndArray_d;
	uchar *Cw6StartArray_d;
	//ulong StartRoundNr_d;
	DEBUG_STRUCT *Debug_d;
	cudaError_t ce;

//int deviceCount = 0;
//cudaError_t err = cudaGetDeviceCount(&deviceCount);
//CheckConditionXR_(err == cudaSuccess, err);
	
	//alloc device memory
	ce = cudaMalloc((void **) &Cw6StartArray_d, BLOCKS_RB * THREADS_RB * 6);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &Cw6EndArray_d, BLOCKS_RB * THREADS_RB * 6);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaMalloc((void **) &Debug_d, sizeof(DEBUG_STRUCT));
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input Cw6StartArray
	ce = cudaMemcpy(Cw6StartArray_d, Cw6StartArray_h,BLOCKS_RB*THREADS_RB*6,
			cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy input Debug
	ce = cudaMemcpy(Debug_d, Debug_h, sizeof(DEBUG_STRUCT),
			cudaMemcpyHostToDevice);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	/////////////////
	//Start kernel
	kernel_csa_rainbow_a<<<BLOCKS_RB,THREADS_RB,0,0>>> 
		( Cw6EndArray_d,Cw6StartArray_d, StartRoundNr, Debug_d);
	/////////////////

	//Sync
	while(cudaStreamQuery(0)==cudaErrorNotReady)
	{
	  int64_t num_cycles = get_cycles(5.0f);
	  //keep CPU utilization near 0% (thanks to old-man)
	  sleep<<< BLOCKS_RB, THREADS_RB, 0, 0 >>>(num_cycles);
	}

	ce = cudaStreamSynchronize(0);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output Cw6EndArray
	ce = cudaMemcpy(Cw6EndArray_h, Cw6EndArray_d, BLOCKS_RB * THREADS_RB * 6,
			cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Copy output Debug
	ce = cudaMemcpy(Debug_h, Debug_d, sizeof(DEBUG_STRUCT),
			cudaMemcpyDeviceToHost);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//Sync
	ce = cudaStreamSynchronize(0);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	//free device memory
	ce = cudaFree(Cw6StartArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(Cw6EndArray_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	ce = cudaFree(Debug_d);
	assert(ce == cudaSuccess);
	if(ce != cudaSuccess){return -1;}

	return 0;//ok
}

