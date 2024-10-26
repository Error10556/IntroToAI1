#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include <tuple>

inline int ManhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

struct Node {
    int x, y;
    Node *back = nullptr;
    int dist;
    int heuristic;
    Node() : Node(0, 0, 0, 0) {}
    Node(int x, int y, int dist, int heuristic)
        : x(x), y(y), dist(dist), heuristic(dist + heuristic) {}
    inline int Estimate() const { return dist + heuristic; }
};

struct NodeAStarOrder {
    bool operator()(const Node &a, const Node &b) const {
        int ae = a.Estimate(), be = b.Estimate();
        return std::tie(ae, a.x, a.y) < std::tie(be, b.x, b.y);
    }
};

enum class CellKind {
    Unknown = 1,
    Empty = 0,
    Percepted = 2,
    Agent = 4,
    Sentinel = 8,
    Keymaker = 16,
};

CellKind CellKindFromChar(char ch)
{
    switch (ch)
    {
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

inline bool CellIsSafe(CellKind cell)
{
    return (static_cast<int>(cell) & ~(static_cast<int>(CellKind::Keymaker)))
        == 0;
}

struct Map {
    static constexpr int MaxX = 9, MaxY = 9;
    static std::pair<int, int> Adjacent[4];

  private:
    CellKind v[MaxX + 1][MaxY + 1];

  public:
    void ResetMap() { std::fill_n(&v[0][0], (MaxX + 1) * (MaxY + 1), CellKind::Unknown); }
    Map() { ResetMap(); }
    inline static bool ValidateCell(int x, int y) {
        return x >= 0 && x <= MaxX && y >= 0 && y <= MaxY;
    }
    std::vector<std::pair<int, int>> SafePath(int x1, int y1, int x2, int y2);
    void Set(int x, int y, CellKind cell)
    {
        if (ValidateCell(x, y))
            v[x][y] = cell;
    }
    inline void ClearCell(int x, int y)
    {
        Set(x, y, CellKind::Empty);
    }
    void Add(int x, int y, CellKind cell)
    {
        if (ValidateCell(x, y))
            v[x][y] = static_cast<CellKind>(static_cast<int>(v[x][y]) |
                    static_cast<int>(cell));
    }
    inline CellKind Cell(int x, int y)
    {
        return v[x][y];
    }
};

std::vector<std::pair<int, int>> Map::SafePath(int x1, int y1, int x2, int y2)
{
    using namespace std;
    set<Node, NodeAStarOrder> pq;
    Node nodes[MaxX + 1][MaxY + 1];
    for (int i = 0; i <= MaxX; i++)
        for (int j = 0; j <= MaxY; j++)
        {
            Node& nd = nodes[i][j];
            nd.x = i;
            nd.y = j;
            nd.heuristic = nd.dist = -1;
        }
    nodes[x1][y1] = Node(x1, y1, 0, ManhattanDistance(x1, y1, x2, y2));
    pq.insert(nodes[x1][y1]);
    while (pq.size()) {
        auto begin = pq.begin();
        Node cur = *begin;
        int curx = cur.x;
        int cury = cur.y;
        if (curx == x2 && cury == y2)
            break;
        pq.erase(begin);
        int newdist = cur.dist + 1;
        for (auto d : Adjacent) {
            int x = curx + d.first;
            int y = cury + d.second;
            if (!(x == x2 && y == y2) &&
                    !(ValidateCell(x, y) && CellIsSafe(v[x][y])))
                continue;
            Node &cur = nodes[x][y];
            if (cur.dist == -1) {
                cur.dist = newdist;
                cur.heuristic = ManhattanDistance(x, y, x2, y2);
                cur.back = &nodes[curx][cury];
                pq.insert(cur);
            } else if (cur.dist > newdist) {
                pq.erase(cur);
                cur.dist = newdist;
                cur.back = &nodes[curx][cury];
                pq.insert(cur);
            }
        }
    }
    Node &fin = nodes[x2][y2];
    if (fin.dist == -1)
        return {};
    vector<pair<int, int>> res;
    Node* cur = &nodes[x2][y2];
    while (cur->x != x1 || cur->y != y1)
    {
        res.emplace_back(cur->x, cur->y);
        cur = cur->back;
    }
    reverse(res.begin(), res.end());
    return res;
}

std::pair<int, int> Map::Adjacent[] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

void ReadSurroundings(Map& mp, int x, int y, int radius)
{
    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++)
            mp.ClearCell(x + i, y + j);
    int n; std::cin >> n;
    while (n--)
    {
        int x, y; char type;
        std::cin >> x >> y >> type;
        CellKind kind = CellKindFromChar(type);
        mp.Add(x, y, kind);
    }
}

void MakeMoveAndRead(Map& mp, int newx, int newy, int radius)
{
    std::cout << "m " << newx << ' ' << newy << std::endl;
    ReadSurroundings(mp, newx, newy, radius);
}

int main() {
    int variant;
    std::cin >> variant;
    int targetx, targety; std::cin >> targetx >> targety;
    Node nodes[Map::MaxX + 1][Map::MaxY + 1];
    std::set<Node, NodeAStarOrder> pq;
    for (int i = 0; i <= Map::MaxX; i++)
        for (int j = 0; j <= Map::MaxY; j++)
        {
            Node& nd = nodes[i][j];
            nd.x = i;
            nd.y = j;
            nd.heuristic = nd.dist = -1;
        }
    nodes[0][0] = Node(0, 0, 0, ManhattanDistance(0, 0, targetx, targety));
    int curx = 0, cury = 0;
    Map mp;

    auto MoveTo = [&](int x, int y) -> void {
        std::vector<std::pair<int, int>> moves;
        if (ManhattanDistance(curx, cury, x, y) > 1)
            moves = mp.SafePath(curx, cury, x, y);
        else
            moves.emplace_back(x, y);
        for (auto p : moves)
            MakeMoveAndRead(mp, p.first, p.second, variant);
        curx = x;
        cury = y;
        int newdist = nodes[x][y].dist + 1;
        for (auto p : Map::Adjacent)
        {
            int nx = x + p.first;
            int ny = y + p.second;
            if (Map::ValidateCell(nx, ny) && CellIsSafe(mp.Cell(nx, ny)))
            {
                Node &cur = nodes[x][y];
                if (cur.dist == -1) {
                    cur.dist = newdist;
                    cur.heuristic = ManhattanDistance(x, y, targetx, targety);
                    cur.back = &nodes[curx][cury];
                    pq.insert(cur);
                } else if (cur.dist > newdist) {
                    pq.erase(cur);
                    cur.dist = newdist;
                    cur.back = &nodes[curx][cury];
                    pq.insert(cur);
                }
            }
        }
    };
    MoveTo(0, 0);
    while (!pq.empty() && nodes[targetx][targety].dist == -1)
    {
        auto iter = pq.begin();
        int est = iter->Estimate();
        std::vector<std::pair<int, int>> nexts;
        std::vector<decltype(pq)::const_iterator> iters;
        while (iter != pq.end() && iter->Estimate() == est)
        {
            iters.emplace_back(iter);
            nexts.emplace_back(iter->x, iter->y);
            ++iter;
        }
        int bestindex = 0;
        if (nexts.size() > 1)
        {
            int near = 100;
            for (int i = 0; i < nexts.size(); i++)
            {
                auto& p = nexts[i];
                int dst = mp.SafePath(curx, cury, p.first, p.second).size();
                if (dst < near)
                {
                    near = dst;
                    bestindex = i;
                }
            }
        }
        pq.erase(iters[bestindex]);
        MoveTo(nexts[bestindex].first, nexts[bestindex].second);
    }
    std::cout << "e " << nodes[targetx][targety].dist;
}
