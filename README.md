## The Matrix Universe

### How to use

First, execute `make all`. Then, use any of the following binaries (**to see the available command-line arguments of .bin files, execute them without parameters**):

`AStar.bin`, `Back.bin`: the solutions of the Codeforces problems. Execute and interact with the Actor.  
`interactor.bin`: automatically interacts with a Codeforces solution.  
`shortestPathLength.bin`: use Breadth-first search to find the solution to a test.  
`tester.bin`: runs `interactor.bin` on all tests in a directory. Outputs the statistics.  
`testgen.bin`: generates randomized valid test maps.  
`testVisualizer.bin`: generates LaTeX code from a map file.  
`searchUnsolvable.sh "tests/*"`: find all test cases that fit the mask and are *unsolvable*, i.e. have the answer `-1`, according to `shortestPathLength.bin`.  
`visualizeUnsolvable.sh`: find and visualize all *unsolvable* test cases **in the tests/** directory.
