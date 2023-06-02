#include <algorithm>
#include<cmath>
#include<ctime>
#include<iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include <utility>
#include <vector>

#include"vocfile.hpp"

using namespace std;

const int frame_duration=33;    //每秒30幀
const string vocab_file_path="vocab.txt";

bool in_range(int input,int min,int max){
    return input>=min&&input<max;
}

int randomNumber(int min,int max) {
    // 設置種子
    std::srand(std::time(0));
    // 生成隨機數
    int randomNum=std::rand()%(max-min+1)+min;
    return randomNum;
}

int main(int argc, char *argv[] ){
    //執行flag
    bool quitting_game=false;  //程式準備關閉
    bool is_in_level=false;  //是否在關卡中
    bool leaving_to_menu=false; //準備回到主選單(玩家手動)
    bool level_ending=false;    //準備退出關卡(自動判定)
    bool ending_round=false;  //是否為回合末結算
    //遊戲資料
    vector<vector<string>>vocab_info; //字典
    int difficulty=-1;  //題目整體難度(5個選項難度的平均)
    int selected_option=-1; //玩家選的選項
    string prompt="";   //提示文字
    int correct_answer=-1;  //五選一(0~4)的正解
    string problem="";  //題目
    vector<string>choices;  //五個選項
    //敵我的生命值與分數
    int player_hp=-1;
    int enemy_hp=-1;
    int player_score=-1;
    int enemy_score=-1;
    //玩家資料
    int ability=-1;
    int level=-1;
    //敵人資料(用來決定敵人頭貼)
    int enemy_level=0;
    //答題計時器、回合間轉場效果計時器和提示文字計時器(單位:幀)
    int ans_counter=-1;
    int effect_counter=-1;
    int prompt_counter=-1;  //註:設為負值代表永久存在直到狀態改變    
    //SDL初始化
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Vocab Master", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window, SDL_FALSE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/Arial.ttf", 12);
    //讀取單字資料以及玩家資料
    VocFile vf;
    vf.openFile(vocab_file_path);
    int line_num=stoi(vf.readValue(0,0));
    ability=stoi(vf.readValue(0,1));
    level=max(0,min(12,(int)pow(ability,0.2)));
    for(int i=0;i<line_num;i++){
        vector<string>tmp;
        for(int j=0;j<3;j++){
            tmp.push_back(vf.readValue(i,j));
        }
        vocab_info.push_back(tmp);
    }
    vf.closeFile();
    //主循環
    while(true){
        //幀初時間
        int frame_start=SDL_GetTicks();
        //處理滑鼠事件
        SDL_Event event;
        int mouse_x,mouse_y;
        if(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                quitting_game=true;
            }else if(event.type==SDL_MOUSEBUTTONDOWN){
                SDL_MouseButtonEvent mouseEvent = event.button;
                mouse_x = mouseEvent.x;
                mouse_y = mouseEvent.y;
            }
        }
        
        if(!is_in_level){
            if(!quitting_game){
                //退出遊戲(右下一)
                if(in_range(mouse_x,1160,1280)&&in_range(mouse_y,700,720)){
                    quitting_game=true;
                    prompt="確定退出遊戲?";
                }
                //進入關卡(右下二)
                if(in_range(mouse_x,1160,1280)&&in_range(mouse_y,680,700)){
                    is_in_level=true;
                    //初始化關卡內資料和第一題
                    player_hp=100;
                    enemy_hp=100;
                    player_score=0;
                    enemy_score=0;
                    enemy_level=sqrt(randomNumber(0,13)*level);
                    //新題目
                    correct_answer=randomNumber(0,5);
                    //防重複選取機制
                    vector<int>anti_repeatness_array;
                    //難度計算暫存值
                    int dff_tmp=0;
                    while(choices.size()<5){
                        int random_voc_index=randomNumber(0,vocab_info.size());
                        if(find(anti_repeatness_array.begin(), anti_repeatness_array.end(), random_voc_index) == anti_repeatness_array.end()){
                            choices.push_back(vocab_info[random_voc_index][0]);
                            //如果剛好在填入正確答案，順便讀取題目
                            if(choices.size()==correct_answer){
                                problem=vocab_info[random_voc_index][1];
                            }
                            dff_tmp+=stoi(vocab_info[random_voc_index][2]);
                        }else{
                            continue;
                        }
                        difficulty=dff_tmp/5;
                    }
            }else{
                //確認或取消
                if(in_range(mouse_x,550,600)&&in_range(mouse_y,360,380)){   //確認
                    SDL_Quit();
                    break;
                }else if(in_range(mouse_x,680,730)&&in_range(mouse_y,360,380)){ //取消
                    quitting_game=false;
                }
            }
        }else{
            if(!leaving_to_menu&&!level_ending){
                //選項
                if(in_range(mouse_x,240,1040)&&in_range(mouse_y,360,510)&&!ending_round){
                    selected_option=(mouse_y-360)/30;
                }
                //退出關卡
                if(in_range(mouse_x,1160,1280)&&in_range(mouse_y,680,700)){
                    leaving_to_menu=true;
                    prompt="確定退出關卡?";
                }
            }else if(leaving_to_menu){
                //確認或取消
                if(in_range(mouse_x,550,600)&&in_range(mouse_y,360,380)){   //確認
                    is_in_level=false;
                }else if(in_range(mouse_x,680,730)&&in_range(mouse_y,360,380)){ //取消
                    leaving_to_menu=false;
                }
            }else if(level_ending){
                if(in_range(mouse_x,610,670)&&in_range(mouse_y,360,380)){
                    is_in_level=false;
                }
            }
                
        }
        //資料更新
        if(is_in_level){
            if(ending_round){
                //對答案，調整生命值和分數(能力越大，加越少、扣越多)
                if(selected_option==correct_answer){
                    //傷害倍率
                    float enemy_damage_rate=player_score+enemy_score>0?player_score/(enemy_score+player_score):1;
                    enemy_hp=max(enemy_hp-10*difficulty/ability,0);
                    enemy_score=max(enemy_score-20*difficulty/ability,0);
                    player_score+=enemy_score-20*difficulty/ability;
                    prompt="回答正確，攻擊成功!!!";
                }else{
                    //傷害倍率
                    float player_damage_rate=player_score+enemy_score>0?enemy_score/(enemy_score+player_score):1;
                    player_hp=max(player_hp-10*ability/difficulty,0);
                    player_score=max(player_score-20*ability/difficulty,0);
                    enemy_score+=player_score-20*ability/difficulty;
                    prompt="回答錯誤，玩家受傷!";
                }
                //進入轉場動畫時間並顯示提示文字
                effect_counter=90;  //3秒鐘
                prompt_counter=90;  //也是3秒鐘
            }else if(ending_round&&effect_counter==0){  //轉場結束
                //應該再加上判斷，如果一方死則結算存檔退出
                if(player_hp==0){
                    prompt="失敗!";
                    ability=max(0,ability-enemy_score/20);
                    level=max(0,min(12,(int)pow(ability,0.2)));
                    //存檔
                    VocFile vf;
                    vf.openFile(vocab_file_path);
                    vf.writeValue(0,0,to_string(ability));
                    vf.closeFile();
                    level_ending=true;
                }else if(enemy_hp==0){
                    prompt="勝利!";
                    ability+=player_score/20;
                    level=max(0,min(12,(int)pow(ability,0.2)));
                    //存檔
                    VocFile vf;
                    vf.openFile(vocab_file_path);
                    vf.writeValue(0,0,to_string(ability));
                    vf.closeFile();
                    level_ending=true;
                }else{
                    ending_round=false;
                    ans_counter=300;    //10秒
                    prompt="";
                    selected_option=-1;
                    //清除舊題目
                    choices.clear();
                    //新題目
                    correct_answer=randomNumber(0,5);
                    //防重複選取機制
                    vector<int>anti_repeatness_array;
                    //難度計算暫存值
                    int dff_tmp=0;
                    while(choices.size()<5){
                        int random_voc_index=randomNumber(0,vocab_info.size());
                        if(find(anti_repeatness_array.begin(), anti_repeatness_array.end(), random_voc_index) == anti_repeatness_array.end()){
                            choices.push_back(vocab_info[random_voc_index][0]);
                            //如果剛好在填入正確答案，順便讀取題目
                            if(choices.size()==correct_answer){
                                problem=vocab_info[random_voc_index][1];
                            }
                            dff_tmp+=stoi(vocab_info[random_voc_index][2]);
                        }else{
                            continue;
                        }
                        difficulty=dff_tmp/5;
                }
                
                }
            }else{
                //答題計時歸零或玩家做出選擇
                if(ans_counter==0||selected_option!=-1){
                    ending_round=true;
                }
            }
        }
        //渲染
        SDL_Rect rect;  //目標矩形
        SDL_Surface* surface;
        SDL_Texture* texture;
        //玩家頭貼
        rect.x=0;
        rect.y=0;
        rect.h=120;
        rect.w=120;
        surface=IMG_Load(("resourec/player_portrait_"+to_string(level)+".jpg").c_str());
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        //玩家等級
        rect.x=120;
        rect.y=0;
        rect.h=60;
        rect.w=120;
        surface=TTF_RenderText_Solid(font, ("等級："+to_string(level)).c_str(), {0,0,0,255});
        texture=SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        //玩家能力值
        rect.y=60;
        surface=TTF_RenderText_Solid(font, ("能力值："+to_string(ability)).c_str(), {0,0,0,255});
        texture=SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        if(!is_in_level){
            //遊戲logo
            rect.x=320;
            rect.y=240;
            rect.w=640;
            rect.h=240;
            surface=IMG_Load("resource/logo.jpg");
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            //進入關卡
            rect.x=1160;
            rect.y=680;
            rect.w=120;
            rect.h=20;
            SDL_SetRenderDrawColor(renderer,192,192,192,255);
            SDL_RenderFillRect(renderer, &rect);
            surface=TTF_RenderText_Solid(font,"進入關卡",{0,0,0,255});
            texture=SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            //退出遊戲
            rect.x=1160;
            rect.y=700;
            rect.w=120;
            rect.h=20;
            SDL_SetRenderDrawColor(renderer,192,192,192,255);
            SDL_RenderFillRect(renderer, &rect);
            surface=TTF_RenderText_Solid(font,"退出遊戲",{0,0,0,255});
            texture=SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
        }else{
            //敵人頭貼
            rect.x=1160;
            rect.y=0;
            rect.w=120;
            rect.h=120;
            surface=IMG_Load(("resourec/player_portrait_"+to_string(enemy_level)+".jpg").c_str());
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            //玩家血條
            rect.x=0;
            rect.y=120;
            rect.w=player_hp;
            rect.h=15;
            SDL_SetRenderDrawColor(renderer,0,192,0,255);
            SDL_RenderFillRect(renderer, &rect);
            //敵方血條
            rect.x=1280-enemy_hp;
            rect.y=120;
            rect.w=enemy_hp;
            rect.h=15;
            SDL_SetRenderDrawColor(renderer,0,192,0,255);
            SDL_RenderFillRect(renderer, &rect);
            //分數對抗條
            if(player_score+enemy_score==0){    //全部灰色
                rect.x=340;
                rect.y=700;
                rect.h=20;
                rect.w=600;
                SDL_SetRenderDrawColor(renderer,64,64,64,255);
                SDL_RenderFillRect(renderer, &rect);
            }else{
                int edge_x=340+600*player_score/(player_score+enemy_score); //敵我交界
                //玩家部分(藍)
                rect.x=340;
                rect.y=700;
                rect.h=20;
                rect.w=edge_x-340;
                SDL_SetRenderDrawColor(renderer,0,0,192,255);
                SDL_RenderFillRect(renderer, &rect);
                //敵人部分(紅)
                rect.x=edge_x;
                rect.y=700;
                rect.h=20;
                rect.w=940-edge_x;
                SDL_SetRenderDrawColor(renderer,192,0,0,255);
                SDL_RenderFillRect(renderer, &rect);
            }
            //題目
            int text_width;
            int text_height;
            TTF_SizeText(font, problem.c_str(), &text_width, &text_height);
            rect.x=640-0.5*text_width;
            rect.y=240;
            rect.h=text_height;
            rect.w=text_width;
            surface=TTF_RenderText_Solid(font,problem.c_str(),{0,0,0,255});
            texture=SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            //選項
            for(int i=0;i<5;i++){
                rect.x=240;
                rect.y=360+30*i;
                rect.w=800;
                rect.h=30;
                SDL_SetRenderDrawColor(renderer,192,192,192,255);
                SDL_RenderFillRect(renderer, &rect);
                TTF_SizeText(font, choices[i].c_str(), &text_width, &text_height);
                rect.x=640-0.5*text_width;
                rect.y=375+30*i-text_height*0.5;
                rect.h=text_height;
                rect.w=text_width;
            }
            //退出關卡
            rect.x=1160;
            rect.y=680;
            rect.w=120;
            rect.h=20;
            SDL_SetRenderDrawColor(renderer,192,192,192,255);
            SDL_RenderFillRect(renderer, &rect);
            surface=TTF_RenderText_Solid(font,"退出關卡",{0,0,0,255});
            texture=SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            //提示文字
            if(prompt_counter>0){
                 TTF_SizeText(font, prompt.c_str(), &text_width, &text_height);
                rect.x=640-0.5*text_width;
                rect.y=60;
                rect.h=text_height;
                rect.w=text_width;
                surface=TTF_RenderText_Solid(font,prompt.c_str(),{255,0,0,255});
                texture=SDL_CreateTextureFromSurface(renderer, surface);
                SDL_RenderCopy(renderer, texture, NULL, &rect);
            }
            //計時器
            TTF_SizeText(font,("時間:"+to_string(ans_counter)).c_str(), &text_width, &text_height);
            rect.x=640-0.5*text_width;
            rect.y=120;
            rect.h=text_height;
            rect.w=text_width;
            surface=TTF_RenderText_Solid(font,prompt.c_str(),{255,0,0,255});
            texture=SDL_CreateTextureFromSurface(renderer, surface);
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            //正確答案標記
            if(ending_round&&effect_counter>0){
                rect.x=240;
                rect.y=360+20*correct_answer;
                rect.w=800;
                rect.h=30;
                SDL_SetRenderDrawColor(renderer,0,255,0,255);
                SDL_RenderDrawRect(renderer, &rect);
                if(selected_option!=correct_answer){    //玩家之錯誤答案標記
                    rect.x=240;
                    rect.y=360+20*selected_option;
                    rect.w=800;
                    rect.h=30;
                    SDL_SetRenderDrawColor(renderer,255,0,0,255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
            //確認與取消鍵
            if(leaving_to_menu||quitting_game){
                rect.x=550;
                rect.y=360;
                rect.h=20;
                rect.w=50;
                SDL_SetRenderDrawColor(renderer,0,255,0,255);
                SDL_RenderFillRect(renderer, &rect);
                rect.x=680;
                rect.y=360;
                rect.h=20;
                rect.w=50;
                SDL_SetRenderDrawColor(renderer,255,0,0,255);
                SDL_RenderFillRect(renderer, &rect);
            }
            //只有確認鍵
            if(level_ending){
                rect.x=610;
                rect.y=360;
                rect.h=20;
                rect.w=60;
                SDL_SetRenderDrawColor(renderer,0,255,0,255);
                SDL_RenderFillRect(renderer, &rect);
            }


            }
        }
        //更新螢幕畫面
        SDL_RenderPresent(renderer);
        //各計時器倒數，在關卡中且並非正在回到主頁面且並非正在離開遊戲
        bool should_count_down=is_in_level&&!leaving_to_menu&&!quitting_game;
        if(ans_counter>0&&should_count_down)
            ans_counter--;
        if(effect_counter>0&&should_count_down)
            effect_counter--;
        if(prompt_counter>0&&should_count_down)
            prompt_counter--;
        //鎖定更新率
        int frame_time=SDL_GetTicks()-frame_start;
        if(frame_time<frame_duration)
            SDL_Delay(frame_duration-frame_time);
    }
    //釋放SDL資源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}