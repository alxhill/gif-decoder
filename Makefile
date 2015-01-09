CC=clang
CFLAGS=-Wall -std=c++11 -stdlib=libc++ -x c++ -c -DDEBUG

all: gif

gif: main.o gif.o gif_frame.o
	$(CC) -lc++ gif_frame.o gif.o main.o -o gif

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

gif.o: gif.hpp gif.cpp
	$(CC) $(CFLAGS) gif.cpp

gif_frame.o: gif_frame.hpp gif_frame.cpp
	$(CC) $(CFLAGS) gif_frame.cpp

clean:
	rm -rf *.o gif
