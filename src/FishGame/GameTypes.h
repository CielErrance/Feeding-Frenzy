#pragma once
#include <windows.h>

// 场景结构体
struct Stage
{
	int stageID;
	HBITMAP bg;
	int timeCountDown;
	bool timerOn;
};

// 按钮结构体
struct Button
{
	int buttonID;
	bool visible;
	HBITMAP img;
	int x;
	int y;
	int width;
	int height;
};

// 单位结构体
struct Unit
{
	HBITMAP img;
	
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
