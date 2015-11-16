/* 
 * threadpool.cxx
 *
 * Copyright 2002-<>-2011 ESTEVE Olivier <olivier@naskel.com>
 *
 * The copyright to the contents herein is the property of ESTEVE Olivier
 * The contents may be used and/or copied only with the written permission of
 * ESTEVE Olivier, or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 *
 *
 * $Log: threadpool.cxx,v $
 *
 *
 *
 *
 *
 *
 * 
 * 
 */
//#include <nengine/include.h>
#include <pcrtt.h>
#include <core/threadpool.h>

#include <errno.h>
#include <string.h>

using namespace std;

namespace PCRTT {

CondVar::CondVar() : _var(0)
{
#if defined(__WIN32__)
#elif defined(HAVE_PTHREAD_H)
	pthread_cond_t* cond_var = new pthread_cond_t;
	_var = cond_var;
	pthread_cond_init(cond_var, NULL);
#endif
}

CondVar::~CondVar()
{
#if defined(__WIN32__)
#elif defined(HAVE_PTHREAD_H)
	pthread_cond_t* cond_var = (pthread_cond_t*)_var;
	pthread_cond_destroy(cond_var);
	KILLOBJECT(cond_var);
#endif
}

void CondVar::wait(Mutex* mutex)
{
#if defined(__WIN32__)
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject( mutex->get_mutex_ptr(), 5000L);
#elif defined(HAVE_PTHREAD_H)
	pthread_cond_t* cond_var = (pthread_cond_t*)_var;
	pthread_cond_wait( cond_var, mutex->get_mutex_ptr() );
#endif
}

void CondVar::signal()
{
#if defined(__WIN32__)
#elif defined(HAVE_PTHREAD_H)
	pthread_cond_t* cond_var = (pthread_cond_t*)_var;
	pthread_cond_signal(cond_var);
#endif
}

void CondVar::broadcast()
{
#if defined(__WIN32__)
#elif defined(HAVE_PTHREAD_H)
	pthread_cond_t* cond_var = (pthread_cond_t*)_var;
	pthread_cond_broadcast(cond_var);
#endif
}

Task::Task(void (*fn_ptr)(void*), void* arg) :
	m_fn_ptr(fn_ptr), m_arg(arg)
{
	
}

Task::~Task()
{
	
}

void Task::operator()()
{
	run();
}

void Task::run()
{
	(*m_fn_ptr)(m_arg);
	ERASE_THREAD_TASK(this);
}

ThreadPool::ThreadPool() : 
	m_pool_size(DEFAULT_POOL_SIZE),
	m_task_mutex(),
	m_task_cond_var(),
	m_threads(),
	m_tasks(),
	m_pool_state(0)
{
	TRACE_DEBUG( "Constructed ThreadPool of size %d", m_pool_size );
}

ThreadPool::ThreadPool(int pool_size) : 
	m_pool_size(pool_size),
	m_task_mutex(),
	m_task_cond_var(),
	m_threads(),
	m_tasks(),
	m_pool_state(0)
{
	TRACE_DEBUG( "Constructed ThreadPool of size %d", m_pool_size );
}

ThreadPool::~ThreadPool()
{
	// Release resources
	if (m_pool_state != STOPPED)
	{
		destroy_threadpool();
	}
}

// We can't pass a member function to pthread_create.
// So created the wrapper function that calls the member function
// we want to run in the thread.
extern "C" void* StartThread(void* arg)
{
	ThreadPool* tp = (ThreadPool*) arg;
	tp->execute_thread();
	return NULL;
}

int ThreadPool::initialize_threadpool()
{
	// TODO: COnsider lazy loading threads instead of creating all at once
	m_pool_state = STARTED;
	int ret = -1;
	for (int i = 0; i < m_pool_size; ++i)
	{
		pthread_t tid;
		ret = pthread_create( &tid, NULL, StartThread, (void*) this );
		//tid = create_thread( StartThread, (void*) this );
		pthread_detach( tid );
		if (ret != 0)
		{
			TRACE_ERROR( "create_thread() failed: " );
			return -1;
		}
		m_threads.push_back(tid);
	}
	TRACE_DEBUG( "%d threads created by the thread pool", m_pool_size );

	return 0;
}

int ThreadPool::destroy_threadpool()
{
	// Note: this is not for synchronization, its for thread communication!
	// destroy_threadpool() will only be called from the main thread, yet
	// the modified m_pool_state may not show up to other threads until its
	// modified in a lock!
	m_task_mutex.lock();
	m_pool_state = STOPPED;
	m_task_mutex.unlock();
	TRACE_DEBUG( "Broadcasting STOP signal to all threads..." );
	m_task_cond_var.broadcast(); // notify all threads we are shutting down

	int ret = -1;
	for (int i = 0; i < m_pool_size; ++i)
	{
		void* result;
		ret = pthread_join( m_threads[i], &result );
		//TRACE_DEBUG( "pthread_join() returned " << ret << ": " << strerror(errno) );
		//wait_thread( m_threads[i] );
		//stop_thread( m_threads[i] );
		m_task_cond_var.broadcast(); // try waking up a bunch of threads that are still waiting
	}
	TRACE_DEBUG( "%d threads exited from the thread pool", m_pool_size );
	return 0;
}

void* ThreadPool::execute_thread()
{
	Task* task = NULL;
//	cout << "Starting thread " << *(uint*)pthread_self() << endl;

	while(true)
	{
		// Try to pick a task
//		cout << "Locking: " << pthread_self() << endl;
		m_task_mutex.lock();
		
		// We need to put pthread_cond_wait in a loop for two reasons:
		// 1. There can be spurious wakeups (due to signal/ENITR)
		// 2. When mutex is released for waiting, another thread can be waken up
		// from a signal/broadcast and that thread can mess up the condition.
		// So when the current thread wakes up the condition may no longer be
		// actually true!
		while ((m_pool_state != STOPPED) && (m_tasks.empty()))
		{
			// Wait until there is a task in the queue
			// Unlock mutex while wait, then lock it back when signaled
	 //	 cout << "Unlocking and waiting: " << *(uint*)pthread_self() << endl;
			m_task_cond_var.wait( &m_task_mutex );
	 //	 cout << "Signaled and locking: " << *(uint*)pthread_self() << endl;
		}

		// If the thread was woken up to notify process shutdown, return from here
		if (m_pool_state == STOPPED)
		{
	//		cout << "Unlocking and exiting: " << *(uint*)pthread_self() << endl;
			m_task_mutex.unlock();
			pthread_exit(NULL);
			return 0;
		}

		task = m_tasks.front();
		m_tasks.pop_front();
	//	cout << "Unlocking: " << *(uint*)pthread_self() << endl;
		m_task_mutex.unlock();

		//cout << "Executing thread " << pthread_self() << endl;
		// execute the task
		(*task)(); // could also do task->run(arg);
		//cout << "Done executing thread " << pthread_self() << endl;
	}
	return NULL;
}

int ThreadPool::add_task(Task* task)
{
	m_task_mutex.lock();
	// TODO: put a limit on how many tasks can be added at most
	m_tasks.push_back(task);
	m_task_cond_var.signal(); // wake up one thread that is waiting for a task to be available
	m_task_mutex.unlock();
	return 0;
}

// --------------------------------------------------------------------------------------------------------
ThreadPool*	ThreadPoolManager::_pool	= 0;
int	ThreadPoolManager::_started			= 0;
Mutex ThreadPoolManager::_mutex;

Vector<Task*> ThreadPoolManager::_list;

void ThreadPoolManager::destroy()
{
#ifdef _DEBUG
	TRACE_DEBUG("ThreadPoolManager::destroy()");
#endif

	/** stop thread pool */
	ThreadPoolManager::stop();

	/** delete all task from the list */
	for (int i=0; i < _list.size(); ++i)
	{
		KILLOBJECT( _list[i] );
	}

	_list.clear();

	KILLOBJECT( _pool );
}

uint ThreadPoolManager::count() const
{
	uint size = _list.size();
	return size;
}

int ThreadPoolManager::start(const int& nbThread)
{
	int nb = nbThread;

	if ( _started )
		return _started;

	if (_pool)
	{
		delete _pool;
		_pool = 0;
	}

	if ( nb == -1 )
	{
		nb = 8; // CPU::nbCore();
		if ( nb <= 0 )
			nb = ThreadPool::DEFAULT_POOL_SIZE;
	}

	if ( nb >= ThreadPool::MAX_POOL_SIZE )
		nb = ThreadPool::DEFAULT_POOL_SIZE;

	_pool = new ThreadPool( nb );

	if ( !_pool )
	{
		TRACE_ERROR("Can't allocate memory for thread pool manager.");
		return 0;
	}

	if ( _pool->initialize_threadpool() < 0 )
	{
		return 0;
	}

	_started = 1;

	return 1;
}

void ThreadPoolManager::stop(void)
{
	_pool->destroy_threadpool();
	_started = 0;
}

void ThreadPoolManager::erase( Task* task )
{
	ScopedMutex m(_mutex);

	int found = 0;
	for (int i=0; i < _list.size(); ++i)
	{
		if ( task == _list[i] )
		{
			found = 1;
			break;
		}
	}

	if ( found )
	{
		KILLOBJECT( task );
	}
}

int ThreadPoolManager::add( void (*fn_ptr)(void*), void* arg )
{
	ScopedMutex m(_mutex);

	if ( !_started || !_pool )
	{
		if ( !ThreadPoolManager::start() )
		{
			return 0;
		}
	}

	Task* task = new Task( fn_ptr, arg );

	_pool->add_task( task );

	return 1;
}

ThreadPoolManager* ThreadPoolManager::get()
{
	static ThreadPoolManager _singleton;
	return &_singleton;
}

} // end of namespace PCRTT

extern "C" 
void add_thread_task(void (*fn_ptr)(void*), void* arg)
{
	PCRTT::ThreadPoolManager::get()->add(fn_ptr,arg);
}
