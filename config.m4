dnl $Id$
dnl config.m4 for extension converter

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(converter, for converter support,
dnl Make sure that the comment is aligned:
dnl [  --with-converter             Include converter support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(converter, whether to enable converter support,
dnl Make sure that the comment is aligned:
dnl [  --enable-converter           Enable converter support])

if test "$PHP_CONVERTER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-converter -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/converter.h"  # you most likely want to change this
  dnl if test -r $PHP_CONVERTER/$SEARCH_FOR; then # path given as parameter
  dnl   CONVERTER_DIR=$PHP_CONVERTER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for converter files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CONVERTER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CONVERTER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the converter distribution])
  dnl fi

  dnl # --with-converter -> add include path
  dnl PHP_ADD_INCLUDE($CONVERTER_DIR/include)

  dnl # --with-converter -> check for lib and symbol presence
  dnl LIBNAME=converter # you may want to change this
  dnl LIBSYMBOL=converter # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CONVERTER_DIR/$PHP_LIBDIR, CONVERTER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CONVERTERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong converter lib version or lib not found])
  dnl ],[
  dnl   -L$CONVERTER_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CONVERTER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(converter, converter.c, $ext_shared)
fi
