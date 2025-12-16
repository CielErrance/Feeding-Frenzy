#include "Renderer.h"
#include "GameState.h"
#include "GameTypes.h"

using namespace Gdiplus;

// GDI+ 全局令牌
static ULONG_PTR gdiplusToken = 0;

// 初始化 GDI+
void InitGDIPlus()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// 关闭 GDI+
void ShutdownGDIPlus()
{
	if (gdiplusToken != 0) {
		GdiplusShutdown(gdiplusToken);
		gdiplusToken = 0;
	}
}

// HBITMAP 转 GDI+ Bitmap
Gdiplus::Bitmap* HBITMAPToBitmap(HBITMAP hBitmap)
{
	if (!hBitmap) return NULL;
	
	// 获取位图信息
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	
	// 创建兼容DC
	HDC hdc = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
	
	// 创建 GDI+ Bitmap
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, PixelFormat24bppRGB);
	
	// 锁定位图数据
	BitmapData bitmapData;
	Rect rect(0, 0, bm.bmWidth, bm.bmHeight);
	bitmap->LockBits(&rect, ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);
	
	// 获取位图数据
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight; // 负数表示从上到下
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	
	GetDIBits(hdcMem, hBitmap, 0, bm.bmHeight, bitmapData.Scan0, &bmi, DIB_RGB_COLORS);
	
	// 解锁位图
	bitmap->UnlockBits(&bitmapData);
	
	// 清理
	SelectObject(hdcMem, hOldBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdc);
	
	return bitmap;
}

// 绘制图像（不透明）
void DrawImage(Gdiplus::Graphics& graphics, Gdiplus::Image* image, 
      int x, int y, int width, int height)
{
	if (!image) return;
	
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.DrawImage(image, x, y, width, height);
}

// 绘制图像区域（不透明）
void DrawImageRegion(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
         int destX, int destY, int destW, int destH,
      int srcX, int srcY, int srcW, int srcH)
{
	if (!image) return;
	
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	// 绘制指定区域
	graphics.DrawImage(image,
		Rect(destX, destY, destW, destH),
		srcX, srcY, srcW, srcH,
		UnitPixel);
}

// 新增：带颜色键透明的绘制函数（完整图像）
void DrawImageWithColorKey(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
      int x, int y, int width, int height,
      Gdiplus::Color transparentColor)
{
	if (!image) return;
	
	// 创建 ImageAttributes 设置颜色键
	ImageAttributes imageAttr;
	imageAttr.SetColorKey(transparentColor, transparentColor);
	
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	// 使用 ImageAttributes 绘制
	graphics.DrawImage(image,
		Rect(x, y, width, height),
		0, 0, image->GetWidth(), image->GetHeight(),
		UnitPixel, &imageAttr);
}

// 新增：带颜色键透明的区域绘制（用于精灵图）
void DrawImageRegionWithColorKey(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
       int destX, int destY, int destW, int destH,
            int srcX, int srcY, int srcW, int srcH,
    Gdiplus::Color transparentColor)
{
	if (!image) return;
	
	// 创建 ImageAttributes 设置颜色键
	ImageAttributes imageAttr;
	imageAttr.SetColorKey(transparentColor, transparentColor);
	
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	// 使用 ImageAttributes 绘制指定区域
	graphics.DrawImage(image,
		Rect(destX, destY, destW, destH),
		srcX, srcY, srcW, srcH,
		UnitPixel, &imageAttr);
}

