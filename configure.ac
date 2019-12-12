AC_INIT([exread], [0.1])
AC_CONFIG_SRCDIR([src/bigint.cpp])
AM_INIT_AUTOMAKE

dnl noext: use -std=c++11 rather than -std=gnu++11
AX_CXX_COMPILE_STDCXX_11([noext])

dnl define AX_CXX_COMPILE_STDCXX_11 macro (sometimes required for mac)
AC_CONFIG_MACRO_DIR([acinclude.d])

AC_PROG_CXX
AC_PROG_RANLIB

AC_OUTPUT(Makefile src/Makefile tests/Makefile)
