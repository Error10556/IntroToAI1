all: AStar.bin Back.bin interactor.bin testgen.bin tester.bin shortestPathLength.bin testVisualizer.bin

AStar.bin: AStar.cpp
	clang++ -std=c++11 AStar.cpp -o AStar.bin

Back.bin: Back.cpp
	clang++ -std=c++11 Back.cpp -o Back.bin

interactor.bin: interactor.cpp launcher.cpp launcher.h interactorMap.h interactorMap.cpp
	clang++ -std=c++20 interactor.cpp launcher.cpp interactorMap.cpp -o interactor.bin

testgen.bin: testgen.cpp interactorMap.h interactorMap.cpp
	clang++ -std=c++20 testgen.cpp interactorMap.cpp -o testgen.bin

tester.bin: tester.cpp launcher.h launcher.cpp
	clang++ -std=c++20 tester.cpp launcher.cpp -o tester.bin

shortestPathLength.bin: shortestPathLength.cpp interactorMap.h interactorMap.cpp
	clang++ -std=c++20 shortestPathLength.cpp interactorMap.cpp -o shortestPathLength.bin

testVisualizer.bin: testVisualizer.cpp interactorMap.h interactorMap.cpp
	clang++ -std=c++20 testVisualizer.cpp interactorMap.cpp -o testVisualizer.bin

clean:
	rm $(wildcard *.bin)

PHONY: clean
