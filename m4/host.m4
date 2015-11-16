dnl
dnl check for host
dnl

AS_ECHO_N("checking for OS name ... ")

HOST_N="unknown"
LINUX=no
IOS=no
WII=no
WIIU=no
PS3=no
PS4=no
ANDROID=no
WINDOWS=no
MINGW=no
CYGWIN=no
MACOSX=no
FREEBSD=no
OPENBSD=no
AIX=no
SOLARIS=no
HPUX=no
OSF=no
SCOUNIX=no
IRIX=no
N_ARCH=""
if test x"$target" = "x"; then
 CHK_TARGET=$host
else
 CHK_TARGET=$target
fi

case ${CHK_TARGET} in 
 *x86_64*)
  N_ARCH="x86_64"
 ;;
 *i386*)
  N_ARCH="i386"
 ;;
 *i586*)
  N_ARCH="i586"
 ;;
 *i686*)
  N_ARCH="i686"
 ;;
 *arm*)
  N_ARCH="arm"
 ;;
esac

case ${CHK_TARGET} in
 *mingw*)
	WINDOWS=yes
	MINGW=yes
	HOST_N="Windows ${N_ARCH}"
	CFLAGS="-D__MSVCRT_VERSION__=0x0601 $CFLAGS"
	CPPFLAGS="-DFTRUNCATE_DEFINED=1 $CPPFLAGS"
 ;;
 *android*)
    HOST_N="Android ${N_ARCH}"
    ANDROID=yes
 ;;
 *linux*)
	LINUX=yes
	HOST_N="Linux ${N_ARCH}"
 ;;
 *darwin*)
	dnl Apple OS X
	APPLE_ARCH=""
	APPLE_CFLAGS="-fno-common -mmacosx-version-min=10.7 "
	if test x"${with_x86_64}" = xyes; then
		APPLE_ARCH+="x86_64 "
		N_ARCH="x86_64"
	fi
	if test x"${with_i386}" = xyes; then
		APPLE_ARCH+="i386 "
		N_ARCH="i386"
	fi
	if test x"$APPLE_ARCH" = x ; then
		APPLE_ARCH+="i386 "
		N_ARCH="i386"
	fi
	MACOSX=yes
	HOST_N="Apple OS X ( $APPLE_ARCH)"
 ;;
 *ios*)
  IOS=yes
  HOST_N="iOS"
 ;;
 *freebsd*)
	dnl FreeBSD
	FREEBSD=yes
	HOST_N=FreeBSD
 ;;
 *openbsd*)
	dnl OpenBSD
	OPENBSD=yes
	HOST_N=OpenBSD
 ;;
 *aix*)
	dnl AIX OS (IBM)
	AIX=yes
	HOST_N="AIX OS (IBM)"
 ;;
 *solaris*)
	dnl Oracle Solaris
	SOLARIS=yes
	HOST_N="Oracle Solaris"
 ;;
 *hpux*)
	dnl Hewlett Packard UniX
	HPUX=yes
	HOST_N="Hewlett Packard UniX"
 ;;
 *osf*)
	dnl Tru64 UNIX
	OSF=yes
	HOST_N="Tru64 UNIX"
 ;;
 *sco*)
	dnl SCO UnixWar
	SCOUNIX=yes
	HOST_N="SCO UnixWar"
 ;;
 *irix*)
    IRIX=yes
    HOST_N="SGI Irix"
 ;;
 *cygwin*)
    CYGWIN=yes
    WINDOWS=yes
    HOST_N="CygWin"
 ;;
esac

AM_CONDITIONAL(ANDROID, test x$ANDROID = xyes)
AM_CONDITIONAL(LINUX, test x$LINUX = xyes)
AM_CONDITIONAL(WINDOWS, test x$WINDOWS = xyes)
AM_CONDITIONAL(MINGW, test x$MINGW = xyes)
AM_CONDITIONAL(MACOSX, test x$MACOSX = xyes)
AM_CONDITIONAL(AIX, test x$AIX = xyes)
AM_CONDITIONAL(FREEBSD, test x$FREEBSD = xyes)
AM_CONDITIONAL(SOLARIS, test x$SOLARIS = xyes)
AM_CONDITIONAL(HPUX, test x$HPUX = xyes)
AM_CONDITIONAL(OSF, test x$OSF = xyes)
AM_CONDITIONAL(SCOUNIX, test x$SCOUNIX = xyes)
AM_CONDITIONAL(OPENBSD, test x$OPENBSD = xyes)
AM_CONDITIONAL(IRIX, test x$IRIX = xyes)
AM_CONDITIONAL(CYGWIN, test x$CYGWIN = xyes)
AM_CONDITIONAL(IOS, test x$IOS = xyes)
AM_CONDITIONAL(WII, test x$WII = xyes)
AM_CONDITIONAL(WIIU, test x$WIIU = xyes)
AM_CONDITIONAL(PS3, test x$PS3 = xyes)
AM_CONDITIONAL(PS4, test x$PS4 = xyes)

dnl ==================== set vars =============================

if test x"${ANDROID}" = xyes; then
 AC_DEFINE(__ANDROID__, 1, [build with android support])
fi
if test x"${MINGW}" = xyes; then
 AC_DEFINE(__WIN32__, 1, [build with windows support])
 AC_DEFINE(__MINGW__, 1, [build with mingw support])
fi
if test x"${LINUX}" = xyes; then
 AC_DEFINE(__LINUX__, 1, [build with linux support])
fi
if test x"${IOS}" = xyes; then
 AC_DEFINE(__IOS__, 1, [build with iOS support])
fi
if test x"${WII}" = xyes; then
 AC_DEFINE(__WII__, 1, [build with Wii support])
fi
if test x"${WIIU}" = xyes; then
 AC_DEFINE(__WIIU__, 1, [build with WiiU support])
fi
if test x"${PS3}" = xyes; then
 AC_DEFINE(__PS3__, 1, [build with PS3 support])
fi
if test x"${PS4}" = xyes; then
 AC_DEFINE(__PS4__, 1, [build with PS4 support])
fi
if test x"${MACOSX}" = xyes; then
 AC_DEFINE(__MACOSX__, 1, [build with macosx support])
 AC_DEFINE(__APPLE__, 1, [build with macosx support])
fi

AS_ECHO("$HOST_N")
