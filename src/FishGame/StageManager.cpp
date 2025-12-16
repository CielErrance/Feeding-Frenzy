#include "StageManager.h"
#include "GameState.h"
#include "EntityManager.h"

void InitStage(HWND hWnd, int stageID)
{
	if (currentStage != NULL) delete currentStage;
	currentStage = new Stage();
	currentStage->stageID = stageID;

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
			Unit* player = CreateUnit(UNIT_SIDE_FISH3, UNIT_FISH_TYPE3,
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
