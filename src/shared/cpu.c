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
#include <pcrtt.h>

#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>


#include <shared/define.h>
#include <shared/table.h>

#define DVBCSA_DATA_SIZE	8
#define DVBCSA_KEYSBUFF_SIZE	56
#define DVBCSA_CWBITS_SIZE	64

typedef uint8_t			dvbcsa_block_t[DVBCSA_DATA_SIZE];
typedef uint8_t			dvbcsa_keys_t[DVBCSA_KEYSBUFF_SIZE];
typedef unsigned char		dvbcsa_cw_t[8];

void
dvbcsa_key_schedule_block(const dvbcsa_cw_t cw, uint8_t * kk);

/** ---------------------------------------------------------------------------
 *  -- kernel C function ------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */
//#define SLOW_DOWN 1
#define NB_WORK_TODO (BLOCKS_RB * THREADS_RB)

struct Cpu_rainbow_t
{
	uchar *oarray; // Cw6EndArray_d;
	const uchar *iarray; // Cw6StartArray_d;
	ulong srnr; // StartRoundNr;
	uint nb_job;
	int proc_num; // core id
} ;

static uint _thread_pid = 0;
static uint _thread_joint = 0;

static const uchar csa_block_perm[256] = TABLE_CSA_BLOCK_PERM
static const uchar block_sbox[256] = TABLE_BLOCK_SBOX

const uint get_size_csa_block_perm()
{
	return sizeof(csa_block_perm);
}

const uchar* get_csa_block_perm()
{
	return csa_block_perm;
}

const uint get_size_block_sbox()
{
	return sizeof(block_sbox);
}

const uchar* get_block_sbox()
{
	return block_sbox;
}

static void KeySchedule07_Xor(uchar *ksfull, const uchar *Cw8)
KeySchedule07_Xor_code_block

static void block_encrypt_dataOnly_ulong(const dvbcsa_keys_t key,ulong *data)
block_encrypt_dataOnly_ulong_code_block

static uint thread_joingned()
{
	uint r = _thread_joint;
	return r;
}

#if !defined(__LINUX__)

void mutex_lock( uint* mtx );
void mutex_unlock( uint* mtx );
uint create_mutex(void);
void destroy_mutex( uint* mtx );
void add_thread_task(void (*fn_ptr)(void*), void* arg);

#define MTX_LU(stuff)	mutex_lock(mutex); \
							stuff \
							mutex_unlock(mutex);
							
static uint mutex = 0;

static uint get_offset()
{
	uint r = _thread_pid;
	MTX_LU( ++_thread_pid; )
	return r;
}

static void join_thread()
{
	MTX_LU( ++_thread_joint; )
}

#else // __LINUX__

static pid_t gettid( void )
{
	return syscall( __NR_gettid );
}

static int prepare_thread(int proc_num)
{
	cpu_set_t set;
	CPU_ZERO( &set );
	CPU_SET( proc_num, &set );

	if ( sched_setaffinity( gettid(), sizeof(cpu_set_t), &set) )
	{
		perror( "sched_setaffinity" );
		return 0;
	}

	return 1;
}

#endif // __LINUX__

static void c_csa_rainbow_a(void* cookies)
{
	struct Cpu_rainbow_t* rdata = (struct Cpu_rainbow_t*)cookies;
	/*************************************************************************/
	uchar *oarray = rdata->oarray;
	const uchar *iarray = rdata->iarray;
	const ulong srnr = rdata->srnr;
	const uint nb_job = rdata->nb_job;
	/*************************************************************************/

	uchar Cw[8]={0};
	ulong data[8]={0};
	uchar ksfull[7*8]={0};
	int Offset, i, RoundNr, job;

#if defined(__LINUX__)
	int proc_num = rdata->proc_num;

	if ( !prepare_thread(proc_num) )
	{
		free(rdata);
		return;
	}
#endif // __LINUX__

	for( job = 0; job < nb_job; ++job )
	{
		const uint tid =
#if defined(__LINUX__)
		_thread_pid;
		__sync_fetch_and_add( &_thread_pid, 1 );
#else
		get_offset();
#endif // __LINUX__

		Offset = tid;

		//printf("Offset: %d\n", Offset );

		// 6 -> 8
		const uchar *Cw6 = &iarray[Offset * 6];
		Cw[0] = Cw6[0];
		Cw[1] = Cw6[1];
		Cw[2] = Cw6[2];
		Cw[3] = Cw6[0] + Cw6[1] + Cw6[2];
		Cw[4] = Cw6[3];
		Cw[5] = Cw6[4];
		Cw[6] = Cw6[5];
		Cw[7] = Cw6[3] + Cw6[4] + Cw6[5];

		for ( RoundNr = srnr; RoundNr < (srnr + 
			RAINBOW_CHAIN_LEN_PER_KERNEL); ++RoundNr )
		{
			KeySchedule07_Xor(ksfull, Cw);

			//plain input
#pragma unroll
			for(i=0; i<8; ++i)
			{
				data[i] = 0;
			}

			// Enc
			for(i=0; i<23; ++i)
			{
				block_encrypt_dataOnly_ulong(ksfull, data);
			}

			// Reduction part 2 (XOR RoundNr)
			data[2] ^= ( (RoundNr >> 24) & 0xFF );
			data[3] ^= ( (RoundNr >> 16) & 0xFF );
			data[4] ^= ( (RoundNr >>  8) & 0xFF );
			data[5] ^= ( RoundNr & 0xFF);

			// checksum
			Cw[0] = data[0];
			Cw[1] = data[1];
			Cw[2] = data[2];
			Cw[3] = data[0] + data[1] + data[2];
			Cw[4] = data[3];
			Cw[5] = data[4];
			Cw[6] = data[5];
			Cw[7] = data[3] + data[4] + data[5];
		}

#pragma unroll
		for(i=0; i<6; i++)
		{
			oarray[(Offset * 6) + i] = data[i];
		}

#if defined(__LINUX__)
		__sync_fetch_and_add( &_thread_joint, 1 );
#else
		join_thread();
#endif // __LINUX__

#if defined(SLOW_DOWN)
		if ( (job % 100) == 0 )
			usleep( 1000 * 50 );
#endif // 
	}

	free(rdata);
	rdata=0;
}

int start_cpu_rainbow_1(uchar *a,const uchar *b,const ulong c)
{
	int x;
	// reset counter
	_thread_pid = 0;
	_thread_joint = 0;

#if !defined(__LINUX__)
	if ( 0 == mutex )
		mutex = create_mutex();
#endif // __LINUX__

	const int nb_core =
#if defined(HAVE_SYSCONF)
	 (int)sysconf( _SC_NPROCESSORS_ONLN );
#else
	8; // get_cpu_nb_core();
#endif // 

	if (nb_core < 0)
	{
		TRACE_ERROR( "invalid return from sysconf(...)" );
		return 0;
	}

	/** push nb_core thread to do the work on the thread pool */
	for( x = 0; x < nb_core; ++x)
	{
		struct Cpu_rainbow_t* t	= malloc( sizeof(struct Cpu_rainbow_t) );

		t->oarray	= a;
		t->iarray	= b;
		t->srnr		= c;
		t->nb_job	= NB_WORK_TODO / nb_core;
		t->proc_num	= x;

		add_thread_task( c_csa_rainbow_a, t );
	}

	/** wait until all job is done */
	for(;;)
	{
		if( thread_joingned() == NB_WORK_TODO )
			break;
		usleep( 1000 * 10 );
	}

	return 1;
}
