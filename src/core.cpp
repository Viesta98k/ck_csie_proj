#include<algorithm>
#include<cmath>
#include<string>
#include<vector>
#include"csv.hpp"
#include"core.hpp"

#include <cstdlib> /* 亂數相關函數 */
#include <ctime>   /* 時間相關函數 */

using namespace std;

const string vocab_file_path="data/vocab.csv";
const string profile_path="data/profile.csv";


int randomNum(int min,int max){
    /* 固定亂數種子 */
  srand( time(NULL) );
  /* 產生亂數 */
  /* 指定亂數範圍 */
  /* 產生 [min , max) 的浮點數亂數 */
  int x = (int)(max - min) * rand() / (RAND_MAX + 1.0) + min;
  return x;
}

GameObject::GameObject(int x1,int y1,int x2,int y2, string render_data_source){
    this->x1=x1;
    this->y1=y1;
    this->x2=x2;
    this->y2=y2;
    this->render_data_source=render_data_source;
    this->rendered=false;
}

Question::Question(vector<vector<string>>vocabs_info, int vocab_num, Profile usr_info){
    vector<int>indexs;
    while(this->choices.size()<5){
        int index=randomNum(0,vocab_num);
        if(find(indexs.begin(),indexs.end(),index) == indexs.end()){
            this->choices.push_back(vocabs_info[index][0]);
            this->definitions.push_back(vocabs_info[index][1]);
            this->dff+=stoi(vocabs_info[index][2]);
            indexs.push_back(index);
        }else{
            continue;
        }
    }
    this->dff/=5;
}

vector<Vocab> Question::giveChoices(){
    return this->choices;
}

int Question::correctAns(){
    return this->ans;
}

int Question::ret_dff(){
    return this->dff;
}

SpecialQuestion::SpecialQuestion(vector<vector<string>>vocabs_info, int vocab_num, Profile usr_info){
    vector<int>indexs;
    int limit=max(10,min(30,(int)pow(usr_info.ability,0.25)*10));   //字母陣列長寬最小10，最大30
    this->width= limit;
    this->height=limit;
    int max_voc_num=max(3,min(15,(int)pow(usr_info.ability,0.25)*2)); //單字數量最少3，最多15
    while(this->vocs.size()<max_voc_num){
        if(vocs.empty()){
            int index=randomNum(0,vocab_num);
            this->vocs.push_back(vocabs_info[index][0]);
            this->definitions.push_back(vocabs_info[index][1]);
            this->dff.push_back(stoi(vocabs_info[index][2]));
            this->is_vertical.push_back(randomNum(0,2));
            this->head_relative.push_back(make_pair(0,0));
            indexs.push_back(index);
        }else{
            string voc_tmp="";
            string definition_tmp="";
            int dff_tmp=0;
            vector<vector<char>>tmp=this->giveCorrectAns();
            while(voc_tmp==""){
                int row=randomNum(0,width-3);
                int column=randomNum(0,height-3);
                bool is_vertical=randomNum(0,2);
                if(is_vertical){    //垂直
                    for(int i=0;i<vocabs_info.size();i++){  //遍歷單字表
                        bool ok=true;
                        for(int j=0;j<vocabs_info[i][0].size();j++){ //單字契合檢定
                            if(vocabs_info[i][0][j]!=tmp[row][column+j]&&tmp[row][column+j]!=NULL){
                                ok=false;
                                break;
                            }
                        }
                        if(ok){ //水平
                            voc_tmp=vocabs_info[i][0];
                            definition_tmp=vocabs_info[i][1];
                            dff_tmp=stoi(vocabs_info[i][2]);
                            break;
                        }
                    }
                }else{
                    for(int i=0;i<vocabs_info.size();i++){  //遍歷單字表
                        bool ok=true;
                        for(int j=0;j<vocabs_info[i][0].size();j++){ //單字契合檢定
                            if(vocabs_info[i][0][j]!=tmp[row+j][column]&&tmp[row+j][column]!=NULL){
                                ok=false;
                                break;
                            }
                        }
                        if(ok){
                            voc_tmp=vocabs_info[i][0];
                            definition_tmp=vocabs_info[i][1];
                            dff_tmp=stoi(vocabs_info[i][2]);
                            break;
                        }
                    }
                }
            }
            this->vocs.push_back(voc_tmp);
            this->definitions.push_back(definition_tmp);
            this->dff.push_back(dff_tmp);
        }
    }
}

