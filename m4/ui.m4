
dnl -- Check for --with-fltk -------------------------------------------------:
AC_ARG_WITH(fltk, [AS_HELP_STRING([--with-fltk],
    [compile with FLTK support. @<:@guess@:>@])
],,[with_fltk=guess])

AC_ARG_WITH(fltk-tab, [AS_HELP_STRING([--with-fltk-tab],
    [compile with FLTK Style Tab support])
],,[with_fltk_tab=no])

dnl -- Check for --with-gtk --------------------------------------------------:
AC_ARG_WITH(gtk, [AS_HELP_STRING([--with-gtk],
    [compile with GTK support (deprecated)])
],,[with_gtk=no])

dnl -- Check for --with-console ----------------------------------------------:
AC_ARG_WITH(console, [AS_HELP_STRING([--with-console],
    [compile with console support])
],,[with_console=no])

dnl -- fltk libs -------------------------------------------------------------:
dnl : guess : if test x"$with_fltk" = "xyes"; then

if test x"$with_console" = "xyes"; then
 AC_DEFINE(__CONSOLE__, 1, [build with console support])
 AC_CHECK_LIB([ncurses], [initscr])
 if test x"$ac_cv_lib_ncurses_initscr" = "xyes"; then
	AC_DEFINE(__NCURSES__, 1, [build with ncurses])
 fi
else # with_console
 if test x"$with_fltk" != "xno"; then
  AC_CHECK_LIB(fltk,main)
  if test x"$ac_cv_lib_fltk_main" = "xno"; then
   AC_MSG_ERROR([FLTK is not installed])
  fi
  if test x"$with_fltk_tab" = "xyes"; then
   AC_DEFINE(__FLTK_TAB__, 1, [build with FLTK TAB Style])
  fi
  AC_DEFINE(__FLTK__, 1, [build with FLTK support])
 else # with_fltk
  dnl -- gtk libs ------------------------------------------------------------:
  if test x"$with_gtk" = "xyes"; then
   dnl -- sanity check -------------------------------------------------------:
   if test x"$with_fltk" = "xyes"; then
    AC_MSG_ERROR([you are trying to compil with fltk and gtk!
     you must select only one of these : --with-fltk or --with-gtk])
   fi # with_fltk
   PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= 3.0])
   AC_DEFINE(__GTK__, 1, [build with GTK support])
  fi # with_gtk
 fi # with_fltk
fi # with_console
AM_CONDITIONAL(USE_FLTK, test x$with_fltk != "xno")
AM_CONDITIONAL(USE_FLTK_TAB, test x$with_fltk_tab = xyes)
AM_CONDITIONAL(USE_GTK, test x$with_gtk = xyes)

dnl -- sanity check ----------------------------------------------------------:
if test x"$with_fltk" = "xno"; then
 if test x"$with_gtk" = "xno"; then
  if test x"$with_console" = "xno"; then
   AC_MSG_ERROR([you must select only one of these options:
    --with-fltk or --with-gtk])
  fi # with_console
 fi # with_gtk
fi # with_fltk
