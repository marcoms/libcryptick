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

#include <curl/curl.h>
#include <stdbool.h>
#include <string.h>

#include "cryptick-priv.h"

#define CRTK__FORMAT_SPECIFIER_EXCHANGE "{e}"
#define CRTK__FORMAT_SPECIFIER_COIN "{c}"

#define CRTK__UNIT_FACTOR 1e10  // base unit == 1e8 + fractional margin

crtk_error crtk_market_get(crtk_market *const market, const char *const api, const char *const exchange, const char *const coin) {
	crtk_error lib_error;

	struct crtk__api_config api_config;
	if((lib_error = crtk__api_config_parse(&api_config, api, exchange, coin)).error) return lib_error;

	char json[65536];
	if((lib_error = crtk__url_get(json, api_config.url)).error) return lib_error;

	if((lib_error = crtk__api_parse(market, json, &api_config)).error) return lib_error;

	return lib_error;
}

static void crtk__error_set(crtk_error *const err, const enum crtk_error_value value, const char *const desc) {
	err->error = value;
	strcpy(err->desc, desc);
}

static crtk_error crtk__api_config_parse(struct crtk__api_config *const api_config, const char *const api, const char *const exchange, const char *const coin) {
	crtk_error lib_error = { .error = CRTK_ERROR_NONE };

	char api_config_path[32] = "/etc/libcryptick/";  // TODO be a bit more dynamic?
	strcat(api_config_path, api);
	strcat(api_config_path, ".json");

	json_error_t json_error;
	json_t *root                     = json_load_file(api_config_path, 0, &json_error);
	json_t *name                     = json_object_get(root, "name");
	json_t *url                      = json_object_get(root, "url");
	json_t *status_success           = json_object_get(root, "status_success");
	json_t *number_format            = json_object_get(root, "number_format");
	json_t *paths                    = json_object_get(root, "paths");
	json_t *paths_status_desc        = json_object_get(paths, "status_desc");
	json_t *paths_status             = json_object_get(paths, "status");
	json_t *paths_buy                = json_object_get(paths, "buy");
	json_t *paths_sell               = json_object_get(paths, "sell");

	// check spec conformance

	if(
		!(
			root && name && url && number_format && paths && paths_buy && paths_sell
			&& (json_is_string(url) || json_is_array(url))
			&& json_is_string(number_format)
			&& json_is_object(paths)
			&& json_is_array(paths_buy)
			&& json_is_array(paths_sell)
		)
		|| (paths_status && !status_success)
		|| (status_success && !(
			json_is_string(status_success)
			|| json_is_integer(status_success)
			|| json_is_boolean(status_success)
		))
	) {
		crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": see https://github.com/marcoms/libcryptick-api-config-spec");
		return lib_error;
	}

	// name
	strcpy(api_config->name, json_string_value(name));

	// url

	if(json_is_array(url)) {  // format array
		crtk__format_array_parse(api_config->url, url, exchange, coin);
	} else if(json_is_string(url)) {
		strcpy(api_config->url, json_string_value(url));
	} else {
		crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": invalid type for url");
		return lib_error;
	}

	// status_success

	if(status_success) switch(json_typeof(status_success)) {
		case JSON_STRING:
			api_config->status_success.type = CRTK__STATUS_TYPE_STRING;
			strcpy(api_config->status_success.value.string, json_string_value(status_success));
			break;

		case JSON_INTEGER:
			api_config->status_success.type = CRTK__STATUS_TYPE_INTEGER;
			api_config->status_success.value.integer = (int16_t) json_integer_value(status_success);
			break;

		case JSON_TRUE:
		case JSON_FALSE:
			api_config->status_success.type = CRTK__STATUS_TYPE_BOOLEAN;
			api_config->status_success.value.boolean = (json_typeof(status_success) == JSON_TRUE ? true : false);
			break;

		default:
			crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": invalid type for status_success");
			return lib_error;
			break;
	}

	// number_format

	const char *const number_format_str = json_string_value(number_format);
	if(!strcmp("string", number_format_str)) {
		api_config->number_format = CRTK__NUMBER_FORMAT_STRING;
	} else if(!strcmp("real", number_format_str)) {
		api_config->number_format = CRTK__NUMBER_FORMAT_REAL;
	} else {
		crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": invalid value for number_format");
		return lib_error;
	}

	// paths

	// paths.status
	if(paths_status) crtk__pc_array_parse(api_config->paths.status, paths_status, exchange, coin);

	// paths.status_desc
	if(paths_status_desc) crtk__pc_array_parse(api_config->paths.status_desc, paths_status_desc, exchange, coin);

	// paths.buy
	crtk__pc_array_parse(api_config->paths.buy, paths_buy, exchange, coin);

	// paths.sell
	crtk__pc_array_parse(api_config->paths.sell, paths_sell, exchange, coin);

	json_decref(root);
	return lib_error;
}

