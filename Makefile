CC=g++ -Wall

PROGS=main 

all: $(PROGS)

clean:
	rm -f $(PROGS)

main: main.cpp ext4_objs.hpp
	$(CC) main.cpp ext4_objs.hpp -o main

