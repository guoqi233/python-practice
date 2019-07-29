run: handler.o reactor.o subReactor.o handler.o main.cpp
	g++  main.cpp handler.o reactor.o subReactor.o -lpthread -g

handler.o: handler.cpp handler.h
	g++ -c handler.cpp handler.h -g

reactor.o: reactor.cpp reactor.h
	g++ -c reactor.cpp reactor.h -g

subreactor.o:subReactor.cpp subReactor.h
	g++ -c subReactor.cpp subReactor.h -g

clean:
	rm -rf *.o
	rm -rf *.gch
	rm -rf *.out
