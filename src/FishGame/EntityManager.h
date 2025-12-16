#pragma once
#include <windows.h>
#include "GameTypes.h"

// 创建函数
Button* CreateButton(int buttonID, HBITMAP img, int width, int height, int x, int y);
Unit* CreateUnit(int side, int type, int x, int y, int health);

// 更新函数
void UpdateUnits(HWND hWnd);

// 单位行为函数
void UnitBehaviour_Player(Unit* unit);
void UnitBehaviour_2(Unit* unit);
void UnitBehaviour_SwimAcross(Unit* unit);
