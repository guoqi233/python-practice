run: TaskPool.o run.o
	g++ -o run run.o TaskPool.o -lpthread
run.o: run.cpp
	g++ -c run.cpp
TaskPool.o:TaskPool.cpp TaskPool.h
	g++ -c TaskPool.cpp TaskPool.h

clean:
	rm -rf run, TaskPool.o, run.o
