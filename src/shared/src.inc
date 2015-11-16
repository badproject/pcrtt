
SHARED_SRC += shared/shared.cxx		\
			shared/calcchain.cxx	\
			shared/crypt8.cxx		\
			shared/searchcw.cxx		\
			shared/rainbow.cxx		\
			shared/cpu.c			\
			shared/crypt8finder.cxx

SHARED_INC = shared/calcchain.h		\
			shared/cpu.h			\
			shared/crypt8finder.h	\
			shared/crypt8.h			\
			shared/define.h			\
			shared/include.h		\
			shared/rainbow.h		\
			shared/searchcw.h		\
			shared/shared.h			\
			shared/table.h

EXTRA_DIST += $(SHARED_INC)
