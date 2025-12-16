#include "StageManager.h"
#include "GameState.h"
#include "EntityManager.h"

void InitStage(HWND hWnd, int stageID)
{
	if (currentStage != NULL) delete currentStage;
	currentStage = new Stage();
	currentStage->stageID = stageID;

	if (stageID == STAGE_STARTMENU) {
		currentStage->bg = bmp_Start_Background;
		currentStage->timeCountDown = 0;
		currentStage->timerOn = false;

		for (int i = 0; i < buttons.size(); i++) {
			Button* button = buttons[i];
			if (button->buttonID == BUTTON_STARTGAME) {
				button->visible = true;
			}
			else {
				button->visible = false;
			}
		}
	}
	else if (stageID >= STAGE_1 && stageID <= STAGE_1) {
		currentStage->stageID = stageID;
		currentStage->bg = bmp_Stage_Background;
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
