#!/usr/bin/env sh

CC="cc"
FLAGS="-g -Wall -Wextra -Werror -pedantic-errors -Wfatal-errors"
LIBS=""
SRCDIR="./src"
BUILDDIR="./out"
BIN="kvatch"

[ ! -d $BUILDDIR ] && mkdir $BUILDDIR

for f in $SRCDIR/*.c; do
	$CC $FLAGS -c -o $BUILDDIR/$(basename ${f%.*}).o $f
done
$CC $FLAGS -o $BIN $BUILDDIR/*.o $LIBS
