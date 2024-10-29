#include "interactorMap.h"
#include <climits>
#include <cstdio>
#include <fstream>
#include <sstream>
using namespace std;

FILE* randgen;

int randint()
{
    int res;
    fread(&res, sizeof(int), 1, randgen);
    res &= INT_MAX;
    return res;
}

int randint(int n)
{
    return randint() % n;
}

int randint(int min, int exmax)
{
    return randint(exmax - min) + min;
}

template <class T> void quickremove(vector<T>& v, int index)
{
    v[index] = v.back();
    v.pop_back();
}

void RenderMap(stringstream& output, string* ans)
{
    for (int i = 0; i < 9; i++)
        output << '\n' << ans[i];
}

stringstream GenerateTestcase()
{
    int nobstacles = randint(3, 12);
    vector<pair<int, int>> allcoords(81);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            allcoords[i * 9 + j] = {i, j};
    quickremove(allcoords, 0);
    vector<pair<int, int>> chosen(nobstacles);
    for (int i = 0; i < nobstacles; i++)
    {
        chosen[i] = allcoords[randint(allcoords.size())];
        quickremove(allcoords, i);
    }
    string ans[9];
    for (string& s : ans)
        s.assign(9, '.');
    for (auto& p : chosen)
        ans[p.second][p.first] = randint(2) ? 'A' : 'S';
    stringstream ss;
    RenderMap(ss, ans);
    Map proto(ss);
    ss.seekg(0);
    ss.seekp(0);
    if (!proto.CellIsSafe(0, 0))
        return GenerateTestcase();
    for (int i = (int)allcoords.size() - 1; i >= 0; i--)
    {
        pair<int, int> coords = allcoords[i];
        if (!proto.CellIsSafe(coords.first, coords.second))
            quickremove(allcoords, i);
    }
    int index = randint(allcoords.size());
    pair<int, int> key = allcoords[index];
    quickremove(allcoords, index);
    pair<int, int> keymaker = allcoords[randint(allcoords.size())];
    ans[key.second][key.first] = 'B';
    ans[keymaker.second][keymaker.first] = 'K';
    RenderMap(ss, ans);
    return ss;
}

int main(int argc, char** argv)
{
    randgen = fopen("/dev/random", "r");

    if (argc < 5)
    {
        cout << "Usage: testgen #solvable #unsolvable "
             << "startindex prefix/of/files" << endl;
        return 0;
    }

    int needsol = atoi(argv[1]);
    int needunsol = atoi(argv[2]);
    int startindex = atoi(argv[3]);
    string fileprefix = argv[4];

    int qsolvable = needsol, qunsolvable = needunsol;
    int curindex = startindex;
    while (qsolvable + qunsolvable)
    {
        auto ss = GenerateTestcase();
        Map mp(ss);
        ss.seekg(0);
        bool solv = mp.Solution() != -1;
        int& quota = solv ? qsolvable : qunsolvable;
        if (!quota)
            continue;
        quota--;
        string filename = fileprefix + to_string(curindex++);
        ofstream fs(filename, ios::out);
        fs << ss.rdbuf();
    }
    cout << "Generated " << needsol + needunsol << " testcases" << endl;

    fclose(randgen);
}
