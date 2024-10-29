#include "interactorMap.h"
#include "launcher.h"
#include <bits/stdc++.h>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int main(int argc, char** argv)
{
    if (argc <= 3)
    {
        cout << "Usage: interactor [-io] [-stats] program.bin testfile.txt "
                "variant"
             << endl;
        return 0;
    }
    bool showio = false;
    bool stats = false;
    int argsstart = 1;
    for (; argsstart < argc && argv[argsstart][0] == '-'; argsstart++)
    {
        if (strcmp(argv[argsstart], "-io") == 0)
            showio = true;
        else if (strcmp(argv[argsstart], "-stats") == 0)
            stats = true;
    }
    Process child(argv[argsstart], {}, {});
    FILE *childw = child.StdIN(), *childr = child.StdOUT();
    ifstream fs(argv[argsstart + 1]);
    Map mp(fs);
    fs.close();
    int radius = atoi(argv[argsstart + 2]);
    fprintf(childw, "%d\n", radius);
    if (showio)
        cout << radius << endl;
    {
        auto km = mp.KeymakerCoords();
        fprintf(childw, "%d %d\n", km.first, km.second);
        if (showio)
            cout << km.first << ' ' << km.second << endl;
    }
    fflush(childw);
    bool ok = true;
    int prevx = 0, prevy = 0;
    int nmoves = 0;
    chrono::steady_clock* clock = nullptr;
    decltype(clock->now()) start;
    if (stats)
    {
        clock = new chrono::steady_clock();
        start = clock->now();
    }
    while (true)
    {
        char cmd;
        do
        {
            fscanf(childr, "%c", &cmd);
        } while (isspace(cmd));
        if (cmd == 'm')
        {
            nmoves++;
            int x, y;
            fscanf(childr, "%d%d", &x, &y);
            if (showio)
                cout << ">>> m " << x << ' ' << y << endl;
            if (abs(x - prevx) + abs(y - prevy) > 1)
            {
                cout << "Tried to move from (" << prevx << ", " << prevy
                     << ") to (" << x << ", " << y << ")" << endl;
                ok = false;
                break;
            }
            if (!mp.CellIsSafe(x, y))
            {
                cout << "Cell (" << x << ", " << y << ") is unsafe!" << endl;
                ok = false;
                break;
            }
            prevx = x;
            prevy = y;
            auto vis = mp.Vision(x, y, radius);
            fprintf(childw, "%d\n", (int)vis.size());
            if (showio)
                cout << vis.size() << endl;
            for (auto& p : vis)
            {
                fprintf(childw, "%d %d %c\n", p.first.first, p.first.second,
                        p.second);
                if (showio)
                {
                    printf("%d %d %c\n", p.first.first, p.first.second,
                           p.second);
                    fflush(stdout);
                }
            }
            fflush(childw);
        }
        else if (cmd == 'e')
        {
            int dist;
            fscanf(childr, "%d", &dist);
            if (showio)
                cout << ">>> e " << dist << endl;
            int expect = mp.Solution();
            if (dist != expect)
            {
                cout << "Wrong answer: expected " << expect << ", got " << dist
                     << endl;
                ok = false;
            }
            break;
        }
        else
        {
            cout << "Wrong response: " << cmd << endl;
            ok = false;
            break;
        }
    }
    if (!ok)
        child.Kill();
    else
        child.Wait();
    if (stats)
    {
        cout << "Stats:\n";
        auto end = clock->now();
        delete clock;
        chrono::duration dur = end - start;
        using rat = decltype(dur)::period;
        long long us = (long long)dur.count() * rat::num * (int)1e6 / rat::den;
        cout << "time_us:" << us << '\n';
        cout << "moves:" << nmoves << endl;
    }
    return !ok;
}