vector<vector<char>> SpecialQuestion::giveCorrectAns(){
    vector<vector<char>>char_list(this->height,vector<char>(this->width,NULL));
    char_list.resize(this->height,vector<char>(this->width));
    for(int i=0;i<this->vocs.size();i++){
        if(this->is_vertical[i]){
            for(int j=0;j<this->vocs.size();j++){
                char_list[head_relative[i].first][head_relative[i].second+j]=this->vocs[i][j];
            }
        }else{
            for(int j=0;j<vocs.size();j++){
                char_list[head_relative[i].first+j][head_relative[i].second]=this->vocs[i][j];
            }
        }
    }
    return char_list;
}

vector<int> SpecialQuestion::ret_dff(){
    return this->dff;
}

int SpecialQuestion::ret_vocs_num(){
    return this->vocs.size();    
}
pair<int,int> SpecialQuestion::ret_size(){
    return make_pair(this->width,this->height);
}
Vocab SpecialQuestion::ret_voc(int i){
    return this->vocs[i];
}
string SpecialQuestion::ret_def(int i){
    return this->definitions[i];
}
bool SpecialQuestion::ret_vertical(int i){
    return this->is_vertical[i];
}
pair<int,int> SpecialQuestion::ret_relative(int i){
    return this->head_relative[i];
}
Game::Game(){
    Csv csv;
    csv.openFile(vocab_file_path);
    this->vocab_num=stoi(csv.readValueWithIndex(0,0));
    this->vocabs_info=csv.readValueWithRange(1,vocab_num,0,2);
    csv.closeFile();
    this->profile.level=max(1,min((int)(log(this->profile.ability)/log(3)),12));
    //初始化問題
    this->current_question=Question(this->vocabs_info,this->vocab_num,this->profile);
    this->current_special_question=SpecialQuestion(this->vocabs_info,this->vocab_num,this->profile);
    //初始化元件
    GameObject game_title=GameObject(320,240,960,480,"resource/title.jpg");
    static_object.push_back(game_title);
    GameObject player_protrait=GameObject(0,0,120,120,"resource/player_portrait_"+to_string(this->profile.level)+".jpg");
    static_object.push_back(player_protrait);
    GameObject enemy_portrait=GameObject(1160,0,1280,120,"resource/player_portrait_"+to_string((int)sqrt(randomNum(0,13)*this->profile.level))+".jpg");
    static_object.push_back(enemy_portrait);
}
void Game::calculate(vector<int>dff, vector<bool>is_correct){   //攻擊、防禦、特殊攻擊(combo>=3)、反擊
    switch (this->situation){
    case 0: //單選題
        if(is_correct[0]){  //攻擊成功
            this->enemy_hp-=(dff[0]/this->profile.ability)*10*this->enemy_score/(this->score+this->enemy_score);
            int score_add=100*dff[0]/this->profile.ability;
            this->score+=score_add;
            this->enemy_score-=score_add;
            this->combo++;
            this->enemy_situtation=0;   //敵方受傷
        }else{  //攻擊失敗
            int score_minus=100*this->profile.ability/dff[0];
            this->score=max(0,this->score-score_minus); //扣分
            this->enemy_score+=score_minus;
            this->enemy_situtation=1;   //敵方可反擊
        }
        break;
    case 1: //單選題
        if(is_correct[0]){  //防禦成功
            this->score+=dff[0];
            this->enemy_situtation=2;   //我方可反擊
        }else{  //防禦失敗
            if(this->enemy_situtation)
            this->hp-=(this->profile.ability/dff[0])*10*this->score/(this->score+this->enemy_score);
            int score_minus=100*this->profile.ability/dff[0];
            this->score=max(0,this->score-score_minus);  
            this->enemy_score+=score_minus;
            this->enemy_situtation=3;   //敵方將行普通攻擊
        }
        break;
    case 2: //填字
        //計算答對得分比
        int total_part_score=0, add_score=0;
        for(int i=0;i<dff.size();i++){
            total_part_score+=dff[i];
            if(is_correct[i])
                add_score+=dff[i];
        }
        //檢查轉換答對率是否大於0.5
        if(add_score/(total_part_score*this->profile.ability)>0.5){
            this->enemy_hp-=100*add_score/(total_part_score*this->profile.ability)*this->enemy_score/(this->score+this->enemy_score+1);
            this->score+=add_score/this->profile.ability;
            this->enemy_score=max(0,this->enemy_score-add_score/this->profile.ability);
            this->combo=2;
            this->enemy_situtation=0;
        }else{
            this->combo=0;
            this->enemy_situtation=3;   //敵方將行普通攻擊
        }
        break;
    default:    //填字
        //計算答對得分比
        int total_part_score=0, add_score=0;
        for(int i=0;i<dff.size();i++){
            total_part_score+=dff[i];
            if(is_correct[i])
                add_score+=dff[i];
        }
        //檢查轉換答對率是否大於0.5
        if(add_score/(total_part_score*this->profile.ability)>0.5){
            this->enemy_hp-=100*add_score/(total_part_score*this->profile.ability)*this->enemy_score/(this->score+this->enemy_score+1);
            this->score+=add_score/this->profile.ability;
            this->enemy_score=max(0,this->enemy_score-add_score/this->profile.ability);
            this->combo+=2;
            this->enemy_situtation=0;
        }else{
            this->enemy_situtation=3;
        }
        break;
    }
}
bool Game::react(){
    switch(this->enemy_situtation){
        case 0: 
            int r=randomNum(0,101);
            if(r<80)   //80%機率加護甲
                this->enemy_score*=randomNum(101,201)/100;
            else  //20%機率回血
                this->enemy_hp+=randomNum(5,26);
            r=randomNum(0,101);
            if(r<50){    //50%機率無法行動，玩家可攻擊
                this->combo>=3? this->situation=0 : this->situation=2;
                this->current_question=Question(this->vocabs_info,this->vocab_num,this->profile);
            }
            else{    //50%機率反擊玩家需防禦
                this->situation=1;
                this->current_special_question=SpecialQuestion(this->vocabs_info,this->vocab_num,this->profile);
            }
            break;
        case 1:
            //敵人準備反擊，強化
            this->enemy_score*=randomNum(101,201)/100;
            this->enemy_hp+=randomNum(5,26);
            this->situation=1;
            break;
        case 2:
            this->situation=3;
            break;
        default:
            this->situation=1;
            break;
    }
    return (this->situation==2||this->situation==3);
}
void Game::newProfile(string profile_name){
        Csv csv;
        csv.openFile(profile_path);
        int index=0;
        while(csv.readValueWithIndex(index,0)!=""){
            index++;
        }
        //寫入資料檔名稱、等級、能力值
        csv.writeValueWithIndex(index,0,profile_name);
        csv.writeValueWithIndex(index,1,"1");
        csv.writeValueWithIndex(index,2,0);
        //Done
        csv.closeFile();
}
void Game::useProfile(string profile_name){
    Csv csv;
    csv.openFile(profile_path);
    int index=0;
    while(csv.readValueWithIndex(index,0)!=""){
        if(csv.readValueWithIndex(index,0)==profile_name){
            this->profile.profile_name=profile_name;
            this->profile.level=stoi(csv.readValueWithIndex(index,1));
            this->profile.ability=stoi(csv.readValueWithIndex(index,2));
            break;
        }
    }
    csv.closeFile();
}
void Game::leaveGmae(){
    this->in_level=false;
}