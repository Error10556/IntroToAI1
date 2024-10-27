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

public:
    // Forget everything
    void ResetMap()
    {
        std::fill_n(&v[0][0], TopX * TopY, CellKind::Unknown);
    }
    // Find the length of the shortest path from (x1, y1) to (x2, y2) through
    // safe cells only
    int ShortestSafePath(int x1, int y1, int x2, int y2);
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
};

// Find the length of the shortest path from (x1, y1) to (x2, y2) through safe
// cells only
// Uses Breadth First Search
int Map::ShortestSafePath(int x1, int y1, int x2, int y2)
{
    // The distances are initially -1 since they are unknown
    int dists[TopX][TopY];
    std::fill_n(&dists[0][0], TopX * TopY, -1);
    // The starting node has distance 0
    dists[x1][y1] = 0;
    int x = x1, y = y1;
    // The queue of cells to visit
    std::queue<std::pair<int, int>> q;
    q.emplace(x1, y1);
    while (!q.empty())
    {
        // (x, y) is the current cell
        auto xy = q.front();
        q.pop();
        int x = xy.first;
        int y = xy.second;
        // The distances to adjacent cells
        int newdist = dists[x][y] + 1;
        for (auto& p : Adjacent)
        {
            // The adjacent cell is (nx, ny)
            int nx = x + p.first;
            int ny = y + p.second;
            // Immediately return if we found the distance
            if (nx == x2 && ny == y2)
                return newdist;
            // If the adjacent cell exists and is unseen and safe
            if (ValidateCell(nx, ny) && dists[nx][ny] == -1 &&
                CellIsSafe(v[nx][ny]))
            {
                // expand there
                dists[nx][ny] = newdist;
                q.emplace(nx, ny);
            }
        }
    }
    // Target could not be reached
    return -1;
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
// visited[x][y] = true iff the cell (x, y) has been visited by Neo
bool visited[Map::TopX][Map::TopY]{};

void DFS(Map& mp, int x, int y, int visionRadius)
{
    // We are in (x, y)
    visited[x][y] = true;
    for (auto& p : Map::Adjacent)
    {
        // An adjacent cell is (nx, ny)
        int nx = x + p.first;
        int ny = y + p.second;
        // Skip if we cannot go to (nx, ny) or we have been there
        if (!Map::ValidateCell(nx, ny) || !CellIsSafe(mp.Cell(nx, ny)) ||
            visited[nx][ny])
            continue;
        // Move there, explore, and go back
        MakeMoveAndRead(mp, nx, ny, visionRadius);
        DFS(mp, nx, ny, visionRadius);
        MakeMoveAndRead(mp, x, y, visionRadius);
    }
}

int main()
{
    // How far Neo sees
    int variant;
    std::cin >> variant;
    // The goal
    int targetx, targety;
    std::cin >> targetx >> targety;

    // Initialize the knowledge map
    Map mp;
    // Initially, receive information about what is seen from (0, 0)
    MakeMoveAndRead(mp, 0, 0, variant);
    // Explore
    DFS(mp, 0, 0, variant);
    // Now we know all safe cells
    // Output the shortest path's length or -1 if it does not exist
    std::cout << "e " << mp.ShortestSafePath(0, 0, targetx, targety)
              << std::endl;
}
