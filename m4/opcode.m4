dnl ====================== check for opcode ===================================
AC_C_CONST
AC_C_INLINE
AC_CHECK_SIZEOF(long)

CFLAGS+=" -O3"
CXXFLAGS+=" -O3"
has_dvbopcode=no
transpose_32=no

dnl ====================== Arch ===============================================
AC_ARG_ENABLE(corei5, [  --enable-corei5        Enable Core i5 support])

if test x"${enable_corei5}" = xyes; then
 AC_MSG_NOTICE(Enabling Core i5 in current arch/CFLAGS)
 CFLAGS+=" -march=corei5"
 AC_DEFINE(HAS_CORE_I5, 1, Build with Core i5 support.)
fi # 

AC_ARG_ENABLE(corei7, [  --enable-corei7        Enable Core i7 support])

if test x"${enable_corei7}" = xyes; then
 dnl automatic optimized flag :
 dnl GCC_OPT=$(gcc -### -E - -march=native 2>&1 | sed -r '/cc1/!d;s/(")|(^.* - )|( -mno-[^\ ]+)//g')
 dnl manually edited:
 GCC_OPT="--param l1-cache-size=32 --param l1-cache-line-size=64 "
 GCC_OPT+=" --param l2-cache-size=8192 -mtune=generic "
 GCC_OPT+=" -ftree-vectorize"
 CFLAGS+=" $GCC_OPT -march=corei7"
 AC_MSG_NOTICE(Enabling Core i7 in current arch/CFLAGS)
 AC_DEFINE(HAS_CORE_I7, 1, Build with Core i7 support.)
fi # 

dnl ====================== 3DNow! =============================================
AC_ARG_ENABLE(3dnow, [  --enable-3dnow          Enable 3DNow! support])

AC_MSG_CHECKING(for 3DNow! in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <mm3dnow.h>
__m64 testfunc(__m64 a, __m64 b) {
  return _m_pfadd(a, b);
}
]])],[has_3dnow=yes],[has_3dnow=no])
AC_MSG_RESULT($has_3dnow)

dnl ====================== MMX =====================================
AC_ARG_ENABLE(mmx, [  --enable-mmx            Enable MMX support])

AC_MSG_CHECKING(for MMX in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <mmintrin.h>
__m64 testfunc(__m64 a, __m64 b) {
  return _mm_add_pi8(a, b);
}
]])],[has_mmx=yes],[has_mmx=no])
AC_MSG_RESULT($has_mmx)

dnl ====================== SSE ==============================

AC_ARG_ENABLE(sse, [  --enable-sse            Enable SSE support])

AC_MSG_CHECKING(for SSE in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <xmmintrin.h>
__m128 testfunc(float *a, float *b) {
  return _mm_add_ps(_mm_loadu_ps(a), _mm_loadu_ps(b));
}
]])],[has_sse=yes],[has_sse=no])
AC_MSG_RESULT($has_sse)

dnl ====================== SSE2 =============================

AC_ARG_ENABLE(sse2, [  --enable-sse2           Enable SSE2 support])

AC_MSG_CHECKING(for SSE2 in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <emmintrin.h>
__m128d  testfunc(__m128d a, __m128d b) {
  return _mm_add_sd(a, b);
}
]])],[has_sse2=yes],[has_sse2=no])
AC_MSG_RESULT($has_sse2)

dnl ====================== SSE3 =============================

AC_ARG_ENABLE(sse3, [  --enable-sse3           Enable SSE3 support])

AC_MSG_CHECKING(for SSE3 in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <pmmintrin.h>
void* test() {
void* p;asm volatile("movsldup %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1"); return p;
}
]])],[has_sse3=yes],[has_sse3=no])
AC_MSG_RESULT($has_sse3)

dnl ====================== SSE4.1 ===========================

AC_ARG_ENABLE(sse41, [  --enable-sse41          Enable SSE4.1 support])

