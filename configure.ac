AC_INIT([stree], [1.0])

AC_CONFIG_AUX_DIR([autoconf-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX

AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

LT_INIT

AC_ARG_ENABLE(
  packed-node,
  [  --enable-packed-node    pack tree nodes into 64 bits],
  [case "${enableval}" in
    yes) packed_node=true ;;
    no)  packed_node=false ;;
    *)   AC_MSG_ERROR([Bad value "${enableval}" for --enable-packed-node])
  esac], [packed_node=false])
AM_CONDITIONAL([PACKED_NODE], [test x$packed_node = xtrue])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "--------------------"
echo "Packed node: " $packed_node
