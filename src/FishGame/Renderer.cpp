#include "Renderer.h"
#include "GameState.h"
#pragma comment(lib, "Msimg32.lib")

void Paint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc_window = BeginPaint(hWnd, &ps);
	HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
	HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);
	
	HBITMAP blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_memBuffer, blankBmp);
	
	if (currentStage->stageID == STAGE_STARTMENU) {
		SelectObject(hdc_loadBmp, bmp_Start_Background);
		TransparentBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			hdc_loadBmp, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, RGB(255, 255, 255));
	}
	else if (currentStage->stageID >= STAGE_1 && currentStage->stageID <= STAGE_1) {
		SelectObject(hdc_loadBmp, bmp_Stage_Background);
		TransparentBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			hdc_loadBmp, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, RGB(255, 255, 255));
		
		for (int i = 0; i < units.size(); i++) {
			Unit* unit = units[i];
			SelectObject(hdc_loadBmp, unit->img);
			int drawWidth = (int)(unit->frame_width * unit->size);
			int drawHeight = (int)(unit->frame_height * unit->size);
			TransparentBlt(
				hdc_memBuffer, unit->x - 0.5 * drawWidth, unit->y - 0.5 * drawHeight,
				drawWidth, drawHeight,
				hdc_loadBmp, unit->frame_width * unit->frame_column, 
				unit->frame_height * unit->frame_row, unit->frame_width, unit->frame_height,
				RGB(255, 255, 255)
			);
		}
	}
	
	for (int i = 0; i < buttons.size(); i++) {
		Button* button = buttons[i];
		if (button->visible) {
			SelectObject(hdc_loadBmp, button->img);
			TransparentBlt(
				hdc_memBuffer, button->x, button->y,
				button->width, button->height,
				hdc_loadBmp, 0, 0, button->width, button->height,
				RGB(255, 255, 255)
			);
		}
	}
	
	if (currentStage->stageID == STAGE_1) {
		int barWidth = 400;
		int barHeight = 20;
		int barX = (WINDOW_WIDTH - barWidth) / 2;
		int barY = WINDOW_HEIGHT - 120;
		
		HBRUSH brushBorder = CreateSolidBrush(RGB(0, 0, 0));
		RECT rectBorder = { barX, barY, barX + barWidth, barY + barHeight };
		FrameRect(hdc_memBuffer, &rectBorder, brushBorder);
		DeleteObject(brushBorder);
		
		int filledWidth = (barWidth * progressValue) / 100;
		RECT filledRect = { barX + 1, barY + 1, barX + filledWidth - 1, barY + barHeight - 1 };
		HBRUSH brushFill = CreateSolidBrush(RGB(0, 200, 0));
		FillRect(hdc_memBuffer, &filledRect, brushFill);
		DeleteObject(brushFill);
		
		wchar_t text[32];
		wsprintf(text, L"当前进度:%d%%", progressValue);
		SetBkMode(hdc_memBuffer, TRANSPARENT);
		SetTextColor(hdc_memBuffer, RGB(255, 255, 255));
		TextOut(hdc_memBuffer, barX + barWidth / 2 - 15, barY - 20, text, wcslen(text));
	}
	
	BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);
	
	DeleteObject(blankBmp);
	DeleteDC(hdc_memBuffer);
	DeleteDC(hdc_loadBmp);
	EndPaint(hWnd, &ps);
}

HBITMAP InitBackGround(HWND hWnd, HBITMAP bmp_src)
{
	PAINTSTRUCT ps;
	HDC hdc_window = BeginPaint(hWnd, &ps);
	HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
	HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);
	
	HBITMAP bmp_output = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_memBuffer, bmp_output);
	SelectObject(hdc_loadBmp, bmp_src);
	
	BITMAP bm;
	GetObject(bmp_src, sizeof(BITMAP), &bm);
	
	StretchBlt(
		hdc_memBuffer,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		hdc_loadBmp,
		0, 0, bm.bmWidth, bm.bmHeight,
		SRCCOPY
	);
	
	BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);
	
	DeleteDC(hdc_memBuffer);
	DeleteDC(hdc_loadBmp);
	EndPaint(hWnd, &ps);
	
	return bmp_output;
}
