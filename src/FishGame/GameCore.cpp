#include "GameCore.h"
#include "GameState.h"
#include "EntityManager.h"
#include "StageManager.h"
#include "Renderer.h"
#include "resource.h"
#include <cstdlib>

void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// 加载图像资源
	bmp_start_bckground = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_START_BG));
	bmp_game_bckground = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_STAGE_BG));
	bmp_StartButton = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_START));
	bmp_Unit_Fish1 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish1));
	bmp_Unit_Fish2 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish2));
	bmp_Unit_Fish3 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish3));

	// 添加按钮
	Button* startButton = CreateButton(BUTTON_STARTGAME, bmp_StartButton, BUTTON_STARTGAME_WIDTH, BUTTON_STARTGAME_HEIGHT,
		(WINDOW_WIDTH - BUTTON_STARTGAME_WIDTH) / 2, (WINDOW_HEIGHT - BUTTON_STARTGAME_HEIGHT) / 2);
	buttons.push_back(startButton);

	// 初始化背景
	bmp_Start_Background = InitBackGround(hWnd, bmp_start_bckground);
	bmp_Stage_Background = InitBackGround(hWnd, bmp_game_bckground);

	// 初始化开始场景
	InitStage(hWnd, STAGE_STARTMENU);

	// 初始化主计时器
	SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
}

void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UpdateUnits(hWnd);

	if (currentStage != NULL && currentStage->stageID == STAGE_1) {
		if (rand() % 100 < 3) {
			int side = rand() % 2;
			int y = rand() % (WINDOW_HEIGHT - 100) + 50;
			int x, direction;
			double vx;

			if (side == 0) {
				x = -80;
				vx = 2.0 + (rand() % 20) / 10.0;
				direction = UNIT_DIRECT_RIGHT;
			}
			else {
				x = WINDOW_WIDTH + 80;
				vx = -(2.0 + (rand() % 20) / 10.0);
				direction = UNIT_DIRECT_LEFT;
			}

			Unit* fish = CreateUnit(UNIT_SIDE_FISH2, UNIT_FISH_TYPE2, x, y, 100);
			fish->vx = vx;
			fish->vy = 0;
			fish->direction = direction;
			fish->state = UNIT_STATE_WALK;
			fish->frame_sequence = FRAMES_WALK;
			fish->frame_count = FRAMES_WALK_COUNT;
			fish->initial_y = y;
			fish->wave_timer = (rand() % 314) / 100.0;
			fish->frame_column = fish->type + fish->direction * (UNIT_LAST_FRAME - 2 * fish->type);
			units.push_back(fish);
		}
	}

	InvalidateRect(hWnd, NULL, FALSE);
}
