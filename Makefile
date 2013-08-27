ga: ga.o
	g++ ga.o -o ga

ga.o: ga.cpp
	g++ -c ga.cpp

clean:
	 rm -rf *o ga
