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
#include <core/threadpool.h>
#include <shared/include.h>

#if defined(__GTK__)
#include <gtk/gui.h>
#else
#include <fltk/gui.h>
#endif

namespace PCRTT {
namespace Worker {

#if defined(__CUDA__)
static Worker_t cuda_worker_;
static Mutex cuda_mtx;
#endif // __CUDA__

#if defined(__OPENCL__)
static Worker_t opencl_worker_;
static Mutex opencl_mtx;
#endif // __OPENCL__

#if defined(__VULKAN__)
static Worker_t vulkan_worker_;
static Mutex vulkan_mtx;
#endif // __VULKAN__

static const char* getWorkerName(const uint& flags)
{
	if ( IS_CUDA(flags) ) return "Cuda";
	if ( IS_OPENCL(flags) ) return "OpenCL";
	if ( IS_VULKAN(flags) ) return "Vulkan";

	return "Unknown";
}

static THREAD_RETURN_TYPE worker_thread(void* cookies)
{
	ThreadPackage* p = (ThreadPackage*)cookies;
	Worker_t* worker = (Worker_t*)p->data;
	char name[128]={0};

	if(!worker)
	{
		TRACE_FATAL("Invalid worker pointer.");
		assert(0);
		return 0;
	}

	strncpy( name, getWorkerName(worker->flags), sizeof(name) );

	srand (time(NULL));

	TRACE_DEBUG( "thread started (%s)", name );

	for(;;)
	{
		const ushort id = worker->job;

		usleep(1000 * 100);

		switch(id)
		{
			default:
			case NO_JOB:
				; // nop
			break;
			case STOP_JOB:		// ExitThread
				TRACE_DEBUG( "thread stoped (%s)", name );
				worker->flags = JOB_STOPPED;
			return 0;
			case CHAIN_START:		// fCalcChainStart
				Shared::SaveLoadSettings(1); // save
				printf("thread (%s) requested a job: CHAIN_START\n",name);
				worker->job = NO_JOB;
				Shared::ClacChain::start(worker); // CalcChain
				worker->job = NO_JOB;
			break;
			case RBT_START: // faddToRbtStart
				Shared::SaveLoadSettings(1); // save
				printf("thread (%s) requested a job: RBT_START\n",name);
				worker->job = NO_JOB;
				Shared::Rainbow::start( worker );
				worker->job = NO_JOB;
			break;
			case C8_START:		// fSearchCrypt8InTsStart
				Shared::SaveLoadSettings(1); // save
				worker->job = NO_JOB;
				//todo: SearchCrypt8InTsStart();
				Shared::Crypt8::start(worker);
				worker->job = NO_JOB;
				printf("thread (%s) requested a job: C8_START\n",name);
			break;
			case CW_START:		// fSearchCwStart
				Shared::SaveLoadSettings(1); // save
				worker->job = NO_JOB;
				//todo: SearchCwStart();
				worker->job = NO_JOB;
				printf("thread (%s) requested a job: CW_START\n",name);
			break;
			case NO_MSG:			// fDontSentMsgToGui
				worker->flags = NO_MSG_OK; // fDontSentMsgToGuiOk
			break;
		}
	}

	TRACE_DEBUG( "thread stoped (%s)", name );

	return 0;
}

int init()
{
	THREAD_POOL_MANAGER->start(8);

#if defined(__CUDA__)
	Worker_t* cuda_worker_p = &cuda_worker_;
	cuda_worker_p->job = NO_JOB;
	cuda_worker_p->flags = CUDA_WORKER;
	start_thread( worker_thread, cuda_worker_p );
#endif // __CUDA__

#if defined(__OPENCL__)

	if ( !PCRTT::OpenCL::init() )
	{
		TRACE_ERROR("Can't init opencl.");
		return 0;
	}

	Worker_t* opencl_worker_p = &opencl_worker_;
	opencl_worker_p->flags = OPENCL_WORKER;
	opencl_worker_p->job = NO_JOB;
	start_thread( worker_thread, opencl_worker_p );
#endif // __OPENCL__

#if defined(__VULKAN__)
	Worker_t* vulkan_worker_p = &vulkan_worker_;
	vulkan_worker_p->flags = VULKAN_WORKER;
	vulkan_worker_p->job = NO_JOB;
	start_thread( worker_thread, vulkan_worker_p );
#endif // __OPENCL__

	return 1;
}

void destroy()
{
	THREAD_POOL_MANAGER->destroy();

#if defined(__CUDA__)
	AutoMutex m(cuda_mtx);
	cuda_worker_.job = STOP_JOB;
#endif // __CUDA__

#if defined(__OPENCL__)
	AutoMutex m(opencl_mtx);
	opencl_worker_.job = STOP_JOB;
#endif // __OPENCL__

#if defined(__VULKAN__)
	AutoMutex m(vulkan_mtx);
	vulkan_worker_.job = STOP_JOB;
#endif // __VULKAN__

	/** wait until all worker has stopped */
	for(;;)
	{
#if defined(__CUDA__)
		if ( !(cuda_worker_.flags & JOB_STOPPED) ) {usleep(1000); continue;}
#endif // __CUDA__
#if defined(__OPENCL__)
		if ( !(opencl_worker_.flags & JOB_STOPPED) ) {usleep(1000); continue;}
#endif // __OPENCL__
#if defined(__VULKAN__)
		if ( !(vulkan_worker_.flags & JOB_STOPPED) ) {usleep(1000); continue;}
#endif // __VULKAN__

		/** no job waiting, we can stop the loop */
		break;
	}

#if defined(__OPENCL__)
	/** worker job stopped we can now stop opencl */
	PCRTT::OpenCL::destroy();
#endif // __OPENCL__
}

int start(const uint& flags, const ushort& job)
{
#if defined(__CUDA__)
	Worker_t* cuda_worker_p = &cuda_worker_;
	
	if ( IS_CUDA(flags) && (cuda_worker_p->job == NO_JOB) )
	{
		AutoMutex m(cuda_mtx);
		cuda_worker_p->job = job;
		return 1;
	}
#endif // __CUDA__

#if defined(__OPENCL__)
	Worker_t* opencl_worker_p = &opencl_worker_;

	if ( IS_OPENCL(flags) && (opencl_worker_p->job == NO_JOB) )
	{
		AutoMutex m(opencl_mtx);
		opencl_worker_p->job = job;
		return 1;
	}
#endif // __OPENCL__

#if defined(__VULKAN__)
	Worker_t* vulkan_worker_p = &vulkan_worker_;

	if ( IS_VULKAN(flags) && (vulkan_worker_p->job == NO_JOB) )
	{
		AutoMutex m(vulkan_mtx);
		vulkan_worker_p->job = job;
		return 1;
	}
#endif // __VULKAN__

	return 0;
}

} // end of namespace Worker
} // end of namespace PCRTT
