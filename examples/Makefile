CC = g++
BINTRAIN = train
BINTEST = test

CFLAGS += `pkg-config --cflags --libs opencv4` -lfaceauth -lpam -lpam_misc -std=c++17

all:
	$(CC) faceauth_train.cpp $(CFLAGS) -o $(BINTRAIN)
	$(CC) faceauth_test.cpp $(CFLAGS) -o $(BINTEST)

train:
	$(CC) faceauth_train.cpp $(CFLAGS) -o $(BINTRAIN)

test:
	$(CC) faceauth_test.cpp $(CFLAGS) -o $(BINTEST)

clean:
	rm $(BINTEST) $(BINTRAIN)