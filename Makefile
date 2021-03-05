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
PAMSRC = $(wildcard $(SRCDIR)/pam/*.cpp)
SRC = $(wildcard $(SRCDIR)/*.cpp)

faceauth: $(SRC)
	mkdir -p $(BDIR)
	$(CXX) -g -shared -fPIC -o $(BDIR)/libfaceauth.so $^ $(CFLAGS)

install:
	install -d $(PREFIX)/lib/
	install -m 644 $(BDIR)/libfaceauth.so $(PREFIX)/lib/
	install -d $(PREFIX)/include/faceauth
	install -m 644 $(DEPS) $(PREFIX)/include/faceauth
	echo "/usr/local/lib" > /etc/ld.so.conf.d/local.conf
	install -d $(DDATA)
	install -m 644 $(SDATA)/deploy.prototxt $(DDATA)
	install -m 644 $(SDATA)/res10_300x300_ssd_iter_140000_fp16.caffemodel $(DDATA)
	install -m 644 $(SDATA)/haarcascade_frontalface_alt.xml $(DDATA)
	ldconfig

pam_faceauth: $(PAMSRC)
	mkdir -p $(BDIR)
	$(CXX) -g -fPIC -shared -o $(BDIR)/pam_faceauth.so $^ -lfaceauth -lpam -lpam_misc $(CFLAGS)

pam_faceauth_install:
	install -m 755 $(BDIR)/pam_faceauth.so /lib/security