
ALL=mars_rover_photo
FLAGS=-std=c++14 -fPIC -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -O3
LIBS=-ljsoncpp -lpthread -lcurl
all: ${ALL}
.PHONEY: man

mars_rover_photo: mars_rover_photo.cc
	g++ ${FLAGS} -o mars_rover_photo mars_rover_photo.cc ${LIBS}
	# tests -- TODO change to google test framework  
	./mars_rover_photo
	echo ---
	./mars_rover_photo -f MarsDates.txt
	echo ---
	./mars_rover_photo -d 3-Jun-15
	./mars_rover_photo -d 15-Dec-17
	./mars_rover_photo -d 1-Mar-17
	./mars_rover_photo -d 22-Feb-17
	./mars_rover_photo -d 3-Jul-17
	./mars_rover_photo -d 5-May-17
	echo ---

man:
	man man1/mars_rover_photo

clean:
	- rm *.o

realclean: clean
	- rm ${ALL}
	- rm *.JPG
