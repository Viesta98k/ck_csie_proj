#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include"csvreader.hpp"

using namespace std;

void Csv::openFile(string fname){
    this->current_file_name=fname;
    ifstream f(current_file_name);
    string line;
    
    while (getline(f,line))
    {
        vector<string>row;
        string cell;
        stringstream ss(line);
        bool in_quotes=false;
        string current_cell;

        while (getline(ss,cell,',')){
            if(!in_quotes){
                if(!cell.empty()&&cell.front()=='"'){
                    current_cell=cell.substr(1);
                    in_quotes=true;
                }
                else{
                    row.push_back(cell);
                }
            }
            else{
                current_cell+=','+cell;
                if(!cell.empty()&&cell.back()=='"'){
                    current_cell=current_cell.substr(0,current_cell.size()-1);
                    row.push_back(current_cell);
                    in_quotes=false;
                }
            }
        }    
    }
}

void Csv::closeFile(){
    ofstream f(this->current_file_name);

    for (const auto& row : this->memory) {
        for (size_t i = 0; i < row.size(); ++i) {
            const string& cell = row[i];
            bool contains_comma = cell.find(',') != string::npos;
            bool contains_quote = cell.find('"') != string::npos;
            if (contains_comma || contains_quote) {
                f << '"';
                for (char c : cell) {
                    if (c == '"') {
                        f << "\"\""; 
                    } else {
                        f << c;
                    }
                }
                f << '"';
            } else {
                f << cell;
            }

            if (i != row.size() - 1) {
                f << ',';
            }
        }
        f << '\n';
    }
    f.close();
}

string Csv::readValueWithIndex(int row,int column){
    return this->memory[row][column];
}

void Csv::writeValueWithIndex(int row,int column,string data){
    this->memory[row][column]=data;
}

vector<vector<string>> Csv::readValueWithRange(int start_row,int end_row,int start_column,int end_column){
    vector<vector<string>>tmp;

    for(int i=start_row;i<=end_row;i++){
        vector<string>t;
        for(int j=start_column;j<=end_column;j++){
            t.push_back(this->memory[i][j]);
        }
        tmp.push_back(t);
    }
    return tmp;
}

void Csv::writeValueWithRange(int start_row,int end_row,int start_column,int end_column,vector<vector<string>> datas){
    this->memory.resize(end_row,vector<string>(end_column));

    for(int i=start_row;i<=end_row;i++){
        for(int j=start_column;j<=end_column;j++){
            this->memory[i][j]=datas[i-start_row][j-start_column];
        }
    }
}