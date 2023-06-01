#include "core.hpp"
#include <SDL2/SDL.h>
#include<utility>
#include <vector>


int main(){
    bool is_running=true;
    bool end_of_round=false;
    bool win_closing=false;
    int level_leaving=0;    //0非正在離開關卡，1勝利，2失敗
    Game g;
    const int frame_duration=33;    //毫秒
    int frame_start;
    //初始化SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS); 
    SDL_Window* window = SDL_CreateWindow("Vocab Master", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
    SDL_SetWindowResizable(window, SDL_FALSE); // 鎖定視窗大小
    SDL_Event event;
    int click_x,click_y;
    //遊戲相關
    bool is_special_question;
    vector<vector<char>>player_answer;
    int player_choice;
    //計時器
    vector<int>counters(8,0);
    //遊戲物件的初始化
    g.static_object[0].rendered=true;   //遊戲標題(主頁面)
    g.static_object[1].rendered=true;   //玩家頭像
    g.static_object[2].rendered=false;  //敵人頭像
    g.static_object[3].rendered=true;   //玩家資訊欄
    g.static_object[4].rendered=false;  //敵人資訊欄
    g.static_object[5].rendered=false;  //飛來飛去的武器(一把劍)
    while(is_running){
        frame_start=SDL_GetTicks();
        int frame_time=SDL_GetTicks()-frame_start;
        //輸入接收與反應
            SDL_PollEvent(&event);
            switch(event.type){
                case SDL_QUIT:
                    win_closing=true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_MouseButtonEvent mouse_event = event.button;
                    click_x=mouse_event.x;
                    click_y=mouse_event.y;
                    break;
            }
        //輸入對應(退出關卡、退出遊戲、選項/字母選擇+網格選擇)
        if(g.in_level){
            if(click_x>1160&&click_x<1280&&click_y>680&&click_y<700){   //退出關卡
                level_leaving=2;    //手動退出算輸
                continue;
            }else if(!is_special_question){ //選項
                if(click_x>340&&click_x<940&&click_y>320&&click_y<400){
                    player_choice=(click_y-320)/20;   //計算選項(1~5)
                }
            }else{ 
                int x_size=g.current_special_question.ret_size().first;
                int y_size=g.current_special_question.ret_size().second;
                //選格子
                if(click_x>640-8*x_size&&click_x<640+8*x_size&&click_y>360-8*y_size&&click_y<360+8*y_size){    

                }else if(){ //選字母

                }
            }
        }
        if(click_x>1160&&click_x<1280&&click_y>700&&click_y<720){   //退出遊戲
            win_closing=true;
        }
        if(win_closing){
            if(click_x>650&&click_x<710&&click_y>350&&click_y<370){ //確認
                SDL_Quit();
                SDL_DestroyWindow(window);
                break;
            }else if(click_x>570&&click_x<630&&click_y>350&&click_y<370){   //取消
                win_closing=false;
            }
        }
        if(level_leaving){
            if(click_x<610&&click_x<670&&click_y>350&&click_y<370){ //確認
                g.leaveGmae();
                level_leaving=0;
            }
        }
        if(g.in_level){
           if(end_of_round){
            vector<int>dff;
            vector<bool>is_correct;
            if(is_special_question){
                dff.push_back(g.current_question.ret_dff());
                is_correct.push_back(player_choice==g.current_question.correctAns());
            }else{
                vector<vector<bool>>answer_situation;
                for(int i=0;i<g.current_special_question.giveCorrectAns().size();i++){
                    vector<bool>tmp;
                    for(int j=0;j<g.current_special_question.giveCorrectAns()[i].size();j++){
                        tmp.push_back(g.current_question.giveChoices()[i][j]==player_answer[i][j]);
                    }
                    answer_situation.push_back(tmp);
                }
                for(int i=0;i<g.current_special_question.ret_vocs_num();i++){
                    dff.push_back(g.current_special_question.ret_dff()[i]);

                }
            }
            g.calculate(dff,is_correct);
            is_special_question=g.react();
            end_of_round=false;
           }else{
                //回合終止判定(計時器)
                if(counters[0]==0){
                    end_of_round=true;
                    is_special_question? counters[0]=g.current_special_question.ret_vocs_num()*25*33+33 : counters[0]=363;  //重設作答時間
                }
           }
            //跳脫判斷(你死我亡)
            if(g.hp==0)
                level_leaving=2;
            else if(g.enemy_hp==0)
                level_leaving=1;
        }
        //渲染

        //更新計時器陣列
        for(int i=0;i<8;i++){
            if(counters[i]>0)
                counters[i]--;
        }
        //鎖定更新率
        if(frame_time<frame_duration)
            SDL_Delay(frame_duration-frame_time);
    }
    return 0;
}