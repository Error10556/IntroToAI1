#include "interactorMap.h"
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Usage: testVisualizer testfile.txt" << endl;
        return 0;
    }
    fstream fs(argv[1], ios::in);
    Map mp(fs);
    fs.close();

    cout << "\\begin{tikzpicture}\n";
    cout << "    \\node at (0.5,-0.5) {\\Huge{N}};\n";
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            if (i == 0 && j == 0)
                continue;
            int kind = (int)mp.Cell(i, j);
            if (kind & (int)CellKind::Perceived)
            {
                cout << "    \\fill[pink] (" << i << ',' << -j << ") rectangle"
                     << " (" << i + 1 << ',' << -j - 1 << ");\n";
                kind &= ~(int)CellKind::Perceived;
            }
            if (kind)
                cout << "    \\node at (" << i << ".5,-" << j << ".5) {"
                     << "\\Huge{" << KindToChar((CellKind)kind) << "}};\n";
        }
    cout << "    \\draw[very thick, step=1.0] (0,0) grid (9,-9);\n";
    cout << "\\end{tikzpicture}\n";
}
