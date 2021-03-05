CXX = g++
SRCDIR = src
BDIR=build/debug
SDATA = data
DDATA = /etc/faceauth_data/

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
	install -d $(DDATA)
	install -m 644 $(SDATA)/deploy.prototxt $(DDATA)
	install -m 644 $(SDATA)/res10_300x300_ssd_iter_140000_fp16.caffemodel $(DDATA)
	install -m 644 $(SDATA)/haarcascade_frontalface_alt.xml $(DDATA)
	ldconfig
