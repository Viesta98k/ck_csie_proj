#include<iostream>
#include<string>
#include<vector>
#include"VocFile.hpp"

using namespace std;

class VocFile;

bool isVowel(char c){
    return (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||c=='y');
}

int main(){
    VocFile vf;
    string target_file;
    cin>>target_file;
    vf.openFile(target_file);
    int i=1;
    string tmp=vf.readValue(0,i);
    while(tmp!=" "){
        int syllabus=0;
        bool in_syllabus=false;
        for(int j=0;j<tmp.length();j++){
            if(isVowel(tmp[j]) && ! in_syllabus)
                in_syllabus=true;
            if(! isVowel(tmp[j]) && in_syllabus){
                syllabus++;
                in_syllabus=false;
            }
        }
        vf.writeValue(2,i, to_string(syllabus*tmp.length()));
        i++;
        tmp=vf.readValue(1,i);
    }
    vf.closeFile();
}