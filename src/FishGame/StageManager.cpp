#include "StageManager.h"
#include "GameState.h"
#include "EntityManager.h"

void InitStage(HWND hWnd, int stageID)
{
	// 清理旧的游戏状态
	if (currentStage != NULL) delete currentStage;
	currentStage = new Stage();
	currentStage->stageID = stageID;

	// 清理所有旧的 units（无论切换到哪个场景）
	for (int i = 0; i < units.size(); i++) {
		delete units[i];
	}
	units.clear();

	// 重置进度值
	progressValue = 0;

	if (stageID == STAGE_STARTMENU) {
		currentStage->bg = gdip_Start_Background;  // 使用 GDI+ Bitmap
		currentStage->timeCountDown = 0;
		currentStage->timerOn = false;

		for (int i = 0; i < buttons.size(); i++) {
			Button* button = buttons[i];
			// 在开始菜单显示新游戏和退出按钮
			if (button->buttonID == BUTTON_STARTGAME || button->buttonID == BUTTON_QUITGAME) {
				button->visible = true;
			}
			else {
				button->visible = false;
			}
		}
	}
	else if (stageID == STAGE_LEVELSELECT) {
		// 新增：关卡选择场景
		currentStage->bg = gdip_Start_Background;  // 使用相同背景
		currentStage->timeCountDown = 0;
		currentStage->timerOn = false;

		for (int i = 0; i < buttons.size(); i++) {
			Button* button = buttons[i];
			// 显示关卡选择按钮
			if (button->buttonID == BUTTON_LEVEL1 || 
			    button->buttonID == BUTTON_LEVEL2 || 
			    button->buttonID == BUTTON_LEVEL3) {
				button->visible = true;
			}
			else {
				button->visible = false;
			}
		}
	}
	else if (stageID >= STAGE_1 && stageID <= STAGE_1) {
		currentStage->stageID = stageID;
		currentStage->bg = gdip_Stage_Background;  // 使用 GDI+ Bitmap
		currentStage->timeCountDown = 10000;
		currentStage->timerOn = true;

		for (int i = 0; i < buttons.size(); i++) {
			Button* button = buttons[i];
			button->visible = false;
		}

		switch (stageID) {
		case STAGE_1:
		{
			Unit* player = CreateUnit(UNIT_SIDE_FISH1, UNIT_FISH_TYPE1,
				WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 100);
			player->isPlayer = true;
			player->size = 1.5f;
			units.push_back(player);
			break;
		}
		default:
			break;
		}
	}

	InvalidateRect(hWnd, NULL, FALSE);
}
