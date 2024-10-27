#include "launcher.h"
#include <bits/stdc++.h>
#include <filesystem>
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cout << "Usage: tester program.bin interactor.bin "
             << "path/of/tests/directory";
        return 0;
    }
    string progfile = argv[1];
    string invokerfile = argv[2];
    string dir = argv[3];
    long long testcount;
    vector<long long> times, moves;
    for (auto& entry : filesystem::directory_iterator(dir))
    {
        testcount++;
        long long &time = times.emplace_back(),
                  &movecount = moves.emplace_back();
        Process invoker(invokerfile, {
    }
}
