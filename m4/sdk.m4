
dnl -- cuda libs -------------------------------------------------------------:
dnl # Provide your CUDA path with this :
AC_ARG_WITH(cuda, 
 [  --with-cuda=PREFIX      Prefix of your CUDA installation],
 [cuda_prefix=$withval], [cuda_prefix="/usr"])

if test x"$with_cuda" != "xno"; then
 AX_CHECK_CUDA
 AC_PROG_CUDA
 AC_DEFINE(__CUDA__, 1, [build with CUDA support])
fi # with_cuda
AM_CONDITIONAL(USE_CUDA, test x$with_cuda != xno)

dnl -- opencl libs -----------------------------------------------------------:
AC_ARG_WITH(opencl,[  --with-opencl    compile with OpenCL support], [], [])

if test x"$with_opencl" = "xyes"; then
dnl AC_PROG_OPENCL
 OPENCL_LDFLAGS="-lGL -lc -lm -lOpenCL -lGLU -lglut -lpthread"
 AC_DEFINE(__OPENCL__, 1, [build with OpenCL support])
 AC_SUBST(OPENCL_LDFLAGS)
fi # with_opencl
AM_CONDITIONAL(USE_OPENCL, test x$with_opencl = xyes)

dnl -- vulkan libs -----------------------------------------------------------:
AC_ARG_WITH(vulkan,[  --with-vulkan    compile with VULKAN support], [], [])

if test x"$with_vulkan" = "xyes"; then
dnl AC_PROG_OPENCL
 VULKAN_LDFLAGS="-lc -lm -lpthread"
 AC_DEFINE(__VULKAN__, 1, [build with VULKAN support])
 AC_SUBST(VULKAN_LDFLAGS)
fi # with_vulkan
AM_CONDITIONAL(USE_VULKAN, test x$with_vulkan = xyes)

dnl -- sanity check ----------------------------------------------------------:
dnl removed du to fact that we can use both now (maybe not now, but soon)
dnl goal is to use  GPU (CUDA or OpenCL) + CPU worker unit (OpenCL)
dnl if test x"$with_cuda" = "xno"; then
dnl  if test x"$with_opencl" = "xno"; then
dnl   AC_MSG_ERROR([Try ./configure --with-cuda=/usr])
dnl  fi # with_opencl
dnl fi # with_cuda
