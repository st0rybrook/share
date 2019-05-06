
// writing on a text file
#include <fstream>
#include <iostream>
using namespace std;
int main()
{
    ofstream OutFile;
    OutFile.open("out.txt");
    if (OutFile.is_open())
    {
        OutFile << "This is a line.\n";
        OutFile << "This is another line.\n";
        OutFile.close();
    }
    return 0;
}