
MASTERSERVER_SRC := masterserver/masterserver.cc \
			  masterserver/masterserverimpl.cc \
			  masterserver/masterserverdb.cc \

MASTERSERVER_OBJ := $(MASTERSERVER_SRC:.c=.o)
MASTERSERVER_OBJ := $(MASTERSERVER_OBJ:.cc=.o)
$(MASTERSERVER_OBJ): include/masterserver.hh

masterserver/%.o: masterserver/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

masterserver/%.o: masterserver/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# Generate initial files to be edited
scaffold: include/masterserver.x xdrpp
	$(XDRC) -masterserverhh -o masterserver/masterserverimpl.hh $<
	$(XDRC) -masterservercc -o masterserver/masterserverimpl.cc $<

masterserver/masterserver: $(MASTERSERVER_OBJ)
	$(LD) -o $@ $(LDFLAGS) $(MASTERSERVER_OBJ) $(LIBS)
