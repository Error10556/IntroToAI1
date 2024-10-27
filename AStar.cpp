#include <algorithm>
#include <iostream>
#include <set>
#include <tuple>
#include <vector>

inline int ManhattanDistance(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

// A node for the A* algorithm
struct Node
{
    int x, y;             // The coordinates of the node
    Node* back = nullptr; // Where we came from (optimally)
    int dist;             // The optimal distance from the start
    int heuristic; // The lower bound for the distance from here to the goal
    Node() : Node(0, 0, 0, 0)
    {
    }
    Node(int x, int y, int dist, int heuristic)
        : x(x), y(y), dist(dist), heuristic(dist + heuristic)
    {
    }
    inline int Estimate() const // How promising this node is
    {
        return dist + heuristic;
    }
};

// A comparator that orders A* nodes by their Estimates (increasingly)
struct NodeAStarOrder
{
    bool operator()(const Node& a, const Node& b) const
    {
        int ae = a.Estimate(), be = b.Estimate();
        int ah = a.heuristic, bh = b.heuristic;
        // Sort by:
        // 1. The estimates
        // 2. If equal - by distance to goal
        // 3. If equal - arbitrarily
        return std::tie(ae, ah, a.x, a.y) < std::tie(be, bh, b.x, b.y);
    }
};

// Bitmask that lists all objects in a cell
enum class CellKind
{
    Unknown = 1,   // The cell has not been seen by Neo
    Empty = 0,     // No objects
    Perceived = 2, // The cell is perceived by an enemy
    Agent = 4,     // An agent is here
    Sentinel = 8,  // A sentinel is here
    Keymaker = 16, // The keymaker is here
                   // Ignore the Backdoor key since using it is optional
};

// Get the object representation from its mnemonic
CellKind CellKindFromChar(char ch)
{
    switch (ch)
    {
    case 'P':
        return CellKind::Perceived;
    case 'A':
        return CellKind::Agent;
    case 'B':
        return CellKind::Empty;
    case 'S':
        return CellKind::Sentinel;
    case 'K':
        return CellKind::Keymaker;
    }
    return CellKind::Empty; // Ignore the backdoor key
}

// Returns true iff Neo can move to the specified cell
inline bool CellIsSafe(CellKind cell)
{
    return (static_cast<int>(cell) & ~(static_cast<int>(CellKind::Keymaker))) ==
           0;
}

// What Neo knows about the environment
struct Map
{
    // The dimensions of the map
    static constexpr int TopX = 9, TopY = 9;
    // The list of 4 directions in which Neo can travel
    static std::pair<int, int> Adjacent[4];

private:
    CellKind v[TopX][TopY];

public:
    // Forget everything
    void ResetMap()
    {
        std::fill_n(&v[0][0], TopX * TopY, CellKind::Unknown);
    }
    Map()
    {
        ResetMap();
    }
    // Is the cell with these coordinates within the boundaries?
    inline static bool ValidateCell(int x, int y)
    {
        return x >= 0 && x < TopX && y >= 0 && y < TopY;
    }
    // Find the shortest path from (x1, y1) to (x2, y2) through safe cells only
    std::vector<std::pair<int, int>> SafePath(int x1, int y1, int x2, int y2);
    // Assign the knowledge about (x, y) cell
    void Set(int x, int y, CellKind cell)
    {
        if (ValidateCell(x, y))
            v[x][y] = cell;
    }
    // Remove all objects from cell (x, y), i.e. make it known and empty.
    inline void ClearCell(int x, int y)
    {
        Set(x, y, CellKind::Empty);
    }
    // Add the given object(s) to the cell (x, y)
    void Add(int x, int y, CellKind cell)
    {
        if (ValidateCell(x, y))
            v[x][y] = static_cast<CellKind>(static_cast<int>(v[x][y]) |
                                            static_cast<int>(cell));
    }
    // return the knowledge about cell (x, y)
    inline CellKind Cell(int x, int y)
    {
        return v[x][y];
    }
};

// Find the shortest path from (x1, y1) to (x2, y2) through safe cells only
// Uses A* algorithm
std::vector<std::pair<int, int>> Map::SafePath(int x1, int y1, int x2, int y2)
{
    using namespace std;
    set<Node, NodeAStarOrder> pq; // The priority queue of nodes
    Node nodes[TopX][TopY];
    for (int i = 0; i < TopX; i++)
        for (int j = 0; j < TopY; j++)
        {
            // Initially, all cells are unknown - denote this by assigning -1
            Node& nd = nodes[i][j];
            nd.x = i;
            nd.y = j;
            nd.heuristic = nd.dist = -1;
        }
    // The first cell is known
    nodes[x1][y1] = Node(x1, y1, 0, ManhattanDistance(x1, y1, x2, y2));
    pq.insert(nodes[x1][y1]);
    while (pq.size())
    {
        // Pop the best node for expansion
        auto begin = pq.begin();
        Node cur = *begin;
        int curx = cur.x;
        int cury = cur.y;
        // Is this the target?
        if (curx == x2 && cury == y2)
            break;
        pq.erase(begin);
        int newdist = cur.dist + 1; // The distance to adjacent nodes
        for (auto d : Adjacent)
        {
            // (x, y) is the adjacent node
            int x = curx + d.first;
            int y = cury + d.second;
            // Skip if this is not the goal, but the cell is blocked
            if (!(x == x2 && y == y2) &&
                !(ValidateCell(x, y) && CellIsSafe(v[x][y])))
                continue;
            Node& cur = nodes[x][y];
            // If cur is an unseen-before cell, make it seen
            if (cur.dist == -1)
            {
                cur.dist = newdist;
                cur.heuristic = ManhattanDistance(x, y, x2, y2);
                cur.back = &nodes[curx][cury];
                pq.insert(cur);
            }
            // If cur has been seen, we update the distance if it got shorter
            else if (cur.dist > newdist)
            {
                pq.erase(cur);
                cur.dist = newdist;
                cur.back = &nodes[curx][cury];
                pq.insert(cur);
            }
        }
    }
    Node& fin = nodes[x2][y2];
    // If we could not reach the goal, there is no path
    if (fin.dist == -1)
        return {};
    // Traverse the path backwards
    vector<pair<int, int>> res;
    Node* cur = &nodes[x2][y2];
    while (cur->x != x1 || cur->y != y1)
    {
        res.emplace_back(cur->x, cur->y);
        cur = cur->back;
    }
    // flip the path, since it is written backwards
    reverse(res.begin(), res.end());
    return res;
}

std::pair<int, int> Map::Adjacent[] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

// Handle the input from the interactor
void ReadSurroundings(Map& mp, int x, int y, int radius)
{
    // Reset everything within vision
    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++)
            mp.ClearCell(x + i, y + j);
    // And receive the information
    int n;
    std::cin >> n;
    while (n--)
    {
        int x, y;
        char type;
        std::cin >> x >> y >> type;
        CellKind kind = CellKindFromChar(type);
        mp.Add(x, y, kind);
    }
}

