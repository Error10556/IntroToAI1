#include "launcher.h"
#include <bits/stdc++.h>
#include <cctype>
#include <cstdio>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>
using namespace std;

enum class CellKind
{
    Empty = 0,
    Perceived = 1,
    Agent = 2,
    Key = 4,
    Sentinel = 8,
    Keymaker = 16,
};

char KindToChar(CellKind cell)
{
    switch (cell)
    {
    case CellKind::Empty:
        return '\0';
    case CellKind::Perceived:
        return 'P';
    case CellKind::Agent:
        return 'A';
    case CellKind::Sentinel:
        return 'S';
    case CellKind::Key:
        return 'B';
    case CellKind::Keymaker:
        return 'K';
    }
    return '\0';
}

inline bool CellIsSafe(CellKind cell)
{
    return (static_cast<int>(cell) & ~(static_cast<int>(CellKind::Keymaker) |
                                       static_cast<int>(CellKind::Key))) == 0;
}

class Map
{
public:
    static const int MaxX = 9, MaxY = 9;
    static bool ValidateCell(int x, int y)
    {
        return x >= 0 && x < MaxX && y >= 0 && y < MaxY;
    }

private:
    int radius;
    CellKind v[MaxX][MaxY];
    inline void AddCellKind(int x, int y, CellKind ck)
    {
        v[x][y] = (CellKind)((int)v[x][y] | (int)ck);
    }
    void ManhattanPerception(int x, int y, int d)
    {
        int startx = max(0, x - d);
        int endx = min(MaxX - 1, x + d);
        for (int i = startx; i <= endx; i++)
        {
            int yd = d - abs(x - i);
            int starty = max(0, y - yd);
            int endy = min(MaxY - 1, y + yd);
            for (int j = starty; j <= endy; j++)
                if (i != x || j != y)
                    AddCellKind(i, j, CellKind::Perceived);
        }
    }
    void ChebyshevPerception(int x, int y, int d)
    {
        int endx = min(MaxX - 1, x + d);
        int endy = min(MaxY - 1, y + d);
        for (int i = max(0, x - d); i <= endx; i++)
            for (int j = max(0, y - d); j <= endy; j++)
            {
                if (i != x || j != y)
                    AddCellKind(i, j, CellKind::Perceived);
            }
    }

public:
    Map(istream& in)
    {
        fill_n(&v[0][0], MaxX * MaxY, CellKind::Empty);
        in >> radius;
        for (int i = 0; i < MaxY; i++)
        {
            string s;
            in >> s;
            for (int j = 0; j < MaxX; j++)
                switch (s[j])
                {
                case 'A':
                    AddCellKind(j, i, CellKind::Agent);
                    ChebyshevPerception(j, i, 1);
                    break;
                case 'S':
                    AddCellKind(j, i, CellKind::Sentinel);
                    ManhattanPerception(j, i, 1);
                    break;
                case 'B':
                    AddCellKind(j, i, CellKind::Key);
                    break;
                case 'K':
                    AddCellKind(j, i, CellKind::Keymaker);
                    break;
                }
        }
    }
    pair<int, int> KeymakerCoords() const
    {
        for (int i = 0; i < MaxX; i++)
            for (int j = 0; j < MaxY; j++)
                if ((int)v[i][j] & (int)CellKind::Keymaker)
                    return {i, j};
        return {-1, -1};
    }
    vector<pair<pair<int, int>, char>> Vision(int x, int y) const
    {
        vector<pair<pair<int, int>, char>> res;
        int endx = min(MaxX - 1, x + radius);
        int endy = min(MaxY - 1, y + radius);
        for (int i = max(0, x - radius); i <= endx; i++)
            for (int j = max(0, y - radius); j <= endy; j++)
            {
                int ck = (int)v[i][j];
                while (ck)
                {
                    res.emplace_back(make_pair(i, j),
                                     KindToChar((CellKind)(ck & -ck)));
                    ck &= ck - 1;
                }
            }
        return res;
    }
    bool CellIsSafe(int x, int y) const
    {
        return ValidateCell(x, y) && ::CellIsSafe(v[x][y]);
    }
    int Radius() const
    {
        return radius;
    }
    int Solution() const
    {
        pair<int, int> adj[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        int dists[MaxX][MaxY];
        fill_n(&dists[0][0], MaxX * MaxY, -1);
        dists[0][0] = 0;
        queue<pair<int, int>> q;
        q.emplace(0, 0);
        int destx, desty;
        {
            auto p = KeymakerCoords();
            destx = p.first;
            desty = p.second;
        }
        while (q.size())
        {
            auto p = q.front();
            q.pop();
            int x = p.first;
            int y = p.second;
            int newdist = dists[x][y] + 1;
            for (auto d : adj)
            {
                int nx = x + d.first;
                int ny = y + d.second;
                if (nx == destx && ny == desty)
                    return newdist;
                if (!CellIsSafe(nx, ny))
                    continue;
                if (dists[nx][ny] != -1)
                    continue;
                dists[nx][ny] = newdist;
                q.emplace(nx, ny);
            }
        }
        return -1;
    }
};

int main(int argc, char** argv)
{
    if (argc <= 2)
    {
        cout << "Usage: interactor [-io] [-stats] program.bin testfile.txt" << endl;
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
    fprintf(childw, "%d\n", mp.Radius());
    if (showio)
        cout << mp.Radius() << endl;
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
            auto vis = mp.Vision(x, y);
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
