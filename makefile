output: main.cpp 
	g++ -o header header.hpp
	g++ -o output main.cpp

main.o: main.cpp header.hpp
	g++ -c main.cpp

functions.o: functions.cpp header.hpp
	g++ -c functions.cpp

header.o: header.hpp
	g++ -c header.hpp