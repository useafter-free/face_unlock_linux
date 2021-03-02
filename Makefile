CXX = g++
SRCDIR = src
BDIR=build/debug

CFLAGS += `pkg-config --cflags --libs opencv4` -std=c++17 -g

#PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

DEPS = $(wildcard $(SRCDIR)/*.hpp)

SRC = $(wildcard $(SRCDIR)/*.cpp)

faceauth: $(SRC)
	mkdir -p $(BDIR)
	$(CXX) -g -c $^ $(CFLAGS)
	ar rcs $(BDIR)/libfaceauth.a *.o
	ar -t $(BDIR)/libfaceauth.a
	rm *.o
	$(CXX) -g -fPIC -c $^ $(CFLAGS)
	$(CXX) -shared -o $(BDIR)/libfaceauth.so *.o
	rm *.o

install: $(BDIR)/libfaceauth.a $(BDIR)/libfaceauth.so
	install -d $(PREFIX)/lib/
	install -m 644 $(BDIR)/libfaceauth.so $(PREFIX)/lib/
	install -m 644 $(BDIR)/libfaceauth.a $(PREFIX)/lib/
	install -d $(PREFIX)/include/faceauth
	install -m 644 $(DEPS) $(PREFIX)/include/faceauth
	echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf
	ldconfig
