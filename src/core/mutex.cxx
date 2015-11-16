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

#if defined( __WIN32__ )

Mutex::Mutex() : 
	initialized(false)
{
	
}

Mutex::~Mutex()
{
	if (initialized)
#ifdef CMUTEX_CRITICALSECTION
		DeleteCriticalSection(&mutex);
#else
		CloseHandle(mutex);
#endif // CMUTEX_CRITICALSECTION
}

int Mutex::init()
{
	if (initialized)
		return ERR_CMUTEX_ALREADYINIT;
#ifdef CMUTEX_CRITICALSECTION
	InitializeCriticalSection(&mutex);
#else
	mutex = CreateMutex(NULL,FALSE,NULL);
	if (mutex == NULL)
		return ERR_CMUTEX_CANTCREATEMUTEX;
#endif // CMUTEX_CRITICALSECTION
	initialized = true;
	return 0;
}

int Mutex::lock()
{
	if (!initialized)
		return ERR_CMUTEX_NOTINIT;
#ifdef CMUTEX_CRITICALSECTION
	EnterCriticalSection(&mutex);
#else
	WaitForSingleObject(mutex,INFINITE);
#endif // CMUTEX_CRITICALSECTION
	return 0;
}

int Mutex::unlock()
{
	if (!initialized)
		return ERR_CMUTEX_NOTINIT;
#ifdef CMUTEX_CRITICALSECTION
	LeaveCriticalSection(&mutex);
#else
	ReleaseMutex(mutex);
#endif // CMUTEX_CRITICALSECTION
	return 0;
}

#elif defined(HAVE_PTHREAD_H)

Mutex::Mutex() : 
#if (defined(WIN32) || defined(_WIN32_WCE))
	#ifdef CMUTEX_CRITICALSECTION
		mutex(),
	#else // Use standard mutex
		mutex(),
	#endif // CMUTEX_CRITICALSECTION
#elif !defined(__WIN32__) && defined(HAVE_PTHREAD_H)
	_mutex(),
	locked(0),
#endif // WIN32
	initialized(true)
{
	pthread_mutex_init(&_mutex, NULL);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy( &_mutex );
}

int Mutex::init()
{
	return 0;	
}

int Mutex::lock()
{
	return pthread_mutex_lock(&_mutex);
}

int Mutex::unlock()
{
	return pthread_mutex_unlock(&_mutex); 
}

#endif	// WIN32

} // end of namespace PCRTT

extern "C" 
void mutex_lock( uint* mtx )
{
	if (!mtx) 
		return;
	PCRTT::Mutex* m = (PCRTT::Mutex*)mtx;
	m->lock();
}

extern "C" 
void mutex_unlock( uint* mtx )
{
	if (!mtx) 
		return;
	PCRTT::Mutex* m = (PCRTT::Mutex*)mtx;
	m->unlock();
}

extern "C" 
uint create_mutex(void)
{
	PCRTT::Mutex* m = new PCRTT::Mutex;
	m->init();

	return *((uint*)m);
}

extern "C" 
void destroy_mutex( uint* mtx )
{
	if (!mtx) 
		return;
	PCRTT::Mutex* m = (PCRTT::Mutex*)mtx;
	KILLOBJECT(m);
}
