#include "InputHandler.h"
#include "GameState.h"
#include "StageManager.h"
#include <windowsx.h>

void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case VK_UP:
		keyUpDown = true;
		break;
	case VK_DOWN:
		keyDownDown = true;
		break;
	case VK_LEFT:
		keyLeftDown = true;
		break;
	case VK_RIGHT:
		keyRightDown = true;
		break;
	default:
		break;
	}
}

void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case VK_UP:
		keyUpDown = false;
		break;
	case VK_DOWN:
		keyDownDown = false;
		break;
	case VK_LEFT:
		keyLeftDown = false;
		break;
	case VK_RIGHT:
		keyRightDown = false;
		break;
	default:
		break;
	}
}

void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
}

void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
	mouseDown = true;
	
	for (int i = 0; i < buttons.size(); i++) {
		Button* button = buttons[i];
		if (button->visible && !button->locked) {  // 锁定的按钮不响应点击
			if (button->x <= mouseX
				&& button->x + button->width >= mouseX
				&& button->y <= mouseY
				&& button->y + button->height >= mouseY) {
				switch (button->buttonID) {
				case BUTTON_STARTGAME:
					// 点击新游戏，进入关卡选择
					InitStage(hWnd, STAGE_LEVELSELECT);
					break;
				case BUTTON_QUITGAME:
					// 退出游戏
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case BUTTON_LEVEL1:
					// 进入关卡1
					InitStage(hWnd, STAGE_1);
					break;
				case BUTTON_LEVEL2:
					// 进入关卡2（目前锁定）
					// InitStage(hWnd, STAGE_2);
					break;
				case BUTTON_LEVEL3:
					// 进入关卡3（目前锁定）
					// InitStage(hWnd, STAGE_3);
					break;
				}
			}
		}
	}
}

void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
	mouseDown = false;
}
