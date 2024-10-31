#include <algorithm>
#include <iostream>
#include <queue>

inline int ManhattanDistance(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

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
    // See Map::CanContinuePath
    bool CanContinuePathHelper(int x, int y, int destx, int desty,
                               bool vis[][Map::TopY]);

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
    // Heuristic: if the path travelled so far is marked with CellKind::Visited,
    // can (x, y) be its continuation if we cannot cross our own path?
    // Returns true if there might be a way to the target from (x, y)
    bool CanContinuePath(int x, int y, int destx, int desty)
    {
        // perform a DFS
        bool vis[Map::TopX][Map::TopY]{};
        return CanContinuePathHelper(x, y, destx, desty, vis);
    }
};

// See Map::CanContinuePath
bool Map::CanContinuePathHelper(int x, int y, int destx, int desty,
                                bool vis[][Map::TopX])
{
    // If we have reached the goal, we return true
    if (x == destx && y == desty)
        return true;
    vis[x][y] = true;
    for (auto p : Adjacent)
    {
        // (nx, ny) is an adjacent cell
        int nx = x + p.first, ny = y + p.second;
        // We skip it if it is invalid, unsafe, or has been considered.
        // However, we allow unknown cells (the real path might be there)
        if (!ValidateCell(nx, ny) || vis[nx][ny] ||
            (!CellIsSafe(v[x][y]) && v[x][y] != CellKind::Unknown))
            continue;
        // if there is a possible path in that direction, return true
        if (CanContinuePathHelper(nx, ny, destx, desty, vis))
            return true;
    }
    // We couldn't find anything
    return false;
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

// This array is used by the Depth-First Search algorithm.
// dists[x][y] = the minimal distance from origin to point (x, y) so far
int dists[Map::TopX][Map::TopY];

int targetx, targety;

void DFS(Map& mp, int x, int y, int dist, int visionRadius, int& answer)
{
    // HEURISTIC: we can stop evaluating if we will not reach a better answer
    if (dist >= answer)
        return;
    // We are in (x, y)
    dists[x][y] = dist;
    // Get the set of neighbouring cells
    std::vector<std::pair<int, int>> adj(
        Map::Adjacent,
        Map::Adjacent + sizeof(Map::Adjacent) / sizeof(Map::Adjacent[0]));
    for (auto& p : adj)
    {
        p.first += x;
        p.second += y;
    }
    // HEURISTIC:
    // order them so that the one closest to the target comes first
    std::sort(adj.begin(), adj.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return ManhattanDistance(a.first, a.second, targetx,
                                           targety) <
                         ManhattanDistance(b.first, b.second, targetx, targety);
              });
    for (auto& p : adj)
    {
        // An adjacent cell is (nx, ny)
        int nx = p.first;
        int ny = p.second;
        if (nx == targetx && ny == targety)
        {
            answer = std::min(answer,
                              dist + 1); // We are one step away from the target
            return;
        }
        // Skip if we cannot go to (nx, ny) or there was a better (shorter)
        // way to get to (nx, ny)
        if (!Map::ValidateCell(nx, ny) || !CellIsSafe(mp.Cell(nx, ny)) ||
            (dists[nx][ny] < dist + 1 && dists[nx][ny] != -1))
            continue;
        // HEURISTIC:
        // We also skip if moving to (nx, ny) guarantees a dead-end
        if (!mp.CanContinuePath(nx, ny, targetx, targety))
            continue;
        // Move there, explore, and go back
        MakeMoveAndRead(mp, nx, ny, visionRadius);
        DFS(mp, nx, ny, dist + 1, visionRadius, answer);
        // HEURISTIC:
        // targetx + targety is the absolute minimum distance, cannot improve it
        if (answer == ManhattanDistance(0, 0, targetx, targety))
            return;
        MakeMoveAndRead(mp, x, y, visionRadius);
    }
}

int main()
{
    // How far Neo sees
    int variant;
    std::cin >> variant;
    // The goal
    std::cin >> targetx >> targety;

    // Initialize the knowledge map
    Map mp;
    std::fill_n(&dists[0][0], Map::TopX * Map::TopY, -1);
    // Initially, receive information about what is seen from (0, 0)
    MakeMoveAndRead(mp, 0, 0, variant);
    int res = 40; // HEURISTIC: the answer can never be greater than approx. 39
    DFS(mp, 0, 0, 0, variant, res);
    if (res == 40)
        res = -1; // Output -1 if not found
    std::cout << "e " << res << std::endl;
}
