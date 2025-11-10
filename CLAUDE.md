# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在此代码仓库中工作时提供指导。

## 项目概述

这是一个基于 Windows 的 2D 小鱼游戏，使用 C++ 和 Win32 API 及 GDI 图形开发。游戏具有基于精灵的动画系统，小鱼单位根据鼠标位置相互交互，并包含碰撞检测和进度条系统。

## 构建系统

**Visual Studio 项目**：项目使用 Visual Studio 2022（平台工具集 v143）和 Windows SDK 10.0。

**构建命令**：
- 在 Visual Studio 中打开 `src/FishGame/FishGame.vcxproj`
- 可用构建配置：Win32 和 x64 的 Debug/Release
- 编译后的可执行文件输出到 `bin/FishGame.exe`

**命令行构建**：
```bash
# 使用 MSBuild（从 Visual Studio 开发人员命令提示符）
msbuild src/FishGame/FishGame.vcxproj /p:Configuration=Release /p:Platform=x64
```

**关键依赖项**：
- `Msimg32.lib` - TransparentBlt 函数所需（精灵透明度）
- Windows SDK 用于 Win32 API 和 GDI

## 代码架构

### 核心组件

**主游戏循环** (FishGame.cpp:103-110)：
- 标准 Win32 消息循环，通过 WndProc 处理事件
- 定时器驱动的游戏更新，间隔 30ms（TIMER_GAMETIMER_ELAPSE）

**场景管理**：
- 游戏使用基于关卡的系统（`Stage` 结构体）
- `currentStage` 全局指针跟踪当前活动场景
- 关卡：STAGE_STARTMENU (0), STAGE_1 (1)
- 通过 `InitStage(hWnd, stageID)` 初始化关卡

**实体系统**：
- `Unit` 结构体：基于精灵的游戏实体（小鱼），带有帧动画
- `units` 向量：所有活动单位的全局容器
- 帧序列控制动画：FRAMES_HOLD、FRAMES_WALK、FRAMES_ATTACK
- 单位支持状态：HOLD（静止）、WALK（行走）、ATTACK（攻击）
- 单位支持方向：LEFT（向左）、RIGHT（向右）

**渲染管线** (FishGame.cpp:689-782)：
- 通过内存 DC 进行双缓冲渲染
- 首先绘制背景，然后通过 TransparentBlt 以 RGB(255,255,255) 透明色绘制单位
- 按钮最后渲染在顶层
- 在 STAGE_1 中进度条渲染在底部中央

### 关键子系统

**输入处理**：
- 通过 `mouseX`、`mouseY` 全局跟踪鼠标位置
- 通过 `keyUpDown`、`keyDownDown`、`keyLeftDown`、`keyRightDown` 跟踪方向键
- 按钮点击在 LButtonDown 中通过坐标碰撞检测处理

**单位行为函数**：
- `UnitBehaviour_1`：小鱼跟随鼠标，基于距离的状态转换（400px 行走阈值，200px 攻击阈值）
- `UnitBehaviour_2`：始终向鼠标方向行走，32px 攻击阈值
- 在 UpdateUnits 中根据单位类型选择行为

**碰撞系统** (FishGame.cpp:505-518)：
- 基于距离的小鱼间碰撞（< 50 像素）
- 碰撞增加 `progressValue`（0-100 范围）

**动画系统**：
- 精灵图表采用行/列索引
- 帧序列定义动画播放顺序
- 根据方向计算列位置以支持左右精灵

## 资源管理

**位图资源** (src/FishGame/res/)：
- 小鱼精灵图表：Fish1.bmp、Fish2.bmp（64x64 方块，11 帧）
- 背景：start_background.bmp、stage_background.bmp
- UI：START.bmp 按钮图形

**资源加载**：
- 在 InitGame 中通过 LoadBitmap 从资源 ID 加载位图
- 背景图像通过 InitBackGround 预处理以匹配窗口尺寸
- 资源在 FishGame.rc 和 resource.h 中定义

## 常量与配置

**窗口设置** (FishGame.h:30-32)：
- WINDOW_WIDTH：1024px
- WINDOW_HEIGHT：768px
- WINDOW_TITLEBARHEIGHT：32px（不可调整大小、不可最大化的窗口）

**单位设置** (FishGame.h:39-60)：
- UNIT_SIZE_X/Y：64px 精灵尺寸
- UNIT_LAST_FRAME：11 个动画帧
- UNIT_SPEED：每帧 3.0 像素

**背景网格** (FishGame.h:63-66)：
- BG_COLUMNS：16，BG_ROWS：12
- BG_CELL_WIDTH/HEIGHT：64px

## 添加新功能

**添加新的单位类型**：
1. 在 FishGame.h 中定义新的 UNIT_FISH_TYPEX 常量
2. 在 InitGame 中加载精灵位图
3. 更新 CreateUnit 以处理新类型
4. 创建 UnitBehaviour_X 函数
5. 在 UpdateUnits switch 语句中添加 case

**添加新的关卡**：
1. 在 FishGame.h 中定义 STAGE_X 常量
2. 将背景资源添加到 res/ 和 FishGame.rc
3. 使用关卡初始化逻辑更新 InitStage
4. 更新 Paint 函数的渲染条件

**添加新的按钮**：
1. 在 FishGame.h 中定义 BUTTON_X 常量及尺寸
2. 在 InitGame 中加载位图资源
3. 调用 CreateButton 并推入 buttons 向量
4. 在 LButtonDown switch 语句中添加按钮点击处理器
5. 在 InitStage 中通过 button->visible 控制可见性

## 常见模式

**内存管理**：
- 手动位图句柄管理（HBITMAP）
- GDI 操作后需要 DeleteObject 和 DeleteDC
- InitStage 中重新分配前删除 Stage 指针

**坐标系统**：
- 左上角为原点 (0,0)
- 单位位置表示精灵中心（注意渲染时的 0.5 * UNIT_SIZE 偏移）

**状态转换**：
- 存储 next_state，与当前 unit->state 比较
- 状态改变时将 frame_id 重置为 -1
- 为新动画更新 frame_sequence 和 frame_count

## 已知问题与 TODOs

- 代码中标记了多个用于添加游戏场景的 TODO（FishGame.cpp:449, 712）
- 游戏关卡选择逻辑的 TODO（FishGame.cpp:331）
- 某些地方的注释引用了 "ContraGame"（模板的遗留命名）
- 尽管跟踪了方向键状态，但没有实现键盘控制
