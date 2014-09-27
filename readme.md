# libcryptick

*lib-cryptocurrency-ticker*

	crtk_market market;
	crtk_market_get(&market, "mintpal", "btc", "doge");
	printf("%f\n", market->buy);

libcryptick is a library that retrieves cryptocurrency exchange rate tickers. It parses API configuration files at runtime, which means that exchanges or users can quickly write simple JSON files that explain their API to libcryptick and use it with no recompilation required.

libcryptick provides a simple interface, with one defined type and only a few public functions. The library stores currency exchange rates as both integers and real numbers, allowing you to preform easy and precise calculations on the results.

## dependencies

* [libcurl](http://curl.haxx.se/libcurl/)
* [jansson](http://www.digip.org/jansson/)
