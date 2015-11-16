CORE_INC = core/core.h 			\
			core/define.h		\
			core/fast.h			\
			core/file.h			\
			core/file.inl			\
			core/log.h			\
			core/mutex.h		\
			core/rainbowtab.h	\
			core/str.h			\
			core/str.inl		\
			core/thread.h		\
			core/threadpool.h	\
			core/types.h		\
			core/vector.h		\
			core/memalloc.h		\
			core/noncopyable.h	\
			core/console.h		\
			core/sort.h

CORE_SRC += core/core.c			\
			core/fast.c			\
			core/log.c			\
			core/str.cxx		\
			core/file.cxx		\
			core/mutex.cxx		\
			core/thread.cxx		\
			core/threadpool.cxx	\
			core/file.cxx		\
			core/memalloc.cxx	\
			core/console.cxx	\
			core/sort.cxx

EXTRA_DIST += $(CORE_INC)
