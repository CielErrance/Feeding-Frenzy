#pragma once
#include <windows.h>

// GDI+ 前向声明
namespace Gdiplus {
	class Bitmap;
}

// 场景结构体
struct Stage
{
	int stageID;
	Gdiplus::Bitmap* bg;  // 使用 GDI+ Bitmap
	int timeCountDown;
	bool timerOn;
};

// 按钮结构体
struct Button
{
	int buttonID;
	bool visible;
	HBITMAP img;  // 保留 HBITMAP 用于兼容性
	int x;
	int y;
	int width;
	int height;
	bool locked;  // 新增：按钮是否被锁定（未解锁）
};

// 单位结构体
struct Unit
{
	HBITMAP img;  // 保留 HBITMAP 用于兼容性
	
	int frame_row;
	int frame_column;
	int frame_width;
	int frame_height;
	
	int* frame_sequence;
	int frame_count;
	int frame_id;
	
	int side;
	int type;
	int state;
	int direction;
	
	int x;
	int y;
	double vx;
	double vy;
	int health;
	
	float size;
	bool isPlayer;
	
	double wave_timer;
	int initial_y;
};
