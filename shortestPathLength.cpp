#include "interactorMap.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Usage: shortestPathLength testfile.txt" << endl;
        return 0;
    }
    fstream fs(argv[1], ios::in);
    Map mp(fs);
    fs.close();
    cout << mp.Solution() << endl;
    return 0;
}
