output: main.cpp 
	g++ -o main main.cpp
	g++ -o header header.hpp
	g++ -o functions functions.cpp
	g++ -o output main.cpp

main.o: main.cpp header.hpp
	g++ -c main.cpp

run:
	./output $(tp) $(rt) $(wt)

functions.o: functions.cpp header.hpp
	g++ -c functions.cpp

header.o: header.hpp
	g++ -c header.hpp
	
clean:
	rm *.o output