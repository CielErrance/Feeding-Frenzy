#include "GameState.h"

// 全局资源初始化 - 原始 HBITMAP
HBITMAP bmp_start_bckground = NULL;
HBITMAP bmp_game_bckground = NULL;
HBITMAP bmp_StartButton = NULL;
HBITMAP bmp_Unit_Fish1 = NULL;
HBITMAP bmp_Unit_Fish2 = NULL;
HBITMAP bmp_Unit_Fish3 = NULL;
HBITMAP bmp_Title = NULL;

// 全局资源初始化 - GDI+ Bitmap
Gdiplus::Bitmap* gdip_Start_Background = NULL;
Gdiplus::Bitmap* gdip_Stage_Background = NULL;
Gdiplus::Bitmap* gdip_StartButton = NULL;
Gdiplus::Bitmap* gdip_Unit_Fish1 = NULL;
Gdiplus::Bitmap* gdip_Unit_Fish2 = NULL;
Gdiplus::Bitmap* gdip_Unit_Fish3 = NULL;

// PNG 按钮
Gdiplus::Bitmap* gdip_NewGameButton = NULL;
Gdiplus::Bitmap* gdip_QuitGameButton = NULL;

// PNG 标题
Gdiplus::Bitmap* gdip_Title = NULL;

// 关卡选择按钮
Gdiplus::Bitmap* gdip_Level1Button = NULL;
Gdiplus::Bitmap* gdip_Level2Button = NULL;
Gdiplus::Bitmap* gdip_Level3Button = NULL;

// 暂停菜单按钮
Gdiplus::Bitmap* gdip_ResumeButton = NULL;
Gdiplus::Bitmap* gdip_RestartButton = NULL;
Gdiplus::Bitmap* gdip_ReturnButton = NULL;

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

// 游戏暂停状态
bool gamePaused = false;

// 帧序列定义
int FRAMES_HOLD[] = { 0 };
int FRAMES_HOLD_COUNT = 1;
int FRAMES_WALK[] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 };
int FRAMES_WALK_COUNT = 16;
int FRAMES_ATTACK[] = { 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
int FRAMES_ATTACK_COUNT = 20;
