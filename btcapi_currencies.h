#ifndef BTCAPI_CURRENCIES_H
#define BTCAPI_CURRENCIES_H

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
