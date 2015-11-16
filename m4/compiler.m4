dnl ===========================================================================
dnl Objective-C(++) related macros
dnl ===========================================================================
m4_if(m4_version_compare(m4_defn([m4_PACKAGE_VERSION]), [2.60]), -1,
[
	m4_define([AC_LANG_OBJECTIVEC],[AC_LANG(C)ac_ext=m])
	m4_define([AC_LANG_OBJECTIVECPLUSPLUS],[AC_LANG(C++)ac_ext=mm])
	m4_define([AC_LANG_OBJC],[AC_LANG(C)ac_ext=m])
	m4_define([AC_LANG_OBJCXX],[AC_LANG(C++)ac_ext=mm])
])

dnl [m4_define([AC_LANG(Objective C++)],
dnl 	[ac_ext=mm
dnl 		ac_cpp='$OBJCXXCPP $OBJCXXPPFLAGS'
dnl 		ac_compile='$OBJCXX -c $OBJCXXFLAGS $OBJCXXPPFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
dnl 		ac_link='$CXX -o conftest$ac_exeext $OBJCXXFLAGS $OBJCXXPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
dnl 	ac_compiler_gnu=$ac_cv_objcxx_compiler_gnu]
dnl )]


dnl Origin: https://stackoverflow.com/questions/1354996/need-an-autoconf-macro-that-detects-if-m64-is-a-valid-compiler-option
dnl Modified by Johann Felix v. Soden-Fraunhofen

dnl @synopsis CXX_FLAGS_CHECK [compiler flags] [result_variable]
dnl @summary check whether compiler supports given C++ flags or not
AC_DEFUN([CXX_FLAG_CHECK],
[dnl
   AC_MSG_CHECKING([if $CXX supports/ignores $1])
   AC_LANG_PUSH([C++])
  ac_saved_cxxflags="$CXXFLAGS"
  CXXFLAGS="-Werror $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [AC_MSG_RESULT([yes])
     $2=yes],
    [AC_MSG_RESULT([no])
     $2=no]
  )
  CXXFLAGS="$ac_saved_cxxflags"
  AC_LANG_POP([C++])
])

AC_DEFUN([CC_FLAG_CHECK],
[dnl
   AC_MSG_CHECKING([if $CC supports/ignores $1])
   AC_LANG_PUSH([C])
  ac_saved_ccflags="$CFLAGS"
  CFLAGS="-Werror $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [AC_MSG_RESULT([yes])
     $2=yes],
    [AC_MSG_RESULT([no])
     $2=no]
  )
  CFLAGS="$ac_saved_ccflags"
  AC_LANG_POP([C])
])

dnl ---------------------------------------------------------------------------
dnl usage :
dnl              $1    $2
dnl N_CHECK_LINK(name, testFnc)
AC_DEFUN([N_CHECK_LINK],
[dnl
 AC_MSG_CHECKING(for m4_toupper(AS_TR_SH($1)))
 AC_TRY_LINK([], $2, [
  have_$1=yes
  AC_MSG_RESULT([yes])
 ], [
  have_$1=no
  AC_MSG_RESULT([no])
 ])
 if test x"$m4_tolower(AS_TR_SH(have_$1))" = xyes; then
  AC_DEFINE(m4_toupper(AS_TR_SH(HAVE_$1)), 1, [build with $1 support])
 fi
])

dnl usage :
dnl              $1    $2        $3      $4       $5   $6)
dnl N_NEED_BUILD(name, path_src, header, testFnc, inc, lib)
AC_DEFUN([N_NEED_BUILD],
[dnl
 save_cxxflags="${CXXFLAGS}"
 save_libs="${LIBS}"
 CXXFLAGS="$5"
 LIBS="$6"
 AC_MSG_CHECKING(for m4_toupper(AS_TR_SH($1)))
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[
 $3
 void  testfunc() { $4 ; }
 ]])],[was_build_$1=yes],[was_build_$1=no])
 AC_MSG_RESULT($was_build_$1)
 AC_SUBST(was_build_$1)
 CXXFLAGS="${save_cxxflags}"
 LIBS="${save_libs}"
])

dnl usage :
dnl              $1   $2        $3      $4       $5 (optionnal) $6 include (optionnal) $7 (optionnal)
dnl N_BUILD_LIB(name, path_src, header, testFnc, conf_opts,     inc,                   link_deps)

