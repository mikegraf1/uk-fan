
ALL=cats_and_mouse
FLAGS=-std=c++14 -fPIC -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -O3
#LIBS=-ljsoncpp -lpthread -lcurl
all: ${ALL}
#.PHONEY: man

cats_and_mouse: cats_and_mouse.cc
	g++ ${FLAGS} -o cats_and_mouse cats_and_mouse.cc
        # tests -- TODO change to google test framework  
	./cats_and_mouse
	echo ---

cats_and_mouse.cc:

clean:
	- rm *.o

realclean: clean
	- rm ${ALL}

