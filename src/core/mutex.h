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
#if !defined(PCRTT_SRC_CORE_MUTEX_HEADER_H_)
#define PCRTT_SRC_CORE_MUTEX_HEADER_H_

namespace PCRTT {

#define ERR_CMUTEX_ALREADYINIT		-1
#define ERR_CMUTEX_NOTINIT			-2
#define ERR_CMUTEX_CANTCREATEMUTEX	-3

class Mutex
{
public:
	Mutex();
	~Mutex();
	int init();
	int lock();
	int unlock();

	inline bool IsInitialized() const { return (const bool)initialized; }
	inline int started() const { return (IsInitialized()?1:0); }

	/* pthread_mutex_t or HANDLE */
#if defined(WIN32)
	inline HANDLE get_mutex_ptr() { return mutex; }
#elif !defined(WIN32) && defined(HAVE_PTHREAD_H)
	inline pthread_mutex_t * get_mutex_ptr() { return &_mutex; }
#endif

private:

#if (defined(WIN32) || defined(_WIN32_WCE))
	#ifdef CMUTEX_CRITICALSECTION
		CRITICAL_SECTION mutex;
	#else // Use standard mutex
		HANDLE mutex;
	#endif // CMUTEX_CRITICALSECTION
#elif !defined(__WIN32__) && defined(HAVE_PTHREAD_H)
	pthread_mutex_t _mutex;
	ushort locked;
#endif // WIN32
	bool initialized;
};

class AutoMutex
{
public:
	AutoMutex(Mutex &m) : _mutex(m) { _mutex.init(); _mutex.lock(); }
	~AutoMutex() { _mutex.unlock(); }
private:
	Mutex &_mutex;
};

class ScopedMutex
{
public:
	ScopedMutex(Mutex &m) : _mutex(m) { _mutex.init(); _mutex.lock(); }
	~ScopedMutex() { _mutex.unlock(); }
private:
	Mutex &_mutex;
};

} // end of namespace PCRTT

/** todo: docs*/
//! todo: docs
extern "C" void mutex_lock( uint* mtx );

/** todo: docs*/
//! todo: docs
extern "C" void mutex_unlock( uint* mtx );

/** todo: docs*/
//! todo: docs
extern "C" uint create_mutex(void);

/** todo: docs*/
//! todo: docs
extern "C" void destroy_mutex( uint* mtx );

#endif // PCRTT_SRC_CORE_MUTEX_HEADER_H_
