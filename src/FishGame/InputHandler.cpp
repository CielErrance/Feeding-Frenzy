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
	case VK_ESCAPE:
		// 处理 ESC 键
		if (currentStage) {
			if (currentStage->stageID == STAGE_1) {
				// 在游戏中按ESC，切换暂停状态
				gamePaused = !gamePaused;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if (currentStage->stageID == STAGE_LEVELSELECT) {
				// 在关卡选择界面按ESC，回到标题界面
				InitStage(hWnd, STAGE_STARTMENU);
			}
		}
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
		
		// 特殊处理：暂停菜单按钮在暂停时直接可点击，不检查 visible
		bool canClick = false;
		if (gamePaused && (button->buttonID == BUTTON_RESUME || 
		     button->buttonID == BUTTON_RESTART || 
		          button->buttonID == BUTTON_RETURN)) {
			canClick = true;  // 暂停时暂停菜单按钮可点击
		}
		else if (button->visible && !button->locked) {
			canClick = true;  // 普通按钮需要 visible 且未锁定
		}
		
		if (canClick) {
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
				case BUTTON_RESUME:
					// 恢复游戏
					gamePaused = false;
					InvalidateRect(hWnd, NULL, FALSE);
					break;
				case BUTTON_RESTART:
					// 重新开始当前关卡
					gamePaused = false;
					InitStage(hWnd, currentStage->stageID);
					break;
				case BUTTON_RETURN:
					// 回到标题界面
					gamePaused = false;
					InitStage(hWnd, STAGE_STARTMENU);
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
