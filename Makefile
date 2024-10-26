AStar.bin: AStar.cpp
	clang++ -std=c++11 AStar.cpp -glldb -o AStar.bin

Back.bin: Back.cpp
	clang++ -std=c++11 Back.cpp -glldb -o Back.bin

clean:
	rm $(wildcard *.bin)

