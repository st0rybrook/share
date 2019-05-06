#include <iostream>
#include <vector>
using namespace std;
vector<int> longestArray(vector<int> v)
{
    vector<int> result;

    if (v.size() == 1)
    {
        return v;
    }
    result.push_back(v[0]);
    int index = 0;
    for (int i = 1; i < v.size(); i++)
    {
        if (v[i] > result[index])
        {
            result.push_back(v[i]);
            index++;
        }
        else
        {
            result[index] = v[i];
        }
    }
    return result;
}
int main()
{
    vector<int> v = {5,9,4,1,3,7,6,7};
    vector<int> result;
    result = longestArray(v);
    for (int i = 0; i < result.size(); i++)
        cout << result[i] << " " << endl;
}
