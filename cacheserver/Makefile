
CSERVER_SRC := cacheserver/cacheserver.cc \
			  cacheserver/cacheserverimpl.cc \

CSERVER_OBJ := $(CSERVER_SRC:.c=.o)
CSERVER_OBJ := $(CSERVER_OBJ:.cc=.o)
$(SERVER_OBJ): include/server.hh

cacheserver/%.o: cacheserver/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cacheserver/%.o: cacheserver/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

cacheserver/cacheserver: $(CSERVER_OBJ)
	$(LD) -o $@ $(LDFLAGS) $(CSERVER_OBJ) $(LIBS)
