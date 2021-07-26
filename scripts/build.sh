#!/usr/bin/env sh

CC="cc"
FLAGS="-Wall -Wextra -Werror -pedantic-errors -Wfatal-errors"
LIBS=""
SRCDIR="./src"
BUILDDIR="./out"
BIN="kvatch"

[ ! -d $BUILDDIR ] && mkdir $BUILDDIR

if [ "$1" = "--debug" ]; then
	FLAGS="${FLAGS} -g -fsanitize=address"
	LIBS="${LIBS} -static-libasan"
fi

for f in $SRCDIR/*.c; do
	$CC $FLAGS -c -o $BUILDDIR/$(basename ${f%.*}).o $f $LIBS
done
$CC $FLAGS -o $BIN $BUILDDIR/*.o $LIBS
