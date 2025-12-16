#include "GameCore.h"
#include "GameState.h"
#include "EntityManager.h"
#include "StageManager.h"
#include "Renderer.h"
#include "resource.h"
#include <cstdlib>

void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// 初始化 GDI+
	InitGDIPlus();

	// 加载原始 BMP 资源
	bmp_start_bckground = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_START_BG));
	bmp_game_bckground = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_STAGE_BG));
	bmp_StartButton = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_START));
	bmp_Unit_Fish1 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish1));
	bmp_Unit_Fish2 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish2));
	bmp_Unit_Fish3 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish3));

	// 转换为 GDI+ Bitmap
	gdip_Start_Background = InitBackGround(hWnd, bmp_start_bckground);
	gdip_Stage_Background = InitBackGround(hWnd, bmp_game_bckground);
	gdip_StartButton = HBITMAPToBitmap(bmp_StartButton);
	gdip_Unit_Fish1 = HBITMAPToBitmap(bmp_Unit_Fish1);
	gdip_Unit_Fish2 = HBITMAPToBitmap(bmp_Unit_Fish2);
	gdip_Unit_Fish3 = HBITMAPToBitmap(bmp_Unit_Fish3);

	// 加载 PNG 按钮（从文件）
	gdip_NewGameButton = new Gdiplus::Bitmap(L"res/new_game.png");
	gdip_QuitGameButton = new Gdiplus::Bitmap(L"res/quit_game.png");

	// 加载 PNG 标题
	gdip_Title = new Gdiplus::Bitmap(L"res/title1.png");

	// 检查 PNG 是否加载成功
	if (gdip_NewGameButton && gdip_NewGameButton->GetLastStatus() != Gdiplus::Ok) {
		delete gdip_NewGameButton;
		gdip_NewGameButton = NULL;
	}
	if (gdip_QuitGameButton && gdip_QuitGameButton->GetLastStatus() != Gdiplus::Ok) {
		delete gdip_QuitGameButton;
		gdip_QuitGameButton = NULL;
	}
	if (gdip_Title && gdip_Title->GetLastStatus() != Gdiplus::Ok) {
		delete gdip_Title;
		gdip_Title = NULL;
	}

	// 添加新游戏按钮（使用 PNG 的原始尺寸）
	int newGameWidth = BUTTON_STARTGAME_WIDTH;
	int newGameHeight = BUTTON_STARTGAME_HEIGHT;
	if (gdip_NewGameButton) {
		newGameWidth = gdip_NewGameButton->GetWidth();
		newGameHeight = gdip_NewGameButton->GetHeight();
	}
	Button* newGameButton = CreateButton(BUTTON_STARTGAME, NULL, newGameWidth, newGameHeight,
		(WINDOW_WIDTH - newGameWidth) / 2, (WINDOW_HEIGHT - newGameHeight) / 2 + 50);
	buttons.push_back(newGameButton);

	// 添加退出按钮（使用 PNG 的原始尺寸）
	int quitGameWidth = BUTTON_QUITGAME_WIDTH;
	int quitGameHeight = BUTTON_QUITGAME_HEIGHT;
	if (gdip_QuitGameButton) {
		quitGameWidth = gdip_QuitGameButton->GetWidth();
		quitGameHeight = gdip_QuitGameButton->GetHeight();
	}
	Button* quitButton = CreateButton(BUTTON_QUITGAME, NULL, quitGameWidth, quitGameHeight,
		(WINDOW_WIDTH - quitGameWidth) / 2, (WINDOW_HEIGHT - quitGameHeight) / 2 + 200);
	buttons.push_back(quitButton);

	// 初始化开始场景
	InitStage(hWnd, STAGE_STARTMENU);

	// 初始化主计时器
	SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
}

void CleanupResources()
{
	// 清理 GDI+ Bitmap
	if (gdip_Start_Background) { delete gdip_Start_Background; gdip_Start_Background = NULL; }
	if (gdip_Stage_Background) { delete gdip_Stage_Background; gdip_Stage_Background = NULL; }
	if (gdip_StartButton) { delete gdip_StartButton; gdip_StartButton = NULL; }
	if (gdip_Unit_Fish1) { delete gdip_Unit_Fish1; gdip_Unit_Fish1 = NULL; }
	if (gdip_Unit_Fish2) { delete gdip_Unit_Fish2; gdip_Unit_Fish2 = NULL; }
	if (gdip_Unit_Fish3) { delete gdip_Unit_Fish3; gdip_Unit_Fish3 = NULL; }
	if (gdip_NewGameButton) { delete gdip_NewGameButton; gdip_NewGameButton = NULL; }
	if (gdip_QuitGameButton) { delete gdip_QuitGameButton; gdip_QuitGameButton = NULL; }
	if (gdip_Title) { delete gdip_Title; gdip_Title = NULL; }

	// 清理 HBITMAP
	if (bmp_start_bckground) { DeleteObject(bmp_start_bckground); bmp_start_bckground = NULL; }
	if (bmp_game_bckground) { DeleteObject(bmp_game_bckground); bmp_game_bckground = NULL; }
	if (bmp_StartButton) { DeleteObject(bmp_StartButton); bmp_StartButton = NULL; }
	if (bmp_Unit_Fish1) { DeleteObject(bmp_Unit_Fish1); bmp_Unit_Fish1 = NULL; }
	if (bmp_Unit_Fish2) { DeleteObject(bmp_Unit_Fish2); bmp_Unit_Fish2 = NULL; }
	if (bmp_Unit_Fish3) { DeleteObject(bmp_Unit_Fish3); bmp_Unit_Fish3 = NULL; }
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
