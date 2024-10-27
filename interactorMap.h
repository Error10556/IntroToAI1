#pragma once
#include <iostream>
#include <vector>

enum class CellKind
{
    Empty = 0,
    Perceived = 1,
    Agent = 2,
    Key = 4,
    Sentinel = 8,
    Keymaker = 16,
};

char KindToChar(CellKind cell);
bool CellIsSafe(CellKind cell);

class Map
{
public:
    static const int MaxX = 9, MaxY = 9;
    static inline bool ValidateCell(int x, int y)
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
    void ManhattanPerception(int x, int y, int d);
    void ChebyshevPerception(int x, int y, int d);

public:
    Map(std::istream& in);
    std::pair<int, int> KeymakerCoords() const;
    std::vector<std::pair<std::pair<int, int>, char>> Vision(int x,
                                                             int y) const;
    bool CellIsSafe(int x, int y) const;
    int Radius() const;
    int Solution() const;
};
