libcryptick
===========

libcryptick (pronounced "libcryptic") is a C library that provides an interface for cryptocurrency exchange rate tickers.

Using libcryptick in projects
----------------------------

Note that you will need a modern compiler supporting the C11 standard.

Copy the header and source file to the same desired location in your projects source tree and compile with:

    $CC -c -Wall -std=c11 -o cryptick.o cryptick.c

Then link with your program with:

    $CC $(pkg-config libcurl --libs --cflags) $(pkg-config jansson --libs --cflags) cryptick.o $YOUR_OBJECT_CODE -o $EXECUTABLE
