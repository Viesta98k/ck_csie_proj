#include<fstream>
#include<string>
#include<vector>

#ifndef VOCFILE_H
#define VOCFILE_H

using namespace std;

class VocFile{
  public:
    void openFile(string file_name);    //開啟檔案，載入記憶體
    string readValue(int row,int column);   //索引從0開始
    void writeValue(int row, int column, string value);
    void closeFile();
  private:
    string current_file_name;
    vector<vector<string>>memory;
};

#endif