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
#include <pcrtt.h>

namespace PCRTT {

Thread::Thread() :
#if defined(__WIN32__)
	threadid(0), threadhandle(0),
#elif defined(HAVE_PTHREAD_H)
	threadid(0),
#endif // WIN32
	mtx_init(0),running(0),retval(0),mtx(),continue_mtx(),continue_mtx2()
{
	
}

Thread::~Thread()
{
	kill();
}

void Thread::started()
{
	continue_mtx2.unlock();
}

int Thread::isRunning()
{
	int r;
	
	mtx.lock();			
	r = running;
	mtx.unlock();
	return r;
}

void *Thread::getReturnValue()
{
	void *val;

	mtx.lock();
	if (running)
		val = NULL;
	else
		val = retval;
	mtx.unlock();

	return val;
}

THREAD_RETURN_TYPE Thread::internal_thread( void *param )
{
	Thread *thd;
	void *ret;

	thd = reinterpret_cast<Thread *>(param);

	thd->continue_mtx2.lock();
	thd->mtx.lock();
	thd->running = 1;
	thd->mtx.unlock();

	thd->continue_mtx.lock();
	thd->continue_mtx.unlock();

	ret = thd->thread();

	thd->mtx.lock();

	thd->running	= 0;
	thd->retval		= ret;

#if defined( __WIN32__ )
	CloseHandle( thd->threadhandle );
#endif	// win32

	thd->mtx.unlock();

	return 0;		
}

#if defined( __WIN32__ )

int Thread::start()
{
	if ( !mtx_init )
	{
		if ( !mtx.started() )
		{
			if ( mtx.init() < 0 )
				return THREAD_CANT_INIT_MUTEX;
		}
		if ( !continue_mtx.started() )
		{
			if ( continue_mtx.init() < 0 )
				return THREAD_CANT_INIT_MUTEX;
		}
		if ( !continue_mtx2.started() )
		{
			if ( continue_mtx2.init() < 0 )
				return THREAD_CANT_INIT_MUTEX;
		}
		mtx_init = 1;
	}

	mtx.lock();
	if (running)
	{
		mtx.unlock();
		return THREAD_ALREADY_RUNNING;
	}
	mtx.unlock();

	continue_mtx.lock();

	threadhandle = (HANDLE)_beginthreadex(NULL,0,internal_thread,this,0,&threadid);

	if (threadhandle == NULL)
	{
		continue_mtx.unlock();
		return THREAD_CANT_START_THREAD;
	}

	// Wait until 'running' is set
	mtx.lock();			
	while (!running)
	{
		mtx.unlock();
		Sleep(1);
		mtx.lock();
	}
	mtx.unlock();

	continue_mtx.unlock();

	continue_mtx2.lock();
	continue_mtx2.unlock();

	return 0;
}

int Thread::kill()
{
	mtx.lock();		
	if (!running)
	{
		mtx.unlock();
		return THREAD_NOT_RUNNING;
	}
	TerminateThread(threadhandle,0);
	CloseHandle(threadhandle);
	running = 0;
	mtx.unlock();

	return 0;
}

#elif defined(HAVE_PTHREAD_H)

int Thread::start()
{
	int status;

	if ( !mtx_init )
	{
		if ( !mtx.started() )
		{
			if ( mtx.init() < 0 )
				return THREAD_CANT_INIT_MUTEX;
		}
		if ( !continue_mtx.started() )
		{
			if ( continue_mtx.init() < 0 )
				return THREAD_CANT_INIT_MUTEX;
		}
		if ( !continue_mtx2.started() )
		{
			if ( continue_mtx2.init() < 0 )
				return THREAD_CANT_INIT_MUTEX;
		}
		mtx_init = 1;
	}

	mtx.lock();
	if ( running )
	{
		mtx.unlock();
		return THREAD_ALREADY_RUNNING;
	}
	mtx.unlock();

	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr,PTHREAD_CREATE_DETACHED );

	continue_mtx.lock();
	status = pthread_create( &threadid, &attr, internal_thread, this );
	pthread_attr_destroy(&attr);

	if (status != 0)
	{
		continue_mtx.unlock();
		return THREAD_CANT_START_THREAD;
	}

	// Wait until 'running' is set
	mtx.lock();

	while ( !running )
	{
		mtx.unlock();

		struct timespec req,rem;

		req.tv_sec	= 0;
		req.tv_nsec	= 1000000;

		nanosleep( &req, &rem );

		mtx.lock();
	}
	mtx.unlock();
	
	continue_mtx.unlock();
	
	continue_mtx2.lock();
	continue_mtx2.unlock();

	return 0;
}

int Thread::kill()
{
	mtx.lock();			

	if ( !running )
	{
		mtx.unlock();
		return THREAD_NOT_RUNNING;
	}
#if defined(__ANDROID__)
	pthread_kill( threadid, SIGUSR1 );
#else
	pthread_cancel( threadid );
#endif // __ANDROID__

	running = 0;
	mtx.unlock();

	return 0;
}
#endif	// WIN32

void start_thread( thd_fnc fnc, void* data )
{
	ThreadPackage* p = new ThreadPackage();
	p->data = data;

#if defined(__WIN32__)
	#if 1
		p->handle = _beginthreadex( NULL, 0, fnc, p, 0, &p->id );
	#else
		p->handle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)fnc, data, CREATE_SUSPENDED, &p->id );
		ResumeThread( p->handle );
	#endif // 1
#elif defined(HAVE_PTHREAD_H)
	// create thread in suspended state
	pthread_create( &p->id, 0, fnc, p );
	pthread_detach( p->id );
#endif	// __WIN32__
}	// start_thread()

class mThread : public Thread
{
public:
	#if defined(__USE_CPP_11__)
	mThread(const mThread&) = delete;
	void operator=(const mThread&) = delete;
	#endif // __USE_CPP_11__

	mThread(thd_fnc fnc,void* data) : Thread(),
		fnc(fnc), data(data)
	{ }
	virtual ~mThread() {}
	virtual void* thread()
	{
		if (fnc) return fnc(data);
		return (void*)THREAD_ERROR;
	}
private:
	thd_fnc fnc;
	void* data;
};

extern "C" void* create_thread( thd_fnc fnc, void* data )
{
	mThread* p = new mThread(fnc,data);

	if(!p)
	{
		return ((void*)THREAD_ERROR);
	}

	p->start();

	return p;
}

extern "C" int wait_thread( void* tid )
{
	mThread* p = (mThread*)tid;

	if(!p)
	{
		return THREAD_ERROR;
	}

	while( p->isRunning() )
	{
		usleep( 1 );
	}

	return THREAD_OK;
}

extern "C" int stop_thread( void* tid )
{
	mThread* p = (mThread*)tid;
	if(!p)
	{
		return THREAD_ERROR;
	}

	p->kill();
	KILLOBJECT(p);

	return THREAD_OK;
}

} // end of namespace PCRTT