static crtk_error crtk__url_get(const char *json, const char *const url) {
	crtk_error lib_error = { .error = CRTK_ERROR_NONE };

	CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
	if(result != CURLE_OK) {
		crtk__error_set(&lib_error, CRTK_ERROR_LIBCURL, curl_easy_strerror(result));
		return lib_error;
	}

	CURL *handle = curl_easy_init();
	if(!handle) {
		crtk__error_set(&lib_error, CRTK_ERROR_LIBCURL, "failed libcurl init");
		return lib_error;
	}

	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, crtk__data_write);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, json);

	result = curl_easy_perform(handle);

	if(result != CURLE_OK) {
		crtk__error_set(&lib_error, CRTK_ERROR_LIBCURL, curl_easy_strerror(result));
		return lib_error;
	}

	curl_easy_cleanup(handle);
	return lib_error;
}

static crtk_error crtk__api_parse(crtk_market *const market, const char *const json, const struct crtk__api_config *const api_config) {
	crtk_error lib_error = { .error = CRTK_ERROR_NONE };
	json_error_t json_error;

	json_t *root = json_loads(json, 0, &json_error);
	if(!root) {
		crtk__error_set(&lib_error, CRTK_ERROR_API, json_error.text);
		return lib_error;
	}

	json_t *status;
	if(api_config->paths.status[0][0]) crtk__object_get(&status, root, api_config->paths.status);

	// first check for error

	bool api_error = false;
	if(api_config->paths.status[0][0]) {
		if(!status) {
			crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
			return lib_error;
		}

		switch(api_config->status_success.type) {
			case CRTK__STATUS_TYPE_STRING:
				if(!json_is_string(status)) {
					crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
					return lib_error;
				}

				if(strcmp(json_string_value(status), api_config->status_success.value.string)) api_error = true;
				break;

			case CRTK__STATUS_TYPE_BOOLEAN:
				if(!json_is_boolean(status)) {
					crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
					return lib_error;
				}

				if(json_is_true(status) != api_config->status_success.value.boolean) api_error = true;
				break;

			case CRTK__STATUS_TYPE_INTEGER:
				if(!json_is_integer(status)) {
					crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
					return lib_error;
				}

				if((int16_t) json_integer_value(status) != api_config->status_success.value.integer) api_error = true;
				break;
		}
	}

	if(api_error) {
		if(api_config->paths.status_desc[0][0]) {
			json_t *status_desc;
			lib_error = crtk__object_get(&status_desc, root, api_config->paths.status_desc);
			if(lib_error.error == CRTK_ERROR_RESPONSE_CONFIG_CONFLICT) return lib_error;

			crtk__error_set(&lib_error, CRTK_ERROR_API, json_string_value(status_desc));
			return lib_error;
		} else {
			crtk__error_set(&lib_error, CRTK_ERROR_API, CRTK__ERROR_API_DESC);
			return lib_error;
		}
	}

	// no error

	json_t *buy;
	crtk__object_get(&buy, root, api_config->paths.buy);

	json_t *sell;
	crtk__object_get(&sell, root, api_config->paths.sell);

	if (
		!(buy && sell)
		|| (api_config->paths.status[0][0] && !status)
	) {
		crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
		return lib_error;
	}

	switch(api_config->number_format) {
		case CRTK__NUMBER_FORMAT_STRING:
			if(!(json_is_string(buy) && json_is_string(sell))) {
				crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
				return lib_error;
			}

			// store values

			market->buy = strtold(json_string_value(buy), NULL);
			market->sell = strtold(json_string_value(sell), NULL);
			market->buy_int = crtk_market_float_to_int(market->buy);
			market->sell_int = crtk_market_float_to_int(market->sell);
			break;

		case CRTK__NUMBER_FORMAT_REAL:
			if(!(json_is_real(buy) && json_is_real(sell))) {
				crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
				return lib_error;
			}

			market->buy = json_real_value(buy);
			market->sell = json_real_value(sell);
			market->buy_int = crtk_market_float_to_int(market->buy);
			market->sell_int = crtk_market_float_to_int(market->sell);
			break;
	}

	json_decref(root);
	return lib_error;
}

