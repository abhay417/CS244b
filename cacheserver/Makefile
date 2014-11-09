
CACHESERVER_SRC := cacheserver/cacheserver.cc \
			  cacheserver/cacheserverimpl.cc \
			  cacheserver/serverdb.cc \

CACHESERVER_OBJ := $(CACHESERVER_SRC:.c=.o)
CACHESERVER_OBJ := $(CACHESERVER_OBJ:.cc=.o)
$(CACHESERVER_OBJ): include/cacheserver.hh

cacheserver/%.o: cacheserver/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cacheserver/%.o: cacheserver/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# Generate initial files to be edited
scaffold: include/cacheserver.x xdrpp
	$(XDRC) -cacheserverhh -o cacheserver/cacheserverimpl.hh $<
	$(XDRC) -cacheservercc -o cacheserver/cacheserverimpl.cc $<

cacheserver/cacheserver: $(CACHESERVER_OBJ)
	$(LD) -o $@ $(LDFLAGS) $(CACHESERVER_OBJ) $(LIBS)
