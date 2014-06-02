/*
	Copyright (C) 2013-2014 Marco Scannadinari <m@scannadinari.co.uk>

	This file is part of libcryptick.

	libcryptick is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libcryptick is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libcryptick.  If not, see <http://www.gnu.org/licenses/>.
*/

#if MT_GOX_API
#define API_URL_CURRCY_POS 32  // position to insert currency
#elif BTC_E_API
#define API_URL_CURRCY_POS  28
#endif

#include <curl/curl.h>
#include <ctype.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cryptick.h"
#include "cryptick_currencies.h"

btc_err_t btc_fill_rates(btc_rates_t *const rates, const char *const currcy) {
	btc_err_t api_err;
	char *json;

	json = malloc(1600);
	if(!json) abort();

	api_err = _btc_get_json(json, rates, currcy);
	if(api_err.err) {
		free(json);
		return api_err;
	}

	api_err = _btc_parse_json(rates, json);
	free(json);

	return api_err;
}

btc_err_t _btc_get_json(char const *json, btc_rates_t *const rates, const char *const currcy) {
	#ifdef MT_GOX_API
	char api_url[] = "https://data.mtgox.com/api/2/BTCxxx/money/ticker_fast";
	#elif defined(BTC_E_API)
	char api_url[] = "https://btc-e.com/api/2/btc_xxx/ticker";
	#endif

	btc_err_t api_err;
	char mod_currcy[3 + 1];
	CURL *handle;
	CURLcode result;
	bool valid_currcy;

	api_err.err = false;

	strcpy(mod_currcy, currcy);

	handle = curl_easy_init();
	if(!handle) {
		api_err.err = true;
		strcpy(api_err.errstr, "unable to initialise libcurl session");
		return api_err;
	}

	valid_currcy = false;

	curl_global_init(CURL_GLOBAL_ALL);

	// length check
	if(strlen(currcy) != 3) {
		api_err.err = true;
		strcpy(api_err.errstr, "bad currency length");
		return api_err;
	}

	// uppercases the currency string
	for(uint_fast8_t i = 0; i < ((sizeof mod_currcy[0]) * (sizeof mod_currcy)); ++i) mod_currcy[i] = toupper(mod_currcy[i]);

	// validation
	for(uint_fast8_t i = 0; i < ((sizeof btc_currencies / sizeof btc_currencies[0])); ++i) {
		if(!strcmp(mod_currcy, btc_currencies[i].name)) {
			valid_currcy = true;
			strcpy(rates -> currcy.name, btc_currencies[i].name);
			strcpy(rates -> currcy.sign, btc_currencies[i].sign);
			rates -> currcy.sf = btc_currencies[i].sf;
			break;
		}
	}

	if(!valid_currcy) {
		api_err.err = true;
		strcpy(api_err.errstr, "invalid currency");
		return api_err;
	}

	#ifdef BTC_E_API
	// lowercases the currency string
	for(uint_fast8_t i = 0; i < ((sizeof mod_currcy[0]) * (sizeof mod_currcy)); ++i) mod_currcy[i] = tolower(mod_currcy[i]);
	#endif

	for(uint_fast8_t i = API_URL_CURRCY_POS, j = 0; i < (API_URL_CURRCY_POS + 3); ++i, ++j) api_url[i] = mod_currcy[j];

	curl_easy_setopt(handle, CURLOPT_URL, api_url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _btc_write_data);  // sets the function to call
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);  // sets the data to be given to the function

	result = curl_easy_perform(handle);  // performs the request, stores result
	if(result != CURLE_OK) {
		api_err.err = true;
		strcpy(api_err.errstr, curl_easy_strerror(result));
		return api_err;
	}

	curl_easy_cleanup(handle);
	curl_global_cleanup();

	return api_err;
}

btc_err_t _btc_parse_json(btc_rates_t *const rates, const char *const json) {
	btc_err_t api_err;
	#ifdef MT_GOX_API
	json_t *buy;
	json_t *data;
	#endif
	json_error_t json_error;
	json_t *root;
	#ifdef MT_GOX_API
	json_t *sell;
	#elif defined(BTC_E_API)
	json_t *ticker;
	#endif

	api_err.err = false;

	root = json_loads(json, 0, &json_error);
	if(!root) {
		api_err.err = true;
		strcpy(api_err.errstr, json_error.text);
		return api_err;
	}

	#ifdef MT_GOX_API
	data = json_object_get(root, "data");
	if(!data) {
		api_err.err = true;
		strcpy(api_err.errstr, "couldn't get JSON object");
		return api_err;
	}
	#elif defined(BTC_E_API)
	ticker = json_object_get(root, "ticker");
	if(!ticker) {
		api_err.err = true;
		strcpy(api_err.errstr, "couldn't get JSON object");
		return api_err;
	}
	#endif

	#ifdef MT_GOX_API
	buy = json_object_get(data, "buy");
	sell = json_object_get(data, "sell");
	if(!buy || !sell) {
		api_err.err = true;
		strcpy(api_err.errstr, "couldn't get JSON object");
		return api_err;
	}

	rates -> result = strcmp(json_string_value(json_object_get(root, "result")), "success") ? false : true;
	#endif

	// stores trade values as int and float
	#ifdef MT_GOX_API
	rates -> buy = atoi(json_string_value(json_object_get(buy, "value_int")));  // MtGox uses strings for their prices se we have to convert it to a string
	rates -> buyf = ((double) rates -> buy / (double) rates -> currcy.sf);
	rates -> sell = atoi(json_string_value(json_object_get(sell, "value_int")));
	rates -> sellf = ((double) rates -> sell / (double) rates -> currcy.sf);
	#elif defined(BTC_E_API)
	rates -> buyf = (double) json_number_value(json_object_get(ticker, "buy"));  // no integer value in BTC-e's API so we have to calculate it manually
	rates -> buy = (int) (rates -> buyf * rates -> currcy.sf);
	rates -> sellf = (double) json_number_value(json_object_get(ticker, "sell"));
	rates -> sell = (int) (rates -> sellf * rates -> currcy.sf);
	#endif

	json_decref(root);
	return api_err;
}

size_t _btc_write_data(
	const char *const buffer,
	const size_t size,
	const size_t nmemb,
	const void *const data
) {
	strcpy((char *) data, buffer);
	return (size * nmemb);
}
