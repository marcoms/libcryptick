libbtcapi
=========

libbtcapi is a C library that accesses both MtGox and BTC-E's Bitcoin tickers in multiple currencies

Using lbibbtcapi in projects
----------------------------

Copy the header and source file to the same desired location in your projects source tree and compile with:

    $CC -c -o btcapi.o btcapi.c

Then link with your program with:

    $CC $(pkg-config libcurl --libs --cflags) $(pkg-config jansson --libs --cflags) btcapi.o $YOUR_OBJECT_CODE -o $EXECUTABLE
