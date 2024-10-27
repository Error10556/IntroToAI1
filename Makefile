all: AStar.bin Back.bin interactor.bin

AStar.bin: AStar.cpp
	clang++ -std=c++11 AStar.cpp -o AStar.bin

Back.bin: Back.cpp
	clang++ -std=c++11 Back.cpp -o Back.bin

interactor.bin: interactor.cpp launcher.cpp launcher.h interactorMap.h interactorMap.cpp
	clang++ -std=c++20 interactor.cpp launcher.cpp interactorMap.cpp -o interactor.bin

clean:
	rm $(wildcard *.bin)

interact: interactor.bin
	./interactor.bin $(read)

PHONY: clean
