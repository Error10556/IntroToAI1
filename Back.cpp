#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

inline int ManhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

enum class CellKind {
    Unknown = 1,
    Empty = 0,
    Percepted = 2,
    Agent = 4,
    Key = 8,
    Sentinel = 16,
    Keymaker = 32,
};

CellKind CellKindFromChar(char ch) {
    switch (ch) {
    case 'P':
        return CellKind::Percepted;
    case 'A':
        return CellKind::Agent;
    case 'B':
        return CellKind::Empty;
    case 'S':
        return CellKind::Sentinel;
    case 'K':
        return CellKind::Keymaker;
    }
    return CellKind::Empty;
}

inline bool CellIsSafe(CellKind cell) {
    return (static_cast<int>(cell) & ~(static_cast<int>(CellKind::Keymaker))) ==
           0;
}

struct Map {
    static constexpr int MaxX = 9, MaxY = 9;
    static std::pair<int, int> Adjacent[4];

  private:
    CellKind v[MaxX + 1][MaxY + 1];

  public:
    void ResetMap() {
        std::fill_n(&v[0][0], (MaxX + 1) * (MaxY + 1), CellKind::Unknown);
    }
    int ShortestSafePath(int x1, int y1, int x2, int y2);
    Map() { ResetMap(); }
    inline static bool ValidateCell(int x, int y) {
        return x >= 0 && x <= MaxX && y >= 0 && y <= MaxY;
    }
    void Set(int x, int y, CellKind cell) {
        if (ValidateCell(x, y))
            v[x][y] = cell;
    }
    inline void ClearCell(int x, int y) { Set(x, y, CellKind::Empty); }
    void Add(int x, int y, CellKind cell) {
        if (ValidateCell(x, y))
            v[x][y] = static_cast<CellKind>(static_cast<int>(v[x][y]) |
                                            static_cast<int>(cell));
    }
    inline CellKind Cell(int x, int y) { return v[x][y]; }
};

int Map::ShortestSafePath(int x1, int y1, int x2, int y2)
{
    int dists[MaxX + 1][MaxY + 1];
    std::fill_n(&dists[0][0], (MaxX + 1) * (MaxY + 1), -1);
    dists[x1][y1] = 0;
    int x = x1, y = y1;
    std::queue<std::pair<int, int>> q;
    q.emplace(x1, y1);
    while (!q.empty())
    {
        auto xy = q.front();
        q.pop();
        int x = xy.first;
        int y = xy.second;
        int newdist = dists[x][y] + 1;
        for (auto& p : Adjacent)
        {
            int nx = x + p.first;
            int ny = y + p.second;
            if (nx == x2 && ny == y2)
                return newdist;
            if (ValidateCell(nx, ny) && CellIsSafe(v[nx][ny]))
            {
                dists[nx][ny] = newdist;
                q.emplace(nx, ny);
            }
        }
    }
    return -1;
}

std::pair<int, int> Map::Adjacent[] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

void ReadSurroundings(Map &mp, int x, int y, int radius) {
    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++)
            mp.ClearCell(x + i, y + j);
    int n;
    std::cin >> n;
    while (n--) {
        int x, y;
        char type;
        std::cin >> x >> y >> type;
        CellKind kind = CellKindFromChar(type);
        mp.Add(x, y, kind);
    }
}

void MakeMoveAndRead(Map &mp, int newx, int newy, int radius) {
    std::cout << "m " << newx << ' ' << newy << std::endl;
    ReadSurroundings(mp, newx, newy, radius);
}

bool visited[Map::MaxX + 1][Map::MaxY + 1]{};

void DFS(Map &mp, int x, int y, int visionRadius) {
    visited[x][y] = true;
    for (auto &p : Map::Adjacent) {
        int nx = x + p.first;
        int ny = y + p.second;
        if (!Map::ValidateCell(nx, ny) || !CellIsSafe(mp.Cell(nx, ny))
                || visited[nx][ny])
            continue;
        MakeMoveAndRead(mp, nx, ny, visionRadius);
        DFS(mp, nx, ny, visionRadius);
        MakeMoveAndRead(mp, x, y, visionRadius);
    }
}

int main()
{
    int variant;
    std::cin >> variant;
    int targetx, targety; std::cin >> targetx >> targety;
    Map mp;
    MakeMoveAndRead(mp, 0, 0, variant);
    DFS(mp, 0, 0, variant);
    std::cout << "e " << mp.ShortestSafePath(0, 0, targetx, targety)
        << std::endl;
}
