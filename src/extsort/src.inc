
EXT_SORT_SRC += extsort/IntegerSorting.c	\
				extsort/DynamicBuffer.c		\
				extsort/BitArray.c			\
				extsort/CrashInfo.c			\
				extsort/ExternalSortAPI.c	\
				extsort/MeasureTime.c		\
				extsort/CrashInfo.c			\
				extsort/ExternalSort.c

EXT_SORT_INC = extsort/BitArray.h			\
				extsort/CrashInfo.h			\
				extsort/DynamicBuffer.h		\
				extsort/ExternalSortAPI.h	\
				extsort/ExternalSort.h		\
				extsort/IntegerSorting.h	\
				extsort/MeasureTime.h

EXTRA_DIST += $(EXT_SORT_INC)
