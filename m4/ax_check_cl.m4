dnl --------------------------------------------------------------------------:
dnl OpenCL support for libtool                                                  :
dnl --------------------------------------------------------------------------:
AC_DEFUN([AC_PROG_OPENCL],[LT_LANG(OPENCL)])

AC_DEFUN([LT_PROG_OPENCL],
    [AC_ARG_VAR(OPENCL,[OPENCL compiler command])
     AC_ARG_VAR(OPENCL_FLAGS,[special compiler flags for the OPENCL compiler])

     AC_PATH_PROG(OPENCL, gcc, no, [$PATH:$opencl_path/bin])
])

# _LT_LANG_OPENCL_CONFIG([TAG])
# --------------------------
# Analogue to _LT_LANG_GCJ_CONFIG for OPENCL
AC_DEFUN([_LT_LANG_OPENCL_CONFIG],
  [AC_REQUIRE([LT_PROG_OPENCL])
   AC_LANG_PUSH(C++)

   # OPENCL file extensions
   ac_ext=cl
   objext=o
   _LT_TAGVAR(objext, $1)=$objext

   # Code to be used in simple compile tests
   lt_simple_compile_test_code="__global int var;"

   # Code to be used in simple link tests
   lt_simple_link_test_code="#include <stdio.h>
                             #include <stdlib.h>
                             #include <CL/cl.h>
                             int main() { clGetPlatformIDs(0,0,0); }"

   # ltmain only uses $GCC for tagged configurations so make sure $GCC is set.
   _LT_TAG_COMPILER

   # save warnings/boilerplate of simple test code
   _LT_COMPILER_BOILERPLATE
   _LT_LINKER_BOILERPLATE

   compiler=$GCC
   _LT_TAGVAR(compiler, $1)=$GCC
   _LT_TAGVAR(LD, $1)="$LD"
   _LT_CC_BASENAME([$compiler])

   # OPENCL did not exist at the time GCC didn't implicitly link libc in.
   dnl _LT_TAGVAR(archive_cmds_need_lc, $1)=no
   dnl _LT_TAGVAR(old_archive_cmds, $1)=$old_archive_cmds
   dnl _LT_TAGVAR(reload_flag, $1)=$reload_flag
   dnl _LT_TAGVAR(reload_cmds, $1)=$reload_cmds

   ## CAVEAT EMPTOR:
   ## There is no encapsulation within the following macros, do not change
   ## the running order or otherwise move them around unless you know exactly
   ## what you are doing...
   dnl if test -n "$compiler"; then
   dnl     _LT_COMPILER_NO_RTTI($1)
   dnl     # building shared with nvcc not there in libtool
   dnl     # $(OPENCLC) -x cl -cl-std=CL1.1 -cl-auto-vectorize-enable -emit-gcl $<
   dnl     _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Xlinker '
   dnl     _LT_TAGVAR(lt_prog_compiler_static, $1)='-Xcompiler -static'
   dnl     _LT_TAGVAR(lt_prog_compiler_pic, $1)='-Xcompiler -fPIC'
   dnl     _LT_COMPILER_C_O($1)
   dnl     _LT_COMPILER_FILE_LOCKS($1)
   dnl     _LT_LINKER_SHLIBS($1)
   dnl     _LT_LINKER_HARDCODE_LIBPATH($1)
   dnl 
   dnl     _LT_CONFIG($1)
   dnl fi

   AC_LANG_POP
])
