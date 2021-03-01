CC = g++
SRCDIR = src
BINTRAIN = bin/train
BINTEST = bin/test

CFLAGS += `pkg-config --cflags --libs opencv4`

all:
	$(CC) $(SRCDIR)/train.cpp $(CFLAGS) -o $(BINTRAIN)
	$(CC) $(SRCDIR)/test.cpp $(CFLAGS) -o $(BINTEST)

train:
	$(CC) $(SRCDIR)/train.cpp $(CFLAGS) -o $(BINTRAIN)

test:
	$(CC) $(SRCDIR)/test.cpp $(CFLAGS) -o $(BINTEST)

clean:
	rm $(BINTEST) $(BINTRAIN)