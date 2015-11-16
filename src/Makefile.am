lib_LTLIBRARIES = 
EXTRA_DIST = 
CLEANFILES = 
libpcrtt_la_CXXFLAGS_EXTRA = 
libpcrtt_la_LDFLAGS_EXTRA = 
pcrtt_gui_CFLAGS =
pcrtt_gui_CPPFLAGS = 
pcrtt_gui_CXXFLAGS_EXTRA = 
pcrtt_gui_LDFLAGS_EXTRA = 

PCRTT_CFLAGS =  $(CUDA_CFLAGS)		\
				$(OPENCL_CFLAGS)
PCRTT_LDFLAGS = $(CUDA_LDFLAGS)		\
				$(OPENCL_LDFLAGS)	\
				-lconfig

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(PCRTT_CFLAGS)
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(PCRTT_CFLAGS)
AM_CFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(PCRTT_CFLAGS)
AM_LDFLAGS = 

include src.inc

## libpcrtt
lib_LTLIBRARIES += libpcrtt.la

libpcrtt_la_CXXFLAGS_EXTRA += -fpermissive
libpcrtt_la_LDFLAGS_EXTRA += -version-info 1:0:0
libpcrtt_la_CFLAGS_EXTRA = -Wl,--no-undefined -z defs

libpcrtt_la_CXXFLAGS_EXTRA += -Wl,--no-undefined -z defs
libpcrtt_la_LDFLAGS_EXTRA += -Wl,--no-undefined -z defs
libpcrtt_la_SOURCES = $(ALL_SOURCES_FILES)

libpcrtt_la_CPPFLAGS = $(AM_CPPFLAGS) $(libpcrtt_la_CPPFLAGS_EXTRA)
libpcrtt_la_CXXFLAGS = $(AM_CXXFLAGS) $(libpcrtt_la_CXXFLAGS_EXTRA)
libpcrtt_la_LDFLAGS = $(AM_LDFLAGS) $(LDFLAGS) $(libpcrtt_la_LDFLAGS_EXTRA)
libpcrtt_la_CFLAGS = $(AM_CFLAGS) $(libpcrtt_la_CFLAGS_EXTRA)

## force static 
libpcrtt_la_LDFLAGS += -static

noinst_LTLIBRARIES =

## bin

bin_PROGRAMS = pcrtt_gui
pcrtt_gui_SOURCES = $(GUI_SRC)

pcrtt_gui_CFLAGS += $(AM_CFLAGS)
pcrtt_gui_CPPFLAGS += $(AM_CPPFLAGS)
#pcrtt_gui_LDFLAGS_EXTRA = @CUDA_LDFLAGS@ 

pcrtt_gui_LDADD = $(PCRTT_LDFLAGS) -L${top_builddir}/src/.libs -lpcrtt
