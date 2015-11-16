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
#if !defined(PCRTT_SRC_TYPE_HEADER_H_)
#define PCRTT_SRC_TYPE_HEADER_H_

// msdn:
// typedef unsigned long ULONG, *PULONG;
// typedef int BOOL, *PBOOL, *LPBOOL;
// typedef unsigned char BYTE, *PBYTE, *LPBYTE;
// typedef unsigned long DWORD, *PDWORD, *LPDWORD;
// typedef signed __int64 LONGLONG;
// typedef unsigned __int64 ULONGLONG;
// typedef unsigned short WORD, *PWORD, *LPWORD;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#if !defined(HAVE_STDINT_H) 
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t;
typedef unsigned long long int uint64_t ;
#if !defined(__NVCC__) // already defined type
typedef signed int int16_t;
typedef signed long int int32_t;
typedef signed long long int int64_t;
#endif // __NVCC__
#endif // HAVE_STDINT_H

typedef uint64_t my_size_t;
typedef uint64_t ulonglong;

/** job type for worker */
enum Job_e {
	NO_JOB,
	STOP_JOB,
	#define STOP_JOB_ASKED (STOP_JOB == worker->job)

	// Calc Chain job
	CHAIN_START,	// fCalcChainStart
	#define CHAIN_START_ASKED (CHAIN_START == worker->job)
	CHAIN_PAUSE,	// fCalcChainPause
	#define CHAIN_PAUSE_ASKED (CHAIN_PAUSE == worker->job)
	CHAIN_STOP,		// fCalcChainStop
	#define CHAIN_STOP_ASKED (CHAIN_STOP == worker->job)
	
	RBT_START,		// fAddToRbtStart
	#define RBT_START_ASKED (RBT_START == worker->job)
	RBT_STOP,		// fAddToRbtStop
	#define RBT_STOP_ASKED (RBT_STOP == worker->job)

	CW_START,		// fSearchCwStart
	#define CW_START_ASKED (CW_START == worker->job)
	CW_STOP,		// fSearchCwStop
	#define CW_STOP_ASKED (CW_STOP == worker->job)
	
	C8_START,		// fSearchCrypt8InTsStart
	#define C8_START_ASKED (C8_START == worker->job)
	C8_STOP,		// fSearchCrypt8InTsStop
	#define C8_STOP_ASKED (C8_STOP == worker->job)

	NO_MSG,			// fDontSentMsgToGui
}; // end of enum Job_e

/** flags for worker */
enum Flags_e {
#if defined(__CUDA__)
	/** current job type */
	CUDA_WORKER		= 1 << 0,
	#define IS_CUDA(flags)		(flags & CUDA_WORKER)
#else
	#define IS_CUDA(flags)		(0)
#endif // __CUDA__

#if defined(__OPENCL__)
	OPENCL_WORKER	= 1 << 1,
	#define IS_OPENCL(flags)	(flags & OPENCL_WORKER)
#else
	#define IS_OPENCL(flags)	(0)
#endif // __OPENCL__

#if defined(__VULKAN__)
	VULKAN_WORKER	= 1 << 2,
	#define IS_VULKAN(flags)	(flags & VULKAN_WORKER)
#else
	#define IS_VULKAN(flags)	(0)
#endif // __VULKAN__

	CPU_WORKER	= 1 << 3,
	#define IS_CPU(flags)	(flags & CPU_WORKER)

	/** answer */
	NO_MSG_OK		= 1 << 10,
	JOB_STOPPED		= 1 << 11,
};

struct Stats_t
{
#ifdef __cplusplus
	Stats_t() : ProcCalcChainPart(0), ProcCalcChainFile(0),
				ProcSearchCrypt8InTsFile(0), kCW_per_sec(0), DeltaTime(0),
				ProcAddToRbtFile(0) {}
	enum Type_t {
		CALC,
		RBT,
		CRYPT8,
		CW
	};
#endif // __cplusplus
	// calc chain statistic
	float ProcCalcChainPart;
	float ProcCalcChainFile;
	float ProcSearchCrypt8InTsFile;
	ulong kCW_per_sec;
	uint64_t DeltaTime;
	// add to RBT statistic
	float ProcAddToRbtFile;
}; // end of struct Stats_t

struct Worker_t
{
#ifdef __cplusplus
	Worker_t() : job(0), flags(0), locked(0), stats() { }
#endif // __cplusplus

	// current job
	ushort job;

	// flags associated with current job
	uint flags;

	// lock
	ushort locked;

	/*int test;
	CView *pView;
	int fExitThread;
	int fDontSentMsgToGui;
	int fDontSentMsgToGuiOk;

	int fCalcChainStart;
	int fCalcChainPause;
	int fCalcChainStop;
	int fAddToRbtStart;
	int fAddToRbtStop;
	int fSearchCwStart;
	int fSearchCwStop;
	int fSearchCrypt8InTsStart;
	int fSearchCrypt8InTsStop;

	CString *pCalcChain_ChainDir;
	CString *pAddToRbt_Src;
	CString *pAddToRbt_RbtDir;
	CString *pSearchCrypt8InTs_Src;
	CString *pSearchCrypt8InTs_Pid;
	CString *pSearchCrypt8InTs_PayloadSize;
	CString *pSearchCrypt8InTs_FileLimit;
	CString *pSearchCw_RbtDir;
	CString *pSearchCw_Crypt8;*/

	// stats for worker
	struct Stats_t stats;

}; // end of struct Worker_t

#endif // PCRTT_SRC_TYPE_HEADER_H_
