EXTRA_DIST = $(top_srcdir)/changelog $(top_srcdir)/bootstrap \
			pcrtt.h \
			core/src.inc \
			dvbcsa/src.inc \
			shared/src.inc \
			cuda/src.inc \
			opencl/src.inc \
			fltk/src.inc \
			gtk/src.inc
			

FLTK_SRC = 
GTK_SRC =
CONSOLE_SRC =
CUDA_SRC = 
OPENCL_SRC = 
DIALOG_SRC = 
SHARED_SRC =
DVBCSA_SRC = 
CORE_SRC = 
ADL_SRC = 
WORKER_SRC = 
EXT_SORT_SRC = 

include worker/src.inc
include adl/src.inc
include core/src.inc
include dvbcsa/src.inc
include shared/src.inc
include extsort/src.inc

if USE_CUDA
include cuda/src.inc
endif # USE_CUDA

if USE_OPENCL
include opencl/src.inc
endif # USE_OPENCL

if USE_FLTK
include fltk/src.inc
endif # USE_FLTK

if USE_GTK
include gtk/src.inc
endif # USE_GTK

# pcrtt gui source code
GUI_SRC = $(FLTK_SRC)		\
		  $(GTK_SRC)		\
		  $(DIALOG_SRC)		\
		  $(CONSOLE_SRC)	\
		  $(SHARED_SRC)		\
		  pcrtt.cxx

# libpcrtt source code

ALL_SOURCES_FILES = $(CORE_SRC) $(WORKER_SRC) $(ADL_SRC) \
					$(DVBCSA_SRC) $(CUDA_SRC) $(OPENCL_SRC) \
					$(EXT_SORT_SRC)

EXTRA_DIST += $(ALL_SOURCES_FILES) $(GUI_SRC)