// Tell the interactor that we move to (newx, newy) and handle the input
void MakeMoveAndRead(Map& mp, int newx, int newy, int radius)
{
    std::cout << "m " << newx << ' ' << newy << std::endl;
    ReadSurroundings(mp, newx, newy, radius);
}

int main()
{
    // How far Neo sees
    int variant;
    std::cin >> variant;
    // The goal
    int targetx, targety;
    std::cin >> targetx >> targety;

    // The A* algorithm
    Node nodes[Map::TopX][Map::TopY];
    std::set<Node, NodeAStarOrder> pq; // The priority queue
    for (int i = 0; i < Map::TopX; i++)
        for (int j = 0; j < Map::TopY; j++)
        {
            // Initially, the nodes are all unknown
            Node& nd = nodes[i][j];
            nd.x = i;
            nd.y = j;
            nd.heuristic = nd.dist = -1;
        }
    // The starting node (0, 0) is safe
    nodes[0][0] = Node(0, 0, 0, ManhattanDistance(0, 0, targetx, targety));
    int curx = 0, cury = 0;
    // Initialize the knowledge map
    Map mp;

    // Nested function that makes Neo travel to (x, y) through safe nodes
    // in as few moves as possible
    auto MoveTo = [&](int x, int y) -> void {
        // The path to the goal (x, y)
        std::vector<std::pair<int, int>> moves;
        if (ManhattanDistance(curx, cury, x, y) > 1)
            moves = mp.SafePath(curx, cury, x, y);
        else
            moves.emplace_back(x, y);
        // Make the moves
        for (auto p : moves)
            MakeMoveAndRead(mp, p.first, p.second, variant);
        curx = x;
        cury = y;
        // Expand (x, y) for the A* algorithm
        int newdist = nodes[x][y].dist + 1;
        for (auto p : Map::Adjacent)
        {
            // The adjacent cell is (nx, ny)
            int nx = x + p.first;
            int ny = y + p.second;
            // Skip if that cell is blocked
            if (!Map::ValidateCell(nx, ny) || !CellIsSafe(mp.Cell(nx, ny)))
                continue;
            Node& cur = nodes[nx][ny];
            // if the node has not been seen, make it seen
            if (cur.dist == -1)
            {
                cur.dist = newdist;
                cur.heuristic = ManhattanDistance(x, y, targetx, targety);
                cur.back = &nodes[curx][cury];
                pq.insert(cur);
            }
            // If it has, update the optimal predecessor
            else if (cur.dist > newdist)
            {
                pq.erase(cur);
                cur.dist = newdist;
                cur.back = &nodes[curx][cury];
                pq.insert(cur);
            }
        }
    };
    // Initially, receive information about what is seen from (0, 0)
    MoveTo(0, 0);
    // While we have not reached the Keymaker
    while (!pq.empty() && nodes[targetx][targety].dist == -1)
    {
        // Look for the most promising node to expand
        auto iter = pq.begin();
        // The best cells are those with the lowest estimate, out of those -
        // with the lowest lower-bound for the remaining distance
        std::pair<int, int> est(iter->Estimate(), iter->heuristic);
        // Write down all such nodes and their iterators in the priority queue
        std::vector<std::pair<int, int>> nexts;
        std::vector<decltype(pq)::const_iterator> iters;
        while (iter != pq.end() &&
               std::make_pair(iter->Estimate(), iter->heuristic) == est)
        {
            iters.emplace_back(iter);
            nexts.emplace_back(iter->x, iter->y);
            ++iter;
        }
        // Out of all optimal cells, choose the one closest to Neo
        int bestindex = 0;
        // If we have to choose
        if (nexts.size() > 1)
        {
            int near = 100; // The best distance seen so far
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
        // nexts[bestindex] is the best cell to expand, so travel there
        pq.erase(iters[bestindex]);
        MoveTo(nexts[bestindex].first, nexts[bestindex].second);
    }
    // Output the shortest distance to the Keymaker or -1 if no safe path exists
    std::cout << "e " << nodes[targetx][targety].dist << std::endl;
}
