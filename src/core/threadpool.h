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
#ifndef PCRTT_SRC_CORE_THREAD_POOL_HEADER_H_
#define PCRTT_SRC_CORE_THREAD_POOL_HEADER_H_

#include <deque>

namespace PCRTT {
	
/** */
//!
class CondVar
{
public:
	#if defined(__USE_CPP_11__)
	CondVar(const CondVar&) = delete;
	void operator=(const CondVar&) = delete;
	#endif // __USE_CPP_11__

	CondVar();
	~CondVar();

	void wait(Mutex* mutex);
	void signal();
	void broadcast();
private:
	void* _var;
};

//template<class TClass>
class Task
{
public:
	#if defined(__USE_CPP_11__)
	Task(const Task&) = delete;
	void operator=(const Task&) = delete;
	#endif // __USE_CPP_11__

// Task(TCLass::* obj_fn_ptr); // pass an object method pointer
	Task(void (*fn_ptr)(void*), void* arg); // pass a free function pointer
	~Task();

	void operator()();

	/** auto delete task at the end of run() */
	void run();

private:
// TClass* _obj_fn_ptr;
	void (*m_fn_ptr)(void*);
	void* m_arg;
};

/** */
//!
class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(int pool_size);
	~ThreadPool();

	enum {
		STARTED  = 0,
		STOPPED  = 1,
		DEFAULT_POOL_SIZE = 4,
		MAX_POOL_SIZE = 30
	};

	int initialize_threadpool();
	int destroy_threadpool();

	void* execute_thread();

	int add_task(Task* task);

	int getState() { return m_pool_state; }

private:

	int m_pool_size;
	Mutex m_task_mutex;
	CondVar m_task_cond_var;
	Vector<pthread_t> m_threads; // storage for threads
	std::deque<Task*> m_tasks;
	volatile int m_pool_state;
};

/*******************************************/
class ThreadPoolManager
{
	ThreadPoolManager() { }
	//virtual ~ThreadPoolManager()
public:

	//! destroy this manager
	/*! destroy this manager */
	void destroy();

	/** start pool
	 *	if max value is equal to '-1' the default value are the number of cpu/core
	 */
	//!
	int start(const int& nbThread = -1);

	//! add a task to the pool
	/*! add a task to the pool */
	int	add( void (*fn_ptr)(void*), void* arg );

	/** delete a task */
	void erase( Task* task );

	// return the number of task inside the list
	uint count() const;

	// singleton
	static ThreadPoolManager* get();

protected:
	void stop(void);

	static inline void _destroy() { get()->destroy(); }

protected:
	static Vector<Task*>	_list;
	static ThreadPool*		_pool;
	static int				_started;
	static Mutex			_mutex;
};

// helper
#define THREAD_POOL_MANAGER			ThreadPoolManager::get()
#define ADD_THREAD_TASK(fnc,arg)	ThreadPoolManager::get()->add(fnc,arg);
#define ERASE_THREAD_TASK(task)		ThreadPoolManager::get()->erase(task);

} // end of namespace PCRTT

extern "C" void add_thread_task(void (*fn_ptr)(void*), void* arg);

#endif /* PCRTT_SRC_CORE_THREAD_POOL_HEADER_H_ */

