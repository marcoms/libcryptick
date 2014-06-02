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

#ifndef CRYPTICK_CURRENCIES_H
#define CRYPTICK_CURRENCIES_H

static const btc_currcy_t btc_currencies[] = {
	#ifdef MT_GOX_API
	// australia

	{
		.name = "AUD",
		.sign = u8"$",
		.sf = (1e5)
	},

	// canada

	{
		.name = "CAD",
		.sign = u8"$",
		.sf = (1e5)
	},

	// switzerland

	{
		.name = "CHF",
		.sign = u8"Fr.",
		.sf = (1e5)
	},

	// china

	{
		.name = "CNY",
		.sign = u8"\u00a5",
		.sf = (1e5)
	},

	// czech republic

	{
		.name = "CZK",
		.sign = u8"K\u010d.",
		.sf = (1e5)
	},

	// denmark

	{
		.name = "DKK",
		.sign = u8"kr.",
		.sf = (1e5)
	},
	#endif

	// eurozone

	{
		.name = "EUR",
		.sign = u8"\u20ac",
		.sf = (1e5)
	},

	#ifdef MT_GOX_API
	// great britain

	{
		.name = "GBP",
		.sign = u8"\u00a3",
		.sf = (1e5)
	},

	// hong kong

	{
		.name = "HKD",
		.sign = u8"$",
		.sf = (1e5)
	},

	// japan

	{
		.name = "JPY",
		.sign = u8"\u00a5",
		.sf = (1e3)
	},

	// norway

	{
		.name = "NOK",
		.sign = u8"kr.",
		.sf = (1e5)
	},

	// poland

	{
		.name = "PLN",
		.sign = u8"z\u0142.",
		.sf = (1e5)
	},
	#endif

	// russia

	{
		#ifdef MT_GOX_API
		.name = "RUB",
		#elif defined(BTC_E_API)
		.name = "RUR",
		#endif
		.sign = u8"p.",
		.sf = (1e5)
	},

	#ifdef MT_GOX_API
	// sweden

	{
		.name = "SEK",
		.sign = u8"kr.",
		.sf = (1e3)
	},

	// singapore

	{
		.name = "SGD",
		.sign = u8"$",
		.sf = (1e5)
	},

	// thailand

	{
		.name = "THB",
		.sign = u8"\u0e3f",
		.sf = (1e5)
	},
	#endif

	// united states

	{
		.name = "USD",
		.sign = u8"$",
		.sf = (1e5)
	},
};

#endif
