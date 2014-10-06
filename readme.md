# libcryptick

*lib-cryptocurrency-ticker*

	crtk_market market;
	crtk_market_get(&market, "mintpal", "btc", "doge");

libcryptick is a library that retrieves cryptocurrency exchange rate tickers. It parses API configuration files at runtime, which means that exchanges or users can quickly write simple JSON files that explain their API to libcryptick and use it with no recompilation required.

Example configuration file:

	{
		    "name": "MintPal"
		    , "url": ["https://api.mintpal.com/v2/market/stats/", "{c}", "/", "{e}"]
		    , "status_success": "success"
		    , "number_format": "string"
		    , "paths": {
		            "code": ["data", "code"]
		            , "status": ["status"]
		            , "status_desc": ["message"]
		            , "buy": ["data", "top_ask"]
		            , "sell": ["data", "top_bid"]
		    }
	}

libcryptick provides a simple interface, with only a few public functions and types defined. The library stores currency exchange rates as both integers and real numbers, allowing you to preform easy and precise calculations on the results.

## dependencies

* [libcurl](http://curl.haxx.se/libcurl/)
* [jansson](http://www.digip.org/jansson/)

## compiling

Compile with:

	$CC cryptick.c `pkg-config libcurl --libs --cflags` `pkg-config jansson --libs --cflags` -c -o cryptick.o
