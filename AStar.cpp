#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

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
    Unknown,
    Empty,
    Percepted,
    Agent,
    Key,
    Sentinel,
    Keymaker,
};

struct Map {
    static constexpr int MaxX = 8, MaxY = 8;
    static std::pair<int, int> Adjacent[4];

  private:
    CellKind v[MaxX][MaxY];

  public:
    void ResetMap() { std::fill_n(&v[0][0], MaxX * MaxY, CellKind::Unknown); }
    Map() { ResetMap(); }
    inline bool ValidateCell(int x, int y) {
        return x >= 0 && x <= MaxX && y >= 0 && y <= MaxY;
    }
    std::vector<std::pair<int, int>> SafePath(int x1, int y1, int x2, int y2) {
        using namespace std;
        set<Node, NodeAStarOrder> pq;
        Node nodes[MaxX][MaxY];
        for (int i = 0; i <= MaxX; i++)
            for (Node &nd : nodes[i])
                nd.heuristic = nd.dist = -1;
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
                    !(ValidateCell(x, y) && v[x][y] == CellKind::Empty))
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
};

std::pair<int, int> Map::Adjacent[] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

int main() {
    std::vector<Node> nodes;

    int variant;
    std::cin >> variant;
    nodes.emplace_back(0, 0, 0, 0);
    std::set<Node, NodeAStarOrder> pq;
    pq.insert(nodes[0]);
}