AC_MSG_CHECKING(for SSE4.1 in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <pmmintrin.h>
void* testfunc() {
void *p;asm volatile("pmaxsb %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1"); return p;
}
]])],[has_sse41=yes],[has_sse41=no])
AC_MSG_RESULT($has_sse41)

dnl ====================== SSE4.2 ===========================

AC_ARG_ENABLE(sse42, [  --enable-sse42          Enable SSE4.2 support])

AC_MSG_CHECKING(for SSE4.2 in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <pmmintrin.h>
void* testfunc() {
void *p;asm volatile("pcmpgtq %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1"); return p;
}
]])],[has_sse42=yes],[has_sse42=no])
AC_MSG_RESULT($has_sse42)

dnl ====================== SSE4a ============================

AC_ARG_ENABLE(sse4a, [  --enable-sse4a          Enable SSE4a support])

AC_MSG_CHECKING(for SSE4a in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <pmmintrin.h>
void* testfunc() {
void *p;asm volatile("insertq %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1"); return p;
}
]])],[has_sse4a=yes],[has_sse4a=no])
AC_MSG_RESULT($has_sse4a)

dnl ====================== AES ==============================

AC_ARG_ENABLE(aes, [  --enable-aes            Enable AES support])

AC_MSG_CHECKING(for AES in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <pmmintrin.h>
void* testfunc() {
void* p; asm volatile("aesenc %%xmm0,%%xmm0"::"r"(p):"xmm0"); return p;
}
]])],[has_aes=yes],[has_aes=no])
AC_MSG_RESULT($has_aes)

dnl ====================== pclmul ===========================

AC_ARG_ENABLE(pclmul, [  --enable-pclmul         Enable PCLMUL support])

AC_MSG_CHECKING(for PCLMUL in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <pmmintrin.h>
void*  testfunc() {
void *p;asm volatile("pcmpgtq %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1");return p;
}
]])],[has_pclmul=yes],[has_pclmul=no])
AC_MSG_RESULT($has_pclmul)

dnl ====================== avx ==============================

AC_ARG_ENABLE(avx, [  --enable-avx            Enable AVX support])

AC_MSG_CHECKING(for AVX in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <immintrin.h>
#include <x86intrin.h>
void* testfunc() {
void *p;asm volatile("vmovsldup %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1");return p;
}
]])],[has_avx=yes],[has_avx=no])
AC_MSG_RESULT($has_avx)

dnl for a good ref:
dnl https://software.intel.com/en-us/articles/soa-cloth-simulation-with-256-bit-intel-advanced-vector-extensions-intel-avx


dnl ====================== avx2 ==============================

AC_ARG_ENABLE(avx, [  --enable-avx2           Enable AVX2 support])

AC_MSG_CHECKING(for AVX2 in current arch/CFLAGS)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <immintrin.h>
#include <x86intrin.h>
void* testfunc() {
void *p;asm volatile("VBROADCASTSS %%xmm1,%%xmm0"::"r"(p):"xmm0", "xmm1");return p;
}
]])],[has_avx2=yes],[has_avx2=no])
AC_MSG_RESULT($has_avx2)

dnl ====================== NEON ==============================
if test x"${N_ARCH}" = xarm; then
 AC_ARG_ENABLE(neon, [  --enable-neon           Enable NEON support])

 AC_MSG_CHECKING(for Arm Neon in current arch/CFLAGS)
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[
 void test() { asm volatile("vqmovun.s64 d0, q1":::"d0"); }
 ]])],[has_neon=yes],[has_neon=no])
 AC_MSG_RESULT($has_neon)
fi # xarm

dnl ====================== set vars =========================

if test x"${has_mmx}" = xyes; then
 AC_DEFINE(__MMX__, 1, [build with mmx support])
 CFLAGS+=" -mmmx"
 CXXFLAGS+=" -mmmx"
 dnl case for dvbcsa :
 AC_DEFINE(DVBCSA_USE_MMX, 1, Using MMX bitslice.)
 has_dvbopcode=yes
