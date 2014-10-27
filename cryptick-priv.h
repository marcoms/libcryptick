/*
	Copyright (C) 2014 Marco Scannadinari <m@scannadinari.co.uk>

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

// PRIVATE DECLERATIONS ET AL FOR INTERNAL USE

#ifndef CRYPTICK_PRIV_H
#define CRYPTICK_PRIV_H

#include <stdbool.h>
#include <stdint.h>

#define CRTK__PC_ARRAY_SIZE 16
#define CRTK__PC_ARRAY_N_ELEMENTS 8

#define CRTK__ERROR_API_CONFIG_INVALID_DESC "invalid API configuration file"
#define CRTK__ERROR_RESPONSE_CONFIG_CONFLICT_DESC "API configuration does not match response"
#define CRTK__ERROR_API_DESC "API call returned unsuccessful"

/*
What type a successful status response is
*/
enum crtk__status_type {
	CRTK__STATUS_TYPE_STRING
	, CRTK__STATUS_TYPE_BOOLEAN
	, CRTK__STATUS_TYPE_INTEGER
};

/*
Different values for different types in crtk__status_type
*/
union crtk__status_value {
	char string[16];
	bool boolean;
	int16_t integer;
};

/*
How numbers are represented in the API
*/
enum crtk__number_format {
	CRTK__NUMBER_FORMAT_STRING
	, CRTK__NUMBER_FORMAT_REAL
};

/*
Paths to common sets of data in the API's response
*/
struct crtk__paths {
	char status[CRTK__PC_ARRAY_N_ELEMENTS][CRTK__PC_ARRAY_SIZE];
	char status_desc[CRTK__PC_ARRAY_N_ELEMENTS][CRTK__PC_ARRAY_SIZE];
	char buy[CRTK__PC_ARRAY_N_ELEMENTS][CRTK__PC_ARRAY_SIZE];
	char sell[CRTK__PC_ARRAY_N_ELEMENTS][CRTK__PC_ARRAY_SIZE];
};

/*
A successful result from the API's status object
*/
struct crtk__status_success {
	enum crtk__status_type type;
	union crtk__status_value value;
};

/*
Abstract representation of the chosen API's workings
*/
struct crtk__api_config {
	char name[32];
	char url[128];
	struct crtk__status_success status_success;
	enum crtk__number_format number_format;
	struct crtk__paths paths;
};

#include "cryptick.h"

/*
Sets a crtk_error's error and description
*/
static void crtk__error_set(
	crtk_error *const var
	, const enum crtk_error_value value
	, const char *const desc
);

/*
Finds a corresponding API configuration file and parses it into api_config
*/
static crtk_error crtk__api_config_parse(
	struct crtk__api_config *const api_config
	, const char *const api
	, const char *const exchange
	, const char *const coin
);

/*
Requests and hopefully retrieves the JSON from url
*/
static crtk_error crtk__url_get(
	const char *body
	, const char *const url
);

/*
Parses API's JSON with api_config as reference, then stores results into market
*/
static crtk_error crtk__api_parse(
	crtk_market *const market
	, const char *const json
	, const struct crtk__api_config *const api_config
);

// UTILITY FUNCTIONS

/*
cURL callback that copies buffer to destination
*/
static size_t crtk__data_write(
	const char *const buffer
	, const size_t size
	, const size_t nmemb
	, void *const dest
);

/*
Replaces all occurances of "{c}" and "{e}" with exchange and coin, respectively,
and copies the concatenated result to destination
*/
static crtk_error crtk__format_array_parse(
	char *const dest
	, const json_t *const format_array
	, const char *const exchange
	, const char *const coin
);

/*
Replaces (in place) a string containing "{c}" or "{c}" with exchange or coin,
respectively
*/
static void crtk__format_replace(
	char *const format
	, const char *const exchange
	, const char *const coin
);

/*
Searches for and processes format arrays (if any) in pc_array and stores the
result in destination
*/
static crtk_error crtk__pc_array_parse(
	char (*dest)[CRTK__PC_ARRAY_SIZE]
	, const json_t *const pc_array
	, const char *const exchange
	, const char *const coin
);

/*
Retrieves the JSON object from root, with the heirarchy described in pc_array as
reference
*/
static crtk_error crtk__object_get(
	json_t **dest
	, const json_t *const root
	, const char (*const pc_array)[CRTK__PC_ARRAY_SIZE]
);

#endif
