#include "launcher.h"
#include <algorithm>
#include <bits/stdc++.h>
#include <cstdio>
#include <filesystem>
#include <sstream>
using namespace std;

void PrintStats(vector<long long> vals, ostream& stream)
{
    int n = vals.size();
    double mean = 0;
    for (auto i : vals)
        mean += i;
    mean /= n;
    stream << "Mean: ";
    stream.setf(ios::fixed);
    stream.precision(8);
    stream << mean << '\n';
    sort(vals.begin(), vals.end());
    int mode = -1, run = 0, maxrun = -1;
    long long prev = vals[0];
    for (auto i : vals)
    {
        if (prev == i)
            run++;
        else
        {
            if (maxrun < run)
            {
                maxrun = run;
                mode = i;
            }
            prev = i;
            run = 1;
        }
    }
    if (maxrun < run)
    {
        maxrun = run;
        mode = prev;
    }
    stream << "Mode: " << mode << " (" << maxrun << " occurrences)\n";
    stream << "Median: ";
    if (n & 1)
        stream << vals[vals.size() / 2];
    else
    {
        long long sm = vals[vals.size() / 2 - 1] + vals[vals.size() / 2];
        if (sm & 1)
        {
            stream.precision(1);
            stream << sm / 2.0;
            stream.precision(8);
        }
        else
            stream << sm / 2;
    }
    stream << '\n';
    double sd = 0;
    for (auto i : vals)
        sd += (i - mean) * (i - mean);
    sd = sqrt(sd / n);
    cout << "Standard deviation: " << sd << endl;
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cout << "Usage: tester program.bin interactor.bin "
             << "path/of/tests/directory" << endl;
        return 0;
    }
    string progfile = argv[1];
    string invokerfile = argv[2];
    string dir = argv[3];
    long long testcount = 0;
    vector<long long> times, moves;
    for (auto& entry : filesystem::directory_iterator(dir))
    {
        testcount++;
        long long &time = times.emplace_back(),
                  &movecount = moves.emplace_back();
        Process invoker(invokerfile,
                        {"invoker", "-stats", progfile, entry.path()}, {});
        stringstream stats;
        FILE* outp = invoker.StdOUT();
        char buf[128];
        while (fgets(buf, 128, outp))
            stats << buf;
        int result = invoker.Wait();
        if (result != 0)
        {
            cout << "Failure on " << entry.path() << "! Aborting." << endl;
            return 0;
        }
        string sstats = stats.str();
        int cur = sstats.find(':');
        cur = sstats.find(':', cur + 1);
        stats.seekg(cur + 1);
        stats >> time;
        cur = sstats.find(':', cur + 1);
        stats.seekg(cur + 1);
        stats >> movecount;
        if (testcount % 100 == 0)
            cout << "Tested on " << testcount << " examples." << endl;
    }
    cout << "Ran " << testcount << " tests.\n\n--Final Statistics--\n";
    cout << "Execution time, in microseconds:\n";
    PrintStats(times, cout);
    cout << "Number of moves:\n";
    PrintStats(moves, cout);
}