AC_DEFUN([N_BUILD_LIB],
[dnl
 if test x"$m4_tolower(AS_TR_SH(with_$1))" = xyes; then
  dnl -- check for configure script --
  if test -f $2/configure ; then
   INSTALL_TEMP=$(echo $(pwd)/externals/install_$1);
   BUILD_TEMP=$(echo $(pwd)/externals/build_$1);
   TMP_CXXFLAGS="-I$2 -I${BUILD_TEMP} -I${BUILD_TEMP}/include -I${INSTALL_TEMP} -I${INSTALL_TEMP}/include $6"
   TMP_LIBS="-L${BUILD_TEMP}/.libs -L${INSTALL_TEMP}/lib -l$1 $7"

   N_NEED_BUILD($1, [$2], [$3], [$4], [$TMP_CXXFLAGS], [$TMP_LIBS])

   echo "was_build_$1 : ${was_build_$1} ----------------------------------------"

   if test x"${was_build_$1}" = xno; then
    echo "Build external lib : $1 ..."
    ( mkdir -p ${BUILD_TEMP} ${INSTALL_TEMP} && \
     cd ${BUILD_TEMP} && cp -a $2/* .  && \
      ./configure --prefix=${INSTALL_TEMP} ${N_COMPILATION_OPTS} $5 && \
      make -j${N_NBCORE} && make install )

    dnl -- retest build --
    N_NEED_BUILD($1, [$2], [$3], [$4], [$TMP_CXXFLAGS], [$TMP_LIBS])
   fi # was_build_$1

   if test x"${was_build_$1}" = xyes; then
    m4_toupper(AS_TR_SH($1))_CFLAGS=$(echo "$TMP_CXXFLAGS")
    m4_toupper(AS_TR_SH($1))_LIBS=$(echo "$TMP_LIBS")
    AC_DEFINE(m4_toupper(AS_TR_SH(WITH_$1)), 1, [build with $1 support])
    AC_DEFINE(m4_toupper(AS_TR_SH(__$1__)), 1, [build with $1 support])
    AC_SUBST(m4_toupper(AS_TR_SH($1))_CFLAGS)
    AC_SUBST(m4_toupper(AS_TR_SH($1))_LIBS)
   else
    AC_MSG_ERROR(m4_toupper(AS_TR_SH($1)) is not installed)
   fi # was_build_xxx
   unset INSTALL_TEMP BUILD_TEMP TMP_CXXFLAGS TMP_LIBS
  else
   AC_MSG_ERROR(m4_toupper(AS_TR_SH($1)) has no configure script)
  fi # has configure
 fi # with_name
])

dnl : same as previous, but without commnad line compiler, you need to pass it
dnl : (except for prefix)

AC_DEFUN([N_BUILD_LIB2],
[dnl
 if test x"$m4_tolower(AS_TR_SH(with_$1))" = xyes; then
  dnl -- check for configure script --
  if test -f $2/configure ; then
   INSTALL_TEMP=$(echo $(pwd)/externals/install_$1);
   BUILD_TEMP=$(echo $(pwd)/externals/build_$1);
   TMP_CXXFLAGS="-I$2 -I${BUILD_TEMP} -I${BUILD_TEMP}/include -I${INSTALL_TEMP} -I${INSTALL_TEMP}/include $6"
   TMP_LIBS="-L${BUILD_TEMP}/.libs -L${INSTALL_TEMP}/lib -l$1 $7"

   N_NEED_BUILD($1, [$2], [$3], [$4], [$TMP_CXXFLAGS], [$TMP_LIBS])

   echo "was_build_$1 : ${was_build_$1} ----------------------------------------"

   if test x"${was_build_$1}" = xno; then
    echo "Build external lib : $1 ..."
    ( mkdir -p ${BUILD_TEMP} ${INSTALL_TEMP} && \
     cd ${BUILD_TEMP} && cp -a $2/* . && make distclean ; \
      ./configure --prefix=${INSTALL_TEMP} $5 && make -j${N_NBCORE} && \
       make install ) || ( echo "Error to compil '$1'" ; exit 1 )

    dnl -- retest build --
    N_NEED_BUILD($1, [$2], [$3], [$4], [$TMP_CXXFLAGS], [$TMP_LIBS])
   fi # was_build_$1

   if test x"${was_build_$1}" = xyes; then
    m4_toupper(AS_TR_SH($1))_CFLAGS=$(echo "$TMP_CXXFLAGS")
    m4_toupper(AS_TR_SH($1))_LIBS=$(echo "$TMP_LIBS")
    AC_DEFINE(m4_toupper(AS_TR_SH(WITH_$1)), 1, [build with $1 support])
    AC_DEFINE(m4_toupper(AS_TR_SH(__$1__)), 1, [build with $1 support])
    AC_SUBST(m4_toupper(AS_TR_SH($1))_CFLAGS)
    AC_SUBST(m4_toupper(AS_TR_SH($1))_LIBS)
   else
    AC_MSG_ERROR(m4_toupper(AS_TR_SH($1)) is not installed)
   fi # was_build_xxx
   unset INSTALL_TEMP BUILD_TEMP TMP_CXXFLAGS TMP_LIBS
  else
   AC_MSG_ERROR(m4_toupper(AS_TR_SH($1)) has no configure script)
  fi # has configure
 fi # with_name
])

dnl : brief : used to generate makefile for lib for android
dnl usage :
dnl              $1   $2 (optionnal, must be relative: "\$(LOCAL_PATH)/include/toto")
dnl N_GEN_MAK_LIB(name, include)

AC_DEFUN([N_GEN_MAK_LIB],
[dnl
 if test x"${with_android}" = xyes; then
 if test x"$m4_tolower(AS_TR_SH(with_$1))" = xyes; then
  INSTALL_TEMP=$(echo $(pwd)/externals/install_$1);
  if test -d "${INSTALL_TEMP}"; then
   LINCLUDE="$2";
   LNAME="$1";
   if test x"${LINCLUDE}" != x ; then
    LINCLUDE="\$(LOCAL_PATH)/include/${LINCLUDE}"
   fi
   dnl
   echo "## auto generated makefile for lib ${LNAME}" > ${INSTALL_TEMP}/Android.mk
   echo "LOCAL_PATH:= \$(call my-dir)" >> ${INSTALL_TEMP}/Android.mk
   echo "include \$(CLEAR_VARS)" >> ${INSTALL_TEMP}/Android.mk
   echo "LOCAL_MODULE := $1" >> ${INSTALL_TEMP}/Android.mk
   echo "LOCAL_SRC_FILES := lib/lib${LNAME}.a" >> ${INSTALL_TEMP}/Android.mk
   echo "LOCAL_EXPORT_C_INCLUDES := \$(LOCAL_PATH)/include" >> ${INSTALL_TEMP}/Android.mk
   echo "include \$(PREBUILT_STATIC_LIBRARY)" >> ${INSTALL_TEMP}/Android.mk
  fi # valid destination directory
 fi # with_name
 fi # with_android
])

dnl # brief used to make a library using standalone tools
dnl #                     $1   $2        $3      $4       $5 (optionnal) $6 include (optionnal) $7 (optionnal)
dnl # N_BUILD_STANDALONE(name, path_src, header, testFnc, conf_opts,     inc,                   link_deps)

AC_DEFUN([N_BUILD_STANDALONE],
[dnl
 if test x"${with_android}" = xyes; then
  if test x"$m4_tolower(AS_TR_SH(with_$1))" = xyes; then
   dnl -- check for configure script --
   if test -f $2/configure ; then
    INSTALL_TEMP=$(echo $(pwd)/externals/install_$1);
    BUILD_TEMP=$(echo $(pwd)/externals/build_$1);
    TMP_CXXFLAGS="-I$2 -I${BUILD_TEMP} -I${BUILD_TEMP}/include -I${INSTALL_TEMP} -I${INSTALL_TEMP}/include $6"
    TMP_LIBS="-L${BUILD_TEMP}/.libs -L${INSTALL_TEMP}/lib -l$1 $7"

    N_NEED_BUILD($1, [$2], [$3], [$4], [$TMP_CXXFLAGS], [$TMP_LIBS])

    echo "was_build_$1 : ${was_build_$1} ----------------------------------------"

    if test x"${was_build_$1}" = xno; then
     echo "Build external lib $1 using ndk standalone: ${NDK_STANDALONE} ..."
     ( unset CC CXX ; mkdir -p ${BUILD_TEMP} ${INSTALL_TEMP} && \
      cd ${BUILD_TEMP} && cp -a $2/* .  && \
       PATH="${NDK_STANDALONE}:$PATH" ./configure --prefix=${INSTALL_TEMP} $5 && \
       make -j${N_NBCORE} && make install )

     dnl -- retest build --
     N_NEED_BUILD($1, [$2], [$3], [$4], [$TMP_CXXFLAGS], [$TMP_LIBS])
    fi # was_build_$1

    if test x"${was_build_$1}" = xyes; then
     m4_toupper(AS_TR_SH($1))_CFLAGS=$(echo "$TMP_CXXFLAGS")
     m4_toupper(AS_TR_SH($1))_LIBS=$(echo "$TMP_LIBS")
     AC_DEFINE(m4_toupper(AS_TR_SH(WITH_$1)), 1, [build with $1 support])
     AC_DEFINE(m4_toupper(AS_TR_SH(__$1__)), 1, [build with $1 support])
     AC_SUBST(m4_toupper(AS_TR_SH($1))_CFLAGS)
     AC_SUBST(m4_toupper(AS_TR_SH($1))_LIBS)
    else
     AC_MSG_ERROR(m4_toupper(AS_TR_SH($1)) is not installed)
    fi # was_build_xxx
    unset INSTALL_TEMP BUILD_TEMP TMP_CXXFLAGS TMP_LIBS
   else
    AC_MSG_ERROR(m4_toupper(AS_TR_SH($1)) has no configure script)
   fi # has configure
  fi # with_name
 fi # with_android
])
