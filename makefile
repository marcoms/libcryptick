CC ?= gcc

CRTK_SOURCES := cryptick.c cryptick-priv.h cryptick.h
CFLAGS += -std=c11 -fPIC `pkg-config libcurl --libs --cflags` `pkg-config jansson --libs --cflags`

cryptick: $(CRTK_SOURCES)
	$(CC) cryptick.c -c $(CFLAGS)

shared: cryptick.o
	$(CC) cryptick.o -shared

install-configs:
	install configs/*.json /etc/libcryptick/ -m 666