fi
if test x"${has_sse}" = xyes; then
 AC_DEFINE(__SSE__, 1, [build with sse support])
 CFLAGS+=" -msse"
 CXXFLAGS+=" -msse"
fi
if test x"${has_sse2}" = xyes; then
 AC_DEFINE(__SSE2__, 1, [build with sse2 support])
 CFLAGS+=" -msse -msse2"
 CXXFLAGS+=" -msse -msse2"
 dnl case for dvbcsa :
 AC_DEFINE(DVBCSA_USE_SSE, 1, Using SSE2 bitslice.)
 has_dvbopcode=yes
fi
if test x"${has_sse3}" = xyes; then
 AC_DEFINE(__SSE3__, 1, [build with sse3 support])
 CFLAGS+=" -msse3 -mssse3"
 CXXFLAGS+=" -msse3 -mssse3"
 dnl case for dvbcsa :
 AC_DEFINE(DVBCSA_USE_SSSE3, 1, Using SSSE3 bitslice.)
 has_dvbopcode=yes
fi
if test x"${has_sse41}" = xyes; then
 AC_DEFINE(__SSE41__, 1, [build with sse4.1 support])
 CFLAGS+=" -msse4.1"
 CXXFLAGS+=" -msse4.1"
fi
if test x"${has_sse42}" = xyes; then
 AC_DEFINE(__SSE42__, 1, [build with sse4.2 support])
 CFLAGS+=" -msse4.2"
 CXXFLAGS+=" -msse4.2"
fi
if test x"${has_sse4a}" = xyes; then
 AC_DEFINE(__SSE4A__, 1, [build with sse4a support])
 CFLAGS+=" -msse4a"
 CXXFLAGS+=" -msse4a"
fi
if test x"${has_aes}" = xyes; then
 AC_DEFINE(__AES__, 1, [build with aes support])
 CFLAGS+=" -maes"
 CXXFLAGS+=" -maes"
fi
if test x"${has_pclmul}" = xyes; then
 AC_DEFINE(__PCLMUL__, 1, [build with pclmul support])
 CFLAGS+=" -mpclmul"
 CXXFLAGS+=" -mpclmul"
fi
if test x"${has_avx}" = xyes; then
 AC_DEFINE(__AVX__, 1, [build with avx support])
 CFLAGS+=" -mavx"
 CXXFLAGS+=" -mavx"
fi
if test x"${has_avx2}" = xyes; then
 AC_DEFINE(__AVX2__, 1, [build with avx2 support])
 CFLAGS+=" -mavx2"
 CXXFLAGS+=" -mavx2"
fi
if test x"${has_3dnow}" = xyes; then
 AC_DEFINE(__3DNOW__, 1, [build with 3DNow! support])
 CFLAGS+=" -m3dnow"
 CXXFLAGS+=" -m3dnow"
fi
if test x"${has_neon}" = xyes; then
 AC_DEFINE(__NEON__, 1, [build with Neon support])
 CFLAGS+=" -march=armv7-a -mfloat-abi=softfp -mfpu=neo"
 CXXFLAGS+=" -march=armv7-a -mfloat-abi=softfp -mfpu=neo"
 dnl case for dvbcsa :
 AC_DEFINE(DVBCSA_USE_NEON, 1, Using NEON bitslice.)
 has_dvbopcode=yes
fi
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)

dnl case for dvbcsa :
if test x"${has_dvbopcode}" = "xno"; then
 case $ac_cv_sizeof_long in
  8)
   AC_DEFINE(DVBCSA_USE_UINT64, 1, Using 64 bits integer bitslice.)
   AC_DEFINE(HAS_64BIT, 1, Using 64 bits.)
  ;;
  *)
   transpose_32=yes
   AC_DEFINE(DVBCSA_USE_UINT32, 1, Using 32 bits integer bitslice.)
  ;;
 esac
fi # has_dvbopcode

AM_CONDITIONAL(TRANSPOSE_32, test "$transpose_32" = "yes")
