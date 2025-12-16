#pragma once
#include <windows.h>

// GDI+ 支持
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// GDI+ 初始化和清理
void InitGDIPlus();
void ShutdownGDIPlus();

// 主绘制函数
void Paint(HWND hWnd);

// 背景初始化（从 HBITMAP 转换为 GDI+ Image）
Gdiplus::Bitmap* InitBackGround(HWND hWnd, HBITMAP bmp_src);

// GDI+ 绘制辅助函数
void DrawImage(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
      int x, int y, int width, int height);

void DrawImageRegion(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
         int destX, int destY, int destW, int destH,
         int srcX, int srcY, int srcW, int srcH);

// 新增：带颜色键透明的绘制函数
void DrawImageWithColorKey(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
             int x, int y, int width, int height,
               Gdiplus::Color transparentColor);

void DrawImageRegionWithColorKey(Gdiplus::Graphics& graphics, Gdiplus::Image* image,
       int destX, int destY, int destW, int destH,
            int srcX, int srcY, int srcW, int srcH,
        Gdiplus::Color transparentColor);

// HBITMAP 转 GDI+ Bitmap
Gdiplus::Bitmap* HBITMAPToBitmap(HBITMAP hBitmap);
