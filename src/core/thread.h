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
 * 
 */
#if !defined(PCRTT_SRC_CORE_THREAD_HEADER_H_)
#define PCRTT_SRC_CORE_THREAD_HEADER_H_

namespace PCRTT {

/**
 *	\brief macro used for thread signal
 */
#define THREAD_CANT_INIT_MUTEX		-1
#define THREAD_CANT_START_THREAD	-2
#define THREAD_THREADFUNCNOTSET		-3
#define THREAD_NOT_RUNNING			-4
#define THREAD_ALREADY_RUNNING		-5

#define THREAD_OK		1
#define THREAD_ERROR	0

// plateform specific
#if defined(__WIN32__)
	#define THREAD_ID				uint
	#define THREAD_RETURN_TYPE		uint __stdcall
	typedef unsigned int (__stdcall *thd_fnc)(void *);
#elif defined(HAVE_PTHREAD_H)
	#define THREAD_ID				pthread_t
	#define THREAD_RETURN_TYPE		void*
	typedef void* (*thd_fnc)(void *);
#else
	#error unknown thread type
#endif	// __WIN32__

#if defined(__cplusplus)

/**
 *	\brief Class to manage thread
 */
class Thread // : private Noncopyable
{
public:
	#if defined(__USE_CPP_11__)
	Thread(const Thread&) = delete;
	void operator=(const Thread&) = delete;
	#endif // __USE_CPP_11__

	/**
	 *	\brief constructor
	 */
	// constructor
	Thread();

	/**
	 *	\brief virtual destructor
	 */
	// virtual destructor
	virtual ~Thread();

	/**
	 *	\brief start the thread
	 *
	 *	\retval int	: THREAD_OK, THREAD_ERROR
	 */
	// start the thread
	int start();

	/**
	 *	\brief kill the thread
	 *
	 *	\retval int	: THREAD_OK, THREAD_ERROR
	 */
	// force to kill the thread
	int kill();

	/**
	 *	\brief pure virtual function to handle the user thread
	 *
	 *	\retval void*	: user value
	 */
	// pure virtual function to handle the user thread
	virtual void* thread() = 0;

	/**
	 *	\brief check if the thread is running
	 *
	 *	\retval int	: THREAD_OK, THREAD_ERROR
	 */
	// check if the thread is running
	int isRunning();

	/**
	 *	\brief get the returned value from the user thread
	 *
	 *	\retval void*	: return user value returned by the thread
	 */
	// get the returned value from the user thread
	void *getReturnValue();

protected:
	/**
	 *	\brief todo
	 *
	 *	\retval int
	 */
	// todo
	void started();

private:

#if defined(__WIN32__)
	// vars
	static UINT __stdcall internal_thread(void *param);
	UINT threadid;
	HANDLE threadhandle;
#elif defined(HAVE_PTHREAD_H)
	static void *internal_thread(void *param);
	pthread_t threadid;
#endif // WIN32

	// \var mtx_init : thread init
	int mtx_init;
	// \var running : thread is running or not
	int running;
	// \var retval : user returned value
	void *retval;
	// \var mtx : thread mutex
	Mutex mtx;
	// \var continue_mtx : thread mutex
	Mutex continue_mtx;
	// \var continue_mtx2 : thread mutex
	Mutex continue_mtx2;
};	// end of class Thread

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** 
 *	\brief structure to manage a thread
 *
 *	Detail:
 *		for simple and complexe usage read the example in tests/thread_mutex
 *
 *		Variables :
 *		id		: id of the thread (plateform specific)
 *		handle	: handle of the thread  (plateform specific)
 *		data	: user data to pass to the thread
 *
 */
//! structure to manage a thread
struct ThreadPackage {
	void* data;
	THREAD_ID id;
	void* handle;
}; // end of class Thread

/**
 *	\brief simple function to create a thread (plateform independant)
 *
 *	\param fnc	: thread function
 *	\param data	: the data to pass in
 *
 *	Detail:
 *		for simple and complexe usage read the example in tests/thread_mutex
 */
//! simple function to create a thread
extern "C" void start_thread( thd_fnc fnc, void* data );

/**
 *	\brief create a thread
 *
 *	\retval void*	thread or THREAD_ERROR
 */
//! create a thread
void* create_thread( thd_fnc fnc, void* data );

/**
 *	\brief wait for the thread to terminate
 *
 *	\retval int	: THREAD_OK, THREAD_ERROR
 */
//! wait for the thread to terminate
int wait_thread( void* tid );

/**
 *	\brief stop and delete the thread
 *
 *	\retval int	: THREAD_OK, THREAD_ERROR
 */
//! stop and delete thread
int stop_thread( void* tid );

#ifdef __cplusplus
}
#endif // __cplusplus

} // end of namespace PCRTT

#endif // PCRTT_SRC_CORE_THREAD_HEADER_H_
