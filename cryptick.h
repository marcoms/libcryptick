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

// PUBLIC DECLERATIONS ET AL FOR INTERNAL AND EXTERNAL USE

#ifndef CRYPTICK_H
#define CRYPTICK_H

#include <stdbool.h>
#include <stdint.h>
#include <jansson.h>

/*
Range of errors that can be reported in a crtk_error
*/
enum crtk_error_value {
	CRTK_ERROR_NONE = 0
	, CRTK_ERROR_API_CONFIG_INVALID
	, CRTK_ERROR_RESPONSE_CONFIG_CONFLICT
	, CRTK_ERROR_LIBCURL
	, CRTK_ERROR_API
};

/*
nonzero error denotes an error, and desc contains a string describing it
*/
typedef struct {
	enum crtk_error_value error;
	char desc[64];
} crtk_error;

/*
Contains buy and sell values for a market as double-precision floats
*/
typedef struct {
	long double buy;
	long double sell;
	uint64_t buy_int;
	uint64_t sell_int;
} crtk_market;

// MAIN FUNCTIONS

/*
Retrieves the market for exchange/coin, using api, if a config file exists
*/
crtk_error crtk_market_get(
	crtk_market *const market
	, const char *const api
	, const char *const exchange
	, const char *const coin
);

/*
Returns the true floating-point representation of value
*/
long double crtk_market_int_to_float(
	uint64_t value
);

/*
Returns an integer representation of value, usually for calculation purposes
*/
uint64_t crtk_market_float_to_int(
	long double value
);

#endif
