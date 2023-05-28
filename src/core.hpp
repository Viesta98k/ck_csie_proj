#include<string>
#include<vector>
#include<utility>

#ifndef CORE_HPP
#define CORE_HPP

using namespace std;

typedef string Vocab;

class Profile{
  public:
    string profile_name;  //玩家名稱(亦為資料檔名稱)
    int level;  //玩家等級(決定稱號)
    int ability;   //能力值
};
class Object{
  public:
    int X,Y;  //物件位置
    double rX,rY,scale; //物件轉角與縮放倍率
    string render_data_source;  //渲染資料來源
};
class Question{
  public:
    Question(vector<vector<string>>vocabs_info, int vocab_num, Profile usr_info);
    vector<Vocab>giveChoices();
    int correctAns();
    int ret_dff();
  private:
    vector<Vocab>choices; //五個選項
    vector<string>definitions;  //選項意思
    int ans;  //正確答案
    int dff;  //該題整體難度
};

class SpecialQuestion{  //就是填字遊戲
  public:
    SpecialQuestion(vector<vector<string>>vocabs_info, int vocab_num, Profile usr_info);
    vector<vector<char>> giveCorrectAns();
    vector<int> ret_dff();
  private:
    vector<Vocab>vocs;  //所有單字
    vector<string>definitions;  //所有單字的意思
    vector<int>dff; //各單字難度
    vector<bool>is_vertical;  //單字是垂直(上至下)或水平(左至右)排列
    vector<pair<int,int>>head_relative; //第一個字母格的相對位置
};


class Game{
  public:
    Game();
    void calculate(vector<int>dff, vector<bool>is_correct);  //計算操作效果
    void react();  //敵人的反應
    void update();  //玩家操作後更新物件和狀態
    void newProfile(string profile_name);  //新增資料檔
    void useProfile(string profile_name); //切換資料檔並更新數據
  private:
    vector<vector<string>>vocabs_info;  //單字表
    bool in_level;  //關卡或主選單
    int vocab_num;  //檔案裡的字彙數量
    int hp, enemy_hp; //敵我生命值
    int combo;  //連擊，可以造成至多十倍傷害
    int score, enemy_score; //分數，決定護甲值(傷害減免=我方分數/敵我分數和)
    int situation, enemy_situtation;  //敵我狀態
    Profile profile;  //目前的使用者資料檔
};

#endif