#pragma once
#include <windows.h>

void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam);
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 清理所有资源
void CleanupResources();