// 主绘制函数
void Paint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc_window = BeginPaint(hWnd, &ps);
	
	// 创建内存位图用于双缓冲
	HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
	HBITMAP blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc_memBuffer, blankBmp);
	
	// 创建 GDI+ Graphics 对象
	Graphics graphics(hdc_memBuffer);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	
	// 定义透明色（白色）
	Color transparentWhite(255, 255, 255);
	
	// 根据场景绘制
	if (currentStage->stageID == STAGE_STARTMENU) {
		// 绘制开始菜单背景（不需要透明）
		if (gdip_Start_Background) {
			DrawImage(graphics, gdip_Start_Background, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		}
		
		// 绘制标题（PNG 自带 Alpha 通道）
		if (gdip_Title) {
			int titleWidth = gdip_Title->GetWidth();
			int titleHeight = gdip_Title->GetHeight();
			int titleX = (WINDOW_WIDTH - titleWidth) / 2;
			int titleY = 50;  // 距离顶部 50 像素
			
			DrawImage(graphics, gdip_Title, titleX, titleY, titleWidth, titleHeight);
		}
	}
	else if (currentStage->stageID >= STAGE_1 && currentStage->stageID <= STAGE_1) {
		// 绘制游戏背景（不需要透明）
		if (gdip_Stage_Background) {
			DrawImage(graphics, gdip_Stage_Background, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		}
		
		// 绘制所有单位（使用颜色键透明）
		for (int i = 0; i < units.size(); i++) {
			::Unit* unit = units[i];  // 使用全局命名空间的 Unit
			
			// 根据单位类型选择对应的 GDI+ Bitmap
			Gdiplus::Bitmap* unitImage = NULL;
			if (unit->side == UNIT_SIDE_FISH1 || unit->type == UNIT_FISH_TYPE1) {
				unitImage = gdip_Unit_Fish1;
			}
			else if (unit->side == UNIT_SIDE_FISH2 || unit->type == UNIT_FISH_TYPE2) {
				unitImage = gdip_Unit_Fish2;
			}
			else if (unit->side == UNIT_SIDE_FISH3 || unit->type == UNIT_FISH_TYPE3) {
				unitImage = gdip_Unit_Fish3;
			}
			
			if (unitImage) {
				int drawWidth = (int)(unit->frame_width * unit->size);
				int drawHeight = (int)(unit->frame_height * unit->size);
				
				// 使用颜色键透明绘制精灵图的指定帧
				DrawImageRegionWithColorKey(graphics, unitImage,
					unit->x - drawWidth / 2, unit->y - drawHeight / 2, drawWidth, drawHeight,
					unit->frame_width * unit->frame_column,
					unit->frame_height * unit->frame_row,
					unit->frame_width, unit->frame_height,
					transparentWhite);  // 白色透明
			}
		}
	}
	
	// 绘制按钮（PNG 自带 Alpha 通道，直接绘制）
	for (int i = 0; i < buttons.size(); i++) {
		Button* button = buttons[i];
		if (button->visible) {
			// 根据按钮ID选择对应的图像
			Gdiplus::Bitmap* buttonImage = NULL;
			
			switch (button->buttonID) {
			case BUTTON_STARTGAME:
				// 使用新的 PNG 按钮（new_game.png）
				buttonImage = gdip_NewGameButton;
				break;
			case BUTTON_QUITGAME:
				// 使用退出 PNG 按钮（quit_game.png）
				buttonImage = gdip_QuitGameButton;
				break;
			default:
				// 回退到旧的 BMP 按钮
				buttonImage = gdip_StartButton;
				break;
			}
			
			if (buttonImage) {
				// PNG 图片自带透明通道，直接绘制（不使用颜色键）
				DrawImage(graphics, buttonImage, 
					button->x, button->y, button->width, button->height);
			}
		}
	}
	
	// 绘制进度条（使用 GDI+ 图形）
	if (currentStage->stageID == STAGE_1) {
		int barWidth = 400;
		int barHeight = 20;
		int barX = (WINDOW_WIDTH - barWidth) / 2;
		int barY = WINDOW_HEIGHT - 120;
		
		// 绘制边框
		Pen borderPen(Color(255, 0, 0, 0), 2);
		graphics.DrawRectangle(&borderPen, barX, barY, barWidth, barHeight);
		
		// 绘制填充
		int filledWidth = (barWidth * progressValue) / 100;
		SolidBrush fillBrush(Color(255, 0, 200, 0));
		graphics.FillRectangle(&fillBrush, barX + 1, barY + 1, filledWidth - 2, barHeight - 2);
		
		// 绘制文字
		wchar_t text[32];
		wsprintf(text, L"当前进度:%d%%", progressValue);
		
		FontFamily fontFamily(L"微软雅黑");
		Font font(&fontFamily, 14, FontStyleRegular, UnitPixel);
		SolidBrush textBrush(Color(255, 255, 255, 255));
		PointF textPos((REAL)(barX + barWidth / 2 - 40), (REAL)(barY - 25));
		
		graphics.DrawString(text, -1, &font, textPos, &textBrush);
	}
	
	// 复制到窗口
	BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);
	
	// 清理
	SelectObject(hdc_memBuffer, hOldBmp);
	DeleteObject(blankBmp);
	DeleteDC(hdc_memBuffer);
	EndPaint(hWnd, &ps);
}

// 初始化背景（从 HBITMAP 转换）
Gdiplus::Bitmap* InitBackGround(HWND hWnd, HBITMAP bmp_src)
{
	if (!bmp_src) return NULL;
	
	// 直接转换为 GDI+ Bitmap
	Gdiplus::Bitmap* bitmap = HBITMAPToBitmap(bmp_src);
	
	return bitmap;
}
