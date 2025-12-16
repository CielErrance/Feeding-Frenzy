#include "GameState.h"

// 全局资源初始化
HBITMAP bmp_start_bckground = NULL;
HBITMAP bmp_game_bckground = NULL;
HBITMAP bmp_StartButton = NULL;
HBITMAP bmp_Unit_Fish1 = NULL;
HBITMAP bmp_Unit_Fish2 = NULL;
HBITMAP bmp_Unit_Fish3 = NULL;
HBITMAP bmp_Title = NULL;
HBITMAP bmp_Start_Background = NULL;
HBITMAP bmp_Stage_Background = NULL;

// 全局游戏状态初始化
Stage* currentStage = NULL;
std::vector<Unit*> units;
std::vector<Button*> buttons;

// 输入状态初始化
int mouseX = 0;
int mouseY = 0;
bool mouseDown = false;
bool keyUpDown = false;
bool keyDownDown = false;
bool keyLeftDown = false;
bool keyRightDown = false;

// 游戏进度初始化
int progressValue = 0;

// 帧序列定义
int FRAMES_HOLD[] = { 0 };
int FRAMES_HOLD_COUNT = 1;
int FRAMES_WALK[] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 };
int FRAMES_WALK_COUNT = 16;
int FRAMES_ATTACK[] = { 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
int FRAMES_ATTACK_COUNT = 20;
