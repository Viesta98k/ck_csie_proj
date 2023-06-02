#include<fstream>
#include<string>
#include<vector>

#include"vocfile.hpp"

using namespace std;

void VocFile::openFile(string file_name){
    ifstream file(file_name);  
    if(file.is_open()){
        string line;
        while(getline(file,line)){
            vector<string>tmp;
            while(line.find('\\')!=string::npos){
                tmp.push_back(line.substr(0,line.find('\\')));
                line=line.substr(line.find('\\')+1);
            }
            this->memory.push_back(tmp);
        }
    }
    this->current_file_name=file_name;
}
void VocFile::closeFile(){
    ofstream file(this->current_file_name);
    if(file.is_open()){
        for(int i=0;i<this->memory.size();i++){
            for(int j=0;j<this->memory[i].size();j++){
                file<<this->memory[i][j];
                if(j!=this->memory[i].size()-1)
                    file<<'\\';
            }
            file<<'\n';
        }
    }
}

string VocFile::readValue(int row,int column){
    return this->memory[row][column];
}

void VocFile::writeValue(int row, int column, string value){
    if(this->memory.size()<row)
        this->memory.resize(row);
    if(this->memory[row].size()<column)
        this->memory[row].resize(column);
    this->memory[row][column]=value;
}


