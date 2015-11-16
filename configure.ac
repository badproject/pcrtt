dnl -- standard init ---------------------------------------------------------:
AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIR([m4])
AC_INIT(pcrtt,1.0,[nengine@skylab],[pcrtt],[http://tobedefined.org])
AM_MAINTAINER_MODE
AC_PROG_CC
AC_PROG_CXX
m4_ifdef([AC_PROG_OBJC],[AC_PROG_OBJC])
m4_ifdef([AC_PROG_OBJCXX],[AC_PROG_OBJCXX])
AC_LANG([C++])
AM_INIT_AUTOMAKE([foreign subdir-objects])
PKG_PROG_PKG_CONFIG
AM_SILENT_RULES([yes]) 

dnl -- Libtool ---------------------------------------------------------------:
LT_PREREQ([2.2.6])
LT_INIT([win32-dll disable-static])
AC_CONFIG_HEADERS([src/config.h])

dnl -- gcc version test ------------------------------------------------------:
HAS_GCC_VER_47=$(echo `gcc -dumpversion | cut -f1-2 -d.` \>= 4.7 | bc)
if test "${HAS_GCC_VER_47}" = "1"; then
 CXXFLAGS+=" -Weffc++ -std=c++11"
 CFLAGS+=" -std=c11"
 AC_DEFINE(__USE_CPP_11__, 1, [build with C++11 flags])
fi # HAS_GCC_VER_47

dnl -- host checkup ----------------------------------------------------------:
m4_include([m4/host.m4])

dnl -- opcode ----------------------------------------------------------------:
m4_include([m4/opcode.m4])

AC_C_BIGENDIAN(
 AC_DEFINE(DVBCSA_ENDIAN_BIG, 1, [Target is big-endian]),
 AC_DEFINE(DVBCSA_ENDIAN_LITTLE, 1, [Target is little-endian]),
 AC_MSG_ERROR(unknown endianess),
 AC_MSG_ERROR(universial endianess not supported)
)

dnl -- Standard C header -----------------------------------------------------:
AC_HEADER_STDC

dnl -- C header --------------------------------------------------------------:
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([assert.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([libconfig.h])

AC_MSG_CHECKING([_mm_malloc availability])
AC_TRY_LINK([#include <mm_malloc.h>], [_mm_malloc(8,8);],
    [
	AC_DEFINE(HAVE_MM_MALLOC, 1, _mm_malloc is available)
	AC_MSG_RESULT([found])
    ],[
	AC_MSG_RESULT([no])
    ])

dnl -- C++ header ------------------------------------------------------------:
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

dnl -- Check function --------------------------------------------------------:
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([sysconf strlwr vsnprintf snprintf sprintf printf strncasecmp])
AC_CHECK_FUNCS([sleep usleep nanosleep open close opendir closedir fstat])
AC_CHECK_FUNCS([fopen fclose fdopen fseek ftell fseeko])
AC_CHECK_FUNCS([__sync_fetch_and_add posix_memalign aligned_alloc])
AC_CHECK_FUNCS([_aligned_malloc])

dnl -- Check for libdvbcsa ---------------------------------------------------:
dnl removed
dnl AC_CHECK_LIB(dvbcsa,main)
dnl if test x"$ac_cv_lib_dvbcsa_main" != "xyes"; then
dnl  AC_MSG_ERROR([libdvbcsa is not installed])
dnl fi

dnl -- libconfig -------------------------------------------------------------:
PKG_CHECK_MODULES([LIBCONFIG], [libconfig])
AC_SUBST(LIBCONFIG_CFLAGS)
AC_SUBST(LIBCONFIG_LIBS)

dnl -- UI libraries ----------------------------------------------------------:
m4_include(m4/ui.m4)

dnl -- sdk -------------------------------------------------------------------:
m4_include(m4/sdk.m4)

dnl -- stxxl lib in contrib --------------------------------------------------:
AC_ARG_WITH(stxxl, [AS_HELP_STRING([--with-stxxl],
    [compile with stxxl support. @<:@guess@:>@])
],,[with_stxxl=no])

if test x"$with_stxxl" = "xyes"; then
 AC_CHECK_HEADER([stxxl/stream], [],
    AC_MSG_FAILURE([Couldn't find stxxl/stream]),
    [#include <stxxl/stream>])
 AC_DEFINE(__STXXL__, 1, [build with stxxl support])
 CFLAGS+=" -fopenmp "
 CXXFLAGS+=" -fopenmp "
 LDFLAGS+=" -lstxxl_debug "
fi # with_stxxl

dnl -- tbb lib in contrib ----------------------------------------------------:
AC_ARG_WITH(tbb, [AS_HELP_STRING([--with-tbb],
    [compile with tbb support. @<:@guess@:>@])
],,[with_tbb=no])

if test x"$with_tbb" = "xyes"; then
 AC_DEFINE(__TBB__, 1, [build with tbb support])
 CFLAGS+=" -I$srcdir/contribs/tbb/include -DTBB_USE_DEBUG"
 CXXFLAGS+=" -I$srcdir/contribs/tbb/include "
 if test x"$with_debug" = "xyes"; then
  LDFLAGS+=" -L$srcdir/contribs/tbb/lib/intel64/gcc4.4 -ltbb_debug "
  CXXFLAGS+=" -DTBB_USE_DEBUG"
 else
  LDFLAGS+=" -L$srcdir/contribs/tbb/lib/intel64/gcc4.4 -ltbb "
  CFLAGS+=" -DNDEBUG "
  CXXFLAGS+=" -DNDEBUG "
 fi # with_debug
 AC_CHECK_HEADER([tbb/parallel_sort.h], [],
   AC_MSG_FAILURE([Couldn't find tbb/parallel_sort.h]),
   [#include <tbb/parallel_sort.h>])
fi # with_tbb

dnl -- debug flags -----------------------------------------------------------:
AC_ARG_WITH(debug, [AS_HELP_STRING([--with-debug],
    [compile with debug support. @<:@guess@:>@])
],,[with_debug=no])

dnl -- optimization ----------------------------------------------------------:
if test x"$with_debug" = "xyes"; then
 CFLAGS+=" -g -DDVBCSA_DEBUG -D_XOPEN_SOURCE=600"
 CXXFLAGS+=" -g -DDVBCSA_DEBUG -D_XOPEN_SOURCE=600"
 AC_DEFINE(__DEBUG__, 1, [build with debug support])
 AC_DEFINE(_DEBUG, 1, [build with debug support])
else
 CFLAGS+=" -O3 -funroll-loops -fomit-frame-pointer -D_XOPEN_SOURCE=600"
 CXXFLAGS+=" -O3 -D_XOPEN_SOURCE=600"
 dnl : -funroll-loops -fomit-frame-pointer
fi # with_debug

dnl -- build flags -----------------------------------------------------------:
configure_flags="CFLAGS: $CFLAGS CXXFLAGS: $CXXFLAGS"
AC_DEFINE_UNQUOTED([__BUILD_FLAGS__], ["$configure_flags"], [build flags])

dnl todo assembler output with source code :
dnl # create assembler code:
dnl c++ -S -fverbose-asm -g -O2 test.cc -o test.s
dnl # create asm interlaced with source lines:
dnl as -alhnd test.s > test.lst

m4_include([m4/report.m4])

dnl -- gen Makefile ----------------------------------------------------------:
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
