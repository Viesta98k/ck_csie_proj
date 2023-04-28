#include<fstream>
#include<string>
#include<vector>

using namespace std;

class Csv{
  public:
    void openFile(string fname);
    void closeFile();
    string readValueWithIndex(int row,int column);
    vector<vector<string>> readValueWithRange(int start_row,int end_row,int start_column,int end_column);
    void writeValueWithIndex(int row,int column,string data);
    void writeValueWithRange(int start_row,int end_row,int start_column,int end_column,vector<vector<string>> datas);
  private:
    string current_file_name;
    vector<vector<string>>memory;
};