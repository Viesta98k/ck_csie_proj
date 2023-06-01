#include<iostream>
#include<string>
#include<vector>
#include"csv.hpp"

using namespace std;

class Csv;

bool isVowel(char c){
    return (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||c=='y');
}

int main(){
    Csv csv;
    string target_file;
    cin>>target_file;
    csv.openFile(target_file);
    int i=1;
    string tmp=csv.readValueWithIndex(0,i);
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
        csv.writeValueWithIndex(2,i, to_string(syllabus*tmp.length()));
        i++;
        tmp=csv.readValueWithIndex(1,i);
    }
}