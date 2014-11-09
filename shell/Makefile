CACHESHELL_OBJ := shell/cacheshell.o

cacheshell/%.o: cacheshell/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

shell/cacheshell: $(CACHESHELL_OBJ) libclient/libclient.a
	$(LD) -o $@ $(LDFLAGS) $(CACHESHELL_OBJ) $(LIBS)

MASTERSHELL_OBJ := shell/cacheshell.o

mastershell/%.o: mastershell/%.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

shell/mastershell: $(MASTERSHELL_OBJ) libclient/libclient.a
	$(LD) -o $@ $(LDFLAGS) $(MASTERSHELL_OBJ) $(LIBS)

