#pragma once
#include <windows.h>
#include <vector>
#include "GameTypes.h"

#pragma region 游戏常量定义

// 窗口配置
#define WINDOW_TITLEBARHEIGHT	32
#define WINDOW_WIDTH			1524
#define WINDOW_HEIGHT			768

// 场景ID
#define STAGE_STARTMENU			0
#define STAGE_1					1

// 各类鱼的精灵帧尺寸配置
#define FISH_TYPE1_FRAME_WIDTH		64
#define FISH_TYPE1_FRAME_HEIGHT		64
#define FISH_TYPE2_FRAME_WIDTH		64
#define FISH_TYPE2_FRAME_HEIGHT		64
#define FISH_TYPE3_FRAME_WIDTH		180
#define FISH_TYPE3_FRAME_HEIGHT		105
#define UNIT_LAST_FRAME				11

// 单位阵营定义
#define UNIT_SIDE_FISH1			10000
#define UNIT_SIDE_FISH2			10001
#define UNIT_SIDE_FISH3			10002

// 单位类型定义
#define UNIT_FISH_TYPE1		1
#define UNIT_FISH_TYPE2		2
#define UNIT_FISH_TYPE3		3

// 单位状态定义
#define UNIT_STATE_HOLD			0
#define UNIT_STATE_WALK			1
#define UNIT_STATE_ATTACK		2

// 单位方向定义
#define UNIT_DIRECT_RIGHT		0
#define UNIT_DIRECT_LEFT		1

// 背景配置
#define BG_COLUMNS				16
#define BG_ROWS					12
#define BG_CELL_WIDTH			64
#define BG_CELL_HEIGHT			64

// 按钮配置
#define BUTTON_STARTGAME			1001
#define BUTTON_STARTGAME_WIDTH		212
#define BUTTON_STARTGAME_HEIGHT		76

// 计时器配置
#define TIMER_GAMETIMER				1
#define TIMER_GAMETIMER_ELAPSE		30

// 游戏参数
#define UNIT_SPEED					3.0

#pragma endregion

#pragma region 全局状态声明

// 全局资源
extern HBITMAP bmp_start_bckground;
extern HBITMAP bmp_game_bckground;
extern HBITMAP bmp_StartButton;
extern HBITMAP bmp_Unit_Fish1;
extern HBITMAP bmp_Unit_Fish2;
extern HBITMAP bmp_Unit_Fish3;
extern HBITMAP bmp_Title;
extern HBITMAP bmp_Start_Background;
extern HBITMAP bmp_Stage_Background;

// 全局游戏状态
extern Stage* currentStage;
extern std::vector<Unit*> units;
extern std::vector<Button*> buttons;

// 输入状态
extern int mouseX;
extern int mouseY;
extern bool mouseDown;
extern bool keyUpDown;
extern bool keyDownDown;
extern bool keyLeftDown;
extern bool keyRightDown;

// 游戏进度
extern int progressValue;

// 帧序列数据
extern int FRAMES_HOLD[];
extern int FRAMES_HOLD_COUNT;
extern int FRAMES_WALK[];
extern int FRAMES_WALK_COUNT;
extern int FRAMES_ATTACK[];
extern int FRAMES_ATTACK_COUNT;

#pragma endregion
