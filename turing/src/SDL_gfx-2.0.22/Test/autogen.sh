#!/bin/sh
#

# Prep
make distclean
aclocal --force
libtoolize --force --copy
autoreconf -fvi
rm -rf autom4te.cache

# Setup
aclocal
autoheader
automake --foreign
autoconf

#./configure $*
echo "Now you are ready to run ./configure"
