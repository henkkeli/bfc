#!/bin/bash

on_error()
{
    echo "error on line $1"
    exit 1
}
trap 'on_error $LINENO' ERR

check_hello()
{
    EXPECTED_OUTPUT="Hello World!"
    ACTUAL_OUTPUT="`$1`"
    STATUS="$?"
    if [ "$STATUS" -ne "0" ] ; then
        echo "$1 returned $STATUS"
        return 1
    fi
    if [ "$EXPECTED_OUTPUT" != "$ACTUAL_OUTPUT" ] ; then
        echo "output of $1 is:"
        echo "$ACTUAL_OUTPUT"
        echo "output of $1 should be:"
        echo "$EXPECTED_OUTPUT"
        return 1
    fi
    return 0
}

BUILDDIR="$PWD/build"
TESTDIR="$PWD/test"
OUTDIR="$PWD/out"
BFC="$BUILDDIR/bfc"

[   -d "$BUILDDIR" ]
[   -d "$TESTDIR" ]
[ ! -d "$OUTDIR" ]
[   -f "$BFC" ]

mkdir "$OUTDIR" && cd "$OUTDIR"

"$BFC" "$TESTDIR"/hello.bf
check_hello ./a.out
rm a.out
echo "test 1 ok (link)"

"$BFC" "$TESTDIR"/hello.bf -o hello
check_hello ./hello
rm hello
echo "test 2 ok (link with outfile)"

"$BFC" -c "$TESTDIR"/hello.bf
gcc -o hello hello.o
check_hello ./hello
rm hello hello.o
echo "test 3 ok (no-link)"

"$BFC" -c "$TESTDIR"/hello.bf -o hello.o
gcc -o hello hello.o
check_hello ./hello
rm hello hello.o
echo "test 4 ok (no-link with outfile)"

"$BFC" -S "$TESTDIR"/hello.bf
gcc -o hello hello.s
check_hello ./hello
rm hello hello.s
echo "test 5 ok (no-assemble)"

"$BFC" -S "$TESTDIR"/hello.bf -o hello.s
gcc -o hello hello.s
check_hello ./hello
rm hello hello.s
echo "test 6 ok (no-assemble with outfile)"

"$BFC" -c --symbol f -o f.o "$TESTDIR"/hello.bf
gcc -o hello f.o "$TESTDIR"/testmain.c
check_hello ./hello
rm hello f.o
echo "test 7 ok (link with c)"

"$BFC" -c --symbol f -o f.o "$TESTDIR"/hello.bf
g++ -o hello f.o "$TESTDIR"/testmain.cpp
check_hello ./hello
rm hello f.o
echo "test 8 ok (link with c++)"

"$BFC" "$TESTDIR"/hello.bf -o hello --no-optimize
check_hello ./hello
rm hello
echo "test 9 ok (no optimization)"

cd ..
rmdir out

echo "all tests ok"
