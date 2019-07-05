pthpool: pth_pool.cpp
	g++ -o pthpool pth_pool.cpp -lpthread

pth: pth.o
	gcc -o pth pth.o -lpthread
pth.o: pth.c
	gcc -c pth.c 
clean:
	rm -rf pth
	rm -rf pth.o
	rm -rf pthpool