static crtk_error crtk__pc_array_parse(char (*dest)[CRTK__PC_ARRAY_SIZE], const json_t *const pc_array, const char *const exchange, const char *const coin) {
	crtk_error lib_error = { .error = CRTK_ERROR_NONE };

	json_t *el;
	uint_fast8_t i = 0;
	for(uint_fast8_t len = json_array_size(pc_array); i < len; ++i) {
		el = json_array_get(pc_array, i);
		if(json_is_array(el)) {
			crtk__format_array_parse(dest[i], el, exchange, coin);
		} else if(json_is_string(el)) {
			strcpy(dest[i], json_string_value(el));
		} else {
			crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": invalid array element type");
			return lib_error;
		}
	}

	dest[i][0] = 0;

	return lib_error;
}

static size_t crtk__data_write(const char *const buffer, const size_t size, const size_t length, void *const dest) {
	strncpy(dest, buffer, (size * length));
	return (size * length);
}

static crtk_error crtk__format_array_parse(char *const dest, const json_t *const format_array, const char *const exchange, const char *const coin) {
	crtk_error lib_error;

	dest[0] = 0;

	json_t *el;
	char el_string[128];
	for(uint_fast8_t i = 0, len = json_array_size(format_array); i < len; ++i) {
		el = json_array_get(format_array, i);
		if(json_is_string(el)) {
			strcpy(el_string, json_string_value(el));
		} else {
			crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": invalid type for array element");
		}

		if(!el_string[0]) {
			crtk__error_set(&lib_error, CRTK_ERROR_API_CONFIG_INVALID, CRTK__ERROR_API_CONFIG_INVALID_DESC ": invalid string for array element");
			return lib_error;
		}

		crtk__format_replace(el_string, exchange, coin);
		strcat(dest, el_string);
	}

	return lib_error;
}

static void crtk__format_replace(char *const format, const char *const exchange, const char *const coin) {
	if(!strcmp(format, CRTK__FORMAT_SPECIFIER_EXCHANGE)) {
		strcpy(format, exchange);
	} else if(!strcmp(format, CRTK__FORMAT_SPECIFIER_COIN)) {
		strcpy(format, coin);
	}
}

static crtk_error crtk__object_get(json_t **dest, const json_t *const root, const char (*const pc_array)[CRTK__PC_ARRAY_SIZE]) {
	crtk_error lib_error = { .error = CRTK_ERROR_NONE };

	json_t *child = json_object_get(root, pc_array[0]);
	for(uint_fast8_t i = 1; pc_array[i][0]; ++i) {
		child = json_object_get(child, pc_array[i]);
		if(!child) {
			*dest = NULL;
			crtk__error_set(&lib_error, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT, CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC);
			return lib_error;
		}
	}

	*dest = child;
	return lib_error;
}

long double crtk_market_int_to_float(uint64_t value) {
	return (value / CRTK__UNIT_FACTOR);
}

uint64_t crtk_market_float_to_int(long double value) {
	return (value * CRTK__UNIT_FACTOR);
}
