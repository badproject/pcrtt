##### 
#
# SYNOPSIS
#
# AX_CHECK_CUDA
#
# DESCRIPTION
#
# Figures out if CUDA Driver API/nvcc is available, i.e. existence of:
# 	cuda.h
#   libcuda.so
#   nvcc
#
# If something isn't found, fails straight away.
#
# Locations of these are included in 
#   CUDA_CFLAGS and 
#   CUDA_LDFLAGS.
# Path to nvcc is included as
#   NVCC_PATH
# in config.h
# 
# The author is personally using CUDA such that the .cu code is generated
# at runtime, so don't expect any automake magic to exist for compile time
# compilation of .cu files.
#
# LICENCE
# Public domain
#
# AUTHOR
# wili
#
##### 

AC_DEFUN([AX_CHECK_CUDA], [
dnl # Setting the prefix to the default if only --with-cuda was given:
if test "$cuda_prefix" == "yes"; then
	if test "$withval" == "yes"; then
		cuda_prefix="/usr"
	fi
fi

dnl # Checking for nvcc:
AC_MSG_CHECKING([nvcc in $cuda_prefix/bin])
if test -x "$cuda_prefix/bin/nvcc"; then
	AC_MSG_RESULT([found])
	AC_DEFINE_UNQUOTED([NVCC_PATH], ["$cuda_prefix/bin/nvcc"], [Path to nvcc binary])
	NVCC=$cuda_prefix/bin/nvcc
else
	AC_MSG_RESULT([not found!])
	AC_MSG_FAILURE([nvcc was not found in $cuda_prefix/bin])
fi
AC_SUBST([NVCC])

dnl # We need to add the CUDA search directories for header and lib searches:

dnl # Saving the current flags:
ax_save_CFLAGS="${CFLAGS}"
ax_save_LDFLAGS="${LDFLAGS}"

CUDA_CFLAGS="-I$cuda_prefix/include "
CUDA_LDFLAGS="-L$cuda_prefix/lib "
CUDA_LDFLAGS+=" -lcuda "
CUDA_LDFLAGS+=" -L${CUDAPATH}/lib64 -L${CUDAPATH}/lib "
CUDA_LDFLAGS+=" -Wl,-rpath=${CUDAPATH}/lib/x86_64-linux-gnu -Wl,-rpath=${CUDAPATH}/lib64 -Wl,-rpath=${CUDAPATH}/lib "
CUDA_LDFLAGS+=" -lcublas -lcudart "

NVCC_FLAGS="--generate-code arch=compute_20,code=sm_20 "
NVCC_FLAGS+=" --ptxas-options=-v -O3 -G -g "

dnl # Announcing the new variables:
AC_SUBST([CUDA_CFLAGS])
AC_SUBST([CUDA_LDFLAGS])

CFLAGS="$CUDA_CFLAGS $CFLAGS"
LDFLAGS="$CUDA_LDFLAGS $LDFLAGS"

dnl # And the header and the lib:
AC_CHECK_HEADER([cuda.h], [], AC_MSG_FAILURE([Couldn't find cuda.h]), [#include <cuda.h>])
dnl AC_CHECK_LIB([cudart], [cuInit], [], AC_MSG_FAILURE([Couldn't find libcudart]))

dnl # Returning to the original flags:
CFLAGS=${ax_save_CFLAGS}
LDFLAGS=${ax_save_LDFLAGS}

])

dnl ---------------------------------------------------------------------------

dnl -*- mode: autoconf -*-
dnl Copyright (C) 2013,2014 The ESPResSo project
dnl  
dnl This file is part of ESPResSo.
dnl  
dnl ESPResSo is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl  
dnl ESPResSo is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl  
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>. 
dnl
dnl CUDA support for libtool
dnl
AC_DEFUN([AC_PROG_CUDA],[LT_LANG(CUDA)])

AC_DEFUN([LT_PROG_CUDA],
    [AC_ARG_VAR(NVCC,[NVIDIA CUDA compiler command])
     AC_ARG_VAR(NVCC_FLAGS,[special compiler flags for the NVIDIA CUDA compiler])

     AC_PATH_PROG(NVCC, nvcc, no, [$PATH:$cuda_path/bin])

     # MAC nvcc stays 32 bit, even if the rest is 64 bit
     case $target in
         x86_64-apple-darwin*)
                 NVCC_FLAGS="$NVCC_FLAGS -m64";;
     esac
])

# _LT_LANG_CUDA_CONFIG([TAG])
# --------------------------
# Analogue to _LT_LANG_GCJ_CONFIG for CUDA
AC_DEFUN([_LT_LANG_CUDA_CONFIG],
  [AC_REQUIRE([LT_PROG_CUDA])
   AC_LANG_PUSH(C++)

   # CUDA file extensions
   ac_ext=cu
   objext=o
   _LT_TAGVAR(objext, $1)=$objext

   # Code to be used in simple compile tests
   lt_simple_compile_test_code="static __device__ __constant__ int var;"

   # Code to be used in simple link tests
   lt_simple_link_test_code="#include <cuda.h>
                             int main() { cudaGetDevice(0); }"

   # ltmain only uses $CC for tagged configurations so make sure $CC is set.
   _LT_TAG_COMPILER

   # save warnings/boilerplate of simple test code
   _LT_COMPILER_BOILERPLATE
   _LT_LINKER_BOILERPLATE

   # Allow CC to be a program name with arguments.
   lt_save_CC="$CC"
   lt_save_GCC=$GCC

   # nvcc interface is not gcc-like (but can steer gcc)
   GCC=no
   CC=$NVCC
   compiler=$CC
   _LT_TAGVAR(compiler, $1)=$CC
   _LT_TAGVAR(LD, $1)="$LD"
   _LT_CC_BASENAME([$compiler])

   # CUDA did not exist at the time GCC didn't implicitly link libc in.
   _LT_TAGVAR(archive_cmds_need_lc, $1)=no
   _LT_TAGVAR(old_archive_cmds, $1)=$old_archive_cmds
   _LT_TAGVAR(reload_flag, $1)=$reload_flag
   _LT_TAGVAR(reload_cmds, $1)=$reload_cmds

   ## CAVEAT EMPTOR:
   ## There is no encapsulation within the following macros, do not change
   ## the running order or otherwise move them around unless you know exactly
   ## what you are doing...
   if test -n "$compiler"; then
       _LT_COMPILER_NO_RTTI($1)
       # building shared with nvcc not there in libtool
       _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Xlinker '
       _LT_TAGVAR(lt_prog_compiler_static, $1)='-Xcompiler -static'
       _LT_TAGVAR(lt_prog_compiler_pic, $1)='-Xcompiler -fPIC'
       _LT_COMPILER_C_O($1)
       _LT_COMPILER_FILE_LOCKS($1)
       _LT_LINKER_SHLIBS($1)
       _LT_LINKER_HARDCODE_LIBPATH($1)

       _LT_CONFIG($1)
   fi

   AC_LANG_POP

   GCC=$lt_save_GCC
   CC="$lt_save_CC"
])
