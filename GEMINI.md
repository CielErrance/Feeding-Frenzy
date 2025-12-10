# Gemini Context File (GEMINI.md)

本文件旨在帮助 Gemini 理解 `FishGame` 项目的上下文、架构和开发规范。

## 项目概述

这是一个基于 Windows 的 2D 游戏项目 ("FishGame")，使用 C++ 和原生 Win32 API 及 GDI 图形库开发。
项目是一个 Visual Studio 解决方案，核心逻辑位于 `src/FishGame` 目录下。

**技术栈**:
- **语言**: C++
- **框架**: Win32 API, GDI
- **IDE**: Visual Studio 2022 (.sln/.vcxproj)
- **图形**: GDI (BitBlt, TransparentBlt)

## 目录结构

- `src/FishGame/` : 源代码主目录
    - `FishGame.sln` : Visual Studio 解决方案文件
    - `FishGame.vcxproj` : 项目配置文件
    - `FishGame.cpp` : 游戏入口、消息循环、主逻辑、渲染函数
    - `FishGame.h` : 全局常量、宏定义、结构体 (`Unit`, `Stage`, `Button`)、函数声明
    - `resource.h` : 资源 ID 定义
    - `FishGame.rc` : 资源脚本（菜单、图标、位图链接）
    - `res/` : 游戏资源文件 (BMP 图片, ICO 图标)
- `bin/` : 编译输出目录 (包含 `FishGame.exe`)
- `doc/` : 项目文档 (设计文档, PPT, 视频等)

## 构建与运行

**构建方式**:
1.  **Visual Studio**: 打开 `src/FishGame/FishGame.sln` 并构建 (Debug/Release, x86/x64)。
2.  **MSBuild (命令行)**:
    ```cmd
    msbuild src/FishGame/FishGame.vcxproj /p:Configuration=Debug /p:Platform=x64
    ```

**运行**:
- 可执行文件通常生成在 `bin/FishGame.exe` 或 `src/FishGame/x64/Debug/FishGame.exe`。

## 核心架构与逻辑

### 1. 游戏循环
- 基于 Win32 消息循环 (`WndProc`)。
- 使用 `SetTimer` 设置定时器 (`TIMER_GAMETIMER`)，每 30ms 触发一次 `WM_TIMER` 消息。
- `TimerUpdate` 函数处理每一帧的逻辑更新。
- `Paint` 函数处理渲染，在 `WM_PAINT` 中调用。

### 2. 实体系统 (`Unit`)
- 所有游戏单位（如小鱼）由 `Unit` 结构体表示。
- 全局 `vector<Unit*> units` 存储所有活动单位。
- **行为控制**: `UpdateUnits` 函数遍历所有单位并调用相应的行为函数 (`UnitBehaviour_1`, `UnitBehaviour_2`)。
- **动画**: 基于帧索引 (`frame_id`, `frame_sequence`) 切换 BMP 图像的局部区域。

### 3. 渲染系统
- **双缓冲**: 使用内存 DC (`hMemDC`, `hBmpMem`) 避免闪烁。
- **流程**:
    1.  绘制背景 (`BitBlt`)。
    2.  绘制所有单位 (`TransparentBlt`，处理透明色)。
    3.  绘制 UI/按钮。
    4.  将内存 DC 复制到窗口 DC。

### 4. 输入处理
- **鼠标**: `WM_MOUSEMOVE` 更新全局 `mouseX`, `mouseY`。`WM_LBUTTONDOWN` 处理点击（如按钮交互）。
- **键盘**: `WM_KEYDOWN`/`WM_KEYUP` 更新全局布尔标志 (`keyUpDown`, `keyLeftDown` 等)。

## 常见开发任务指南

### 添加新单位类型
1.  **定义**: 在 `FishGame.h` 中添加 `UNIT_FISH_TYPEX` 宏。
2.  **资源**: 在 `InitGame` (`FishGame.cpp`) 中加载新的 `HBITMAP`。
3.  **行为**: 编写新的 `UnitBehaviour_X` 函数并在 `UpdateUnits` 中注册。
4.  **创建**: 更新 `CreateUnit` 以支持新类型。

### 修改游戏参数
- `FishGame.h` 中包含大部分可配置参数，如：
    - `WINDOW_WIDTH`, `WINDOW_HEIGHT` (窗口大小)
    - `UNIT_SPEED` (移动速度)
    - `TIMER_GAMETIMER_ELAPSE` (帧率控制)

### 注意事项
- **资源管理**: GDI 对象 (HBITMAP, HDC) 需要手动管理，注意内存泄漏。
- **坐标系**: 原点在左上角 (0,0)。
- **遗留代码**: 注意部分注释或变量名可能包含 "Contra" 字样，这是项目模板的遗留痕迹，请忽略或按需修正。
