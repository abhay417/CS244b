
-include Makefile.local

PKGCONFIG = PKG_CONFIG_PATH="xdrpp:$${PKG_CONFIG_PATH}" pkg-config

# Seems to fix issues with the latest Xcode
LD := $(CXX)

# Don't use :=, or will break first time
XDRC = $(shell $(PKGCONFIG) --variable=xdrc xdrpp)

CPPFLAGS := `$(PKGCONFIG) --cflags xdrpp` -I.
LIBDIRS := -Llibclient
LIBS := -lclient `$(PKGCONFIG) --libs xdrpp` -ldl

CFLAGS := -ggdb3 -O0 -pthread
CXXFLAGS := -ggdb3 -O0 -pthread -std=c++11
LDFLAGS := -g -pthread $(LIBDIRS)

default: all

include cacheserver/Makefile
include libclient/Makefile
include shell/Makefile

.PHONY: all clean xdrpp

all: xdrpp include/cacheserver.hh libclient/libclient.a cacheserver/cacheserver shell/shell 

include/cacheserver.hh: include/cacheserver.x
	$(XDRC) -hh -o include/cacheserver.hh $<

xdrpp:
	+git submodule update --init
	cd xdrpp; test -f Makefile || ./configure CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"
	$(MAKE) -C xdrpp

clean:
	rm -f cacheserver/cacheserver
	rm -f cacheserver/*.o
	rm -f libclient/*.o
	rm -f libclient/libclient.a
	rm -f shell/*.o
	rm -f shell/shell
	! test -f xdrpp/Makefile || cd xdrpp && $(MAKE) clean

README.html: README.md
	-pandoc -o $@ README.md
