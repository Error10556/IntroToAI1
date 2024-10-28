#include "interactorMap.h"
#include <cstdlib>
#include <queue>
using namespace std;

char KindToChar(CellKind cell)
{
    if (((int)cell & ~(int)CellKind::Perceived) != 0)
        cell = (CellKind)((int)cell & ~(int)CellKind::Perceived);
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

bool CellIsSafe(CellKind cell)
{
    return (static_cast<int>(cell) & ~(static_cast<int>(CellKind::Keymaker) |
                                       static_cast<int>(CellKind::Key))) == 0;
}

void Map::ManhattanPerception(int x, int y, int d)
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

void Map::ChebyshevPerception(int x, int y, int d)
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

Map::Map(istream& in)
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

pair<int, int> Map::KeymakerCoords() const
{
    for (int i = 0; i < MaxX; i++)
        for (int j = 0; j < MaxY; j++)
            if ((int)v[i][j] & (int)CellKind::Keymaker)
                return {i, j};
    return {-1, -1};
}

vector<pair<pair<int, int>, char>> Map::Vision(int x, int y) const
{
    std::vector<std::pair<std::pair<int, int>, char>> res;
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

bool Map::CellIsSafe(int x, int y) const
{
    return ValidateCell(x, y) && ::CellIsSafe(v[x][y]);
}

int Map::Radius() const
{
    return radius;
}

int Map::Solution() const
{
    std::pair<int, int> adj[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    int dists[MaxX][MaxY];
    fill_n(&dists[0][0], MaxX * MaxY, -1);
    dists[0][0] = 0;
    queue<std::pair<int, int>> q;
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
