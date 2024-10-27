all: AStar.bin Back.bin interactor.bin testgen.bin

AStar.bin: AStar.cpp
	clang++ -std=c++11 AStar.cpp -o AStar.bin

Back.bin: Back.cpp
	clang++ -std=c++11 Back.cpp -o Back.bin

interactor.bin: interactor.cpp launcher.cpp launcher.h interactorMap.h interactorMap.cpp
	clang++ -std=c++20 interactor.cpp launcher.cpp interactorMap.cpp -o interactor.bin

testgen.bin: testgen.cpp interactorMap.h interactorMap.cpp
	clang++ -std=c++20 testgen.cpp interactorMap.cpp -o testgen.bin

clean:
	rm $(wildcard *.bin)

PHONY: clean
