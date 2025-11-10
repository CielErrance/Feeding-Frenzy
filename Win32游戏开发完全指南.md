# Win32 游戏开发完全指南 - 大鱼吃小鱼项目

> 本指南面向没有 Win32 编程经验的初学者，将从零开始讲解如何开发这个大鱼吃小鱼游戏。

---

## 目录

1. [Win32 编程基础概念](#1-win32-编程基础概念)
2. [程序架构详解](#2-程序架构详解)
3. [关键系统深入理解](#3-关键系统深入理解)
4. [开发步骤详解](#4-开发步骤详解)
5. [实战：实现大鱼吃小鱼核心功能](#5-实战实现大鱼吃小鱼核心功能)
6. [常见问题与调试技巧](#6-常见问题与调试技巧)
7. [扩展功能建议](#7-扩展功能建议)

---

## 1. Win32 编程基础概念

### 1.1 什么是 Win32 程序？

Win32 是 Windows 操作系统提供的一套 API（应用程序编程接口）。传统的 C/C++ 程序是从 `main()` 函数开始执行的，而 Win32 程序则不同：

- **入口函数**：`wWinMain` 或 `WinMain`
- **核心机制**：消息循环（Message Loop）
- **事件驱动**：程序通过响应各种"消息"来运行

### 1.2 消息机制 - Win32 的核心

**什么是消息？**

在 Windows 中，所有的事件都被转换成"消息"（Message）：
- 鼠标点击 → `WM_LBUTTONDOWN` 消息
- 键盘按下 → `WM_KEYDOWN` 消息
- 窗口需要重绘 → `WM_PAINT` 消息
- 定时器触发 → `WM_TIMER` 消息

**消息循环的工作流程：**

```
开始
  ↓
创建窗口
  ↓
[消息循环] ←─────┐
  ↓              │
获取消息(GetMessage) │
  ↓              │
有消息？         │
  ├─ 是 → 分发消息 ─┘
  └─ 否 → 退出程序
```

**代码对应（FishGame.cpp:99-107）：**

```cpp
while (GetMessage(&msg, nullptr, 0, 0))  // 获取消息
{
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
        TranslateMessage(&msg);  // 翻译消息（例如把键盘扫描码转成字符）
        DispatchMessage(&msg);   // 分发消息到窗口处理函数
    }
}
```

### 1.3 窗口过程函数（Window Procedure）

所有消息最终会到达 `WndProc` 函数（FishGame.cpp:178-221）：

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:       // 窗口创建时
        InitGame(hWnd, wParam, lParam);
        break;
    case WM_KEYDOWN:      // 键盘按下
        KeyDown(hWnd, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:  // 鼠标左键按下
        LButtonDown(hWnd, wParam, lParam);
        break;
    case WM_TIMER:        // 定时器触发
        if (currentStage != NULL && currentStage->timerOn)
            TimerUpdate(hWnd, wParam, lParam);
        break;
    case WM_PAINT:        // 需要重绘
        Paint(hWnd);
        break;
    // ... 其他消息
    }
    return 0;
}
```

**理解要点：**
- `hWnd`：窗口句柄，代表这个窗口的唯一标识
- `message`：消息类型（如 `WM_PAINT`）
- `wParam`、`lParam`：消息的附加参数（不同消息含义不同）

### 1.4 GDI 绘图基础

**GDI（Graphics Device Interface）** 是 Windows 的图形设备接口。

**核心概念：**

1. **设备上下文（DC - Device Context）**
   - 可以理解为一块"画布"
   - 所有绘图操作都在 DC 上进行
   - 类型：`HDC`

2. **位图（Bitmap）**
   - 存储图像数据的对象
   - 类型：`HBITMAP`
   - 需要"选入"DC 才能显示

3. **双缓冲技术**
   - 为什么需要？防止闪烁
   - 原理：在内存中画完整画面，再一次性显示到窗口

**双缓冲绘图流程（FishGame.cpp:686-778）：**

```cpp
void Paint(HWND hWnd)
{
    HDC hdc_window = BeginPaint(hWnd, &ps);        // 1. 获取窗口DC
    HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);  // 2. 创建内存DC（画布）
    HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);    // 3. 创建加载位图用的DC

    // 4. 创建空白位图并选入内存DC
    HBITMAP blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hdc_memBuffer, blankBmp);

    // 5. 在内存DC上绘制背景
    SelectObject(hdc_loadBmp, bmp_Start_Background);
    BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);

    // 6. 在内存DC上绘制游戏对象（使用透明色）
    SelectObject(hdc_loadBmp, unit->img);
    TransparentBlt(hdc_memBuffer, x, y, width, height,
                   hdc_loadBmp, srcX, srcY, srcWidth, srcHeight,
                   RGB(255, 255, 255));  // 白色作为透明色

    // 7. 一次性将内存DC的内容复制到窗口DC
    BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);

    // 8. 释放资源（非常重要！）
    DeleteObject(blankBmp);
    DeleteDC(hdc_memBuffer);
    DeleteDC(hdc_loadBmp);

    EndPaint(hWnd, &ps);
}
```

**关键函数说明：**

| 函数 | 作用 |
|------|------|
| `BeginPaint` / `EndPaint` | 开始/结束绘图 |
| `CreateCompatibleDC` | 创建兼容的内存DC |
| `CreateCompatibleBitmap` | 创建兼容的位图 |
| `SelectObject` | 将对象（如位图）选入DC |
| `BitBlt` | 位图块传送（复制图像） |
| `TransparentBlt` | 透明位图传送（指定透明色） |
| `DeleteObject` / `DeleteDC` | 删除对象/DC，释放内存 |

---

## 2. 程序架构详解

### 2.1 程序启动流程

```
wWinMain (入口函数)
    ↓
MyRegisterClass (注册窗口类)
    ↓
InitInstance (创建窗口)
    ↓
显示窗口
    ↓
触发 WM_CREATE 消息
    ↓
InitGame (初始化游戏)
    ├─ 加载图像资源
    ├─ 创建按钮
    ├─ 初始化背景
    ├─ 初始化开始场景
    └─ 启动定时器
    ↓
进入消息循环
```

### 2.2 游戏循环机制

本项目的游戏循环基于 **Windows 定时器**：

```cpp
// InitGame 中启动定时器
SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
// 每 30 毫秒触发一次 WM_TIMER 消息

// WndProc 中处理定时器消息
case WM_TIMER:
    if (currentStage != NULL && currentStage->timerOn)
        TimerUpdate(hWnd, wParam, lParam);
    break;

// TimerUpdate 更新游戏状态
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    UpdateUnits(hWnd);           // 更新所有游戏对象
    InvalidateRect(hWnd, NULL, FALSE);  // 触发重绘
}
```

**游戏循环图示：**

```
[定时器 30ms] → WM_TIMER → TimerUpdate
                              ↓
                          UpdateUnits (更新逻辑)
                              ↓
                          InvalidateRect (标记需要重绘)
                              ↓
                          WM_PAINT → Paint (绘制画面)
                              ↓
                          [等待下一个 30ms] ───┘
```

### 2.3 关键数据结构

#### Stage（场景）

```cpp
struct Stage
{
    int stageID;        // 场景编号（STAGE_STARTMENU, STAGE_1 等）
    HBITMAP bg;         // 背景图片
    int timeCountDown;  // 游戏时间倒计时
    bool timerOn;       // 计时器是否运行（是否暂停）
};
```

**全局变量：**
```cpp
Stage* currentStage = NULL;  // 当前场景
```

#### Unit（游戏对象 - 鱼）

```cpp
struct Unit
{
    // 图像相关
    HBITMAP img;          // 图片资源
    int frame_row;        // 当前显示第几行（对应动作类型）
    int frame_column;     // 当前显示第几列（对应帧数）
    int* frame_sequence;  // 帧序列数组（动画播放顺序）
    int frame_count;      // 帧序列长度
    int frame_id;         // 当前播放到序列的第几个

    // 属性
    int side;             // 阵营（UNIT_SIDE_FISH1, UNIT_SIDE_FISH2）
    int type;             // 类型（UNIT_FISH_TYPE1, UNIT_FISH_TYPE2）
    int state;            // 状态（HOLD, WALK, ATTACK）
    int direction;        // 方向（LEFT, RIGHT）

    // 位置与运动
    int x, y;             // 当前坐标
    double vx, vy;        // 速度
    int health;           // 生命值
};
```

**全局变量：**
```cpp
vector<Unit*> units;  // 存储所有游戏对象
```

#### Button（按钮）

```cpp
struct Button
{
    int buttonID;       // 按钮编号
    bool visible;       // 是否可见
    HBITMAP img;        // 图片
    int x, y;           // 位置
    int width, height;  // 尺寸
};
```

### 2.4 文件组织结构

```
FishGame.h          - 头文件
    ├─ 引用其他头文件
    ├─ 宏定义（常量）
    ├─ 结构体声明
    └─ 函数声明

FishGame.cpp        - 实现文件
    ├─ 全局变量定义
    ├─ Win32 框架代码
    ├─ 事件处理函数
    ├─ 游戏逻辑函数
    └─ 绘图函数

resource.h          - 资源ID定义
FishGame.rc         - 资源脚本（位图、图标等）
res/                - 资源文件夹（BMP图片）
```

---

## 3. 关键系统深入理解

### 3.1 场景（关卡）系统

**设计理念：**

游戏被划分为多个独立的"场景"，每个场景有：
- 独立的背景图
- 独立的游戏对象
- 独立的逻辑规则

**当前实现的场景：**

1. **STAGE_STARTMENU (0)** - 开始菜单
   - 显示开始按钮
   - 定时器关闭
   - 无游戏对象

2. **STAGE_1 (1)** - 第一关
   - 游戏背景
   - 两条鱼（演示用）
   - 定时器开启

**场景切换流程（InitStage 函数）：**

```cpp
void InitStage(HWND hWnd, int stageID)
{
    // 1. 清理旧场景
    if (currentStage != NULL) delete currentStage;
    currentStage = new Stage();
    currentStage->stageID = stageID;

    // 2. 根据场景ID初始化
    if (stageID == STAGE_STARTMENU) {
        // 开始菜单设置
        currentStage->bg = bmp_Start_Background;
        currentStage->timerOn = false;  // 不运行游戏逻辑

        // 显示开始按钮
        for (int i = 0; i < buttons.size(); i++) {
            if (buttons[i]->buttonID == BUTTON_STARTGAME)
                buttons[i]->visible = true;
        }
    }
    else if (stageID == STAGE_1) {
        // 游戏场景设置
        currentStage->bg = bmp_Stage_Background;
        currentStage->timerOn = true;   // 启动游戏逻辑

        // 隐藏所有按钮
        for (int i = 0; i < buttons.size(); i++)
            buttons[i]->visible = false;

        // 创建游戏对象
        units.push_back(CreateUnit(UNIT_SIDE_FISH1, UNIT_FISH_TYPE1, 200, 200, 100));
        units.push_back(CreateUnit(UNIT_SIDE_FISH2, UNIT_FISH_TYPE2, 600, 200, 100));
    }

    // 3. 刷新显示
    InvalidateRect(hWnd, NULL, FALSE);
}
```

**如何添加新关卡？**

```cpp
// 1. 在 FishGame.h 中定义新场景ID
#define STAGE_2     2
#define STAGE_3     3

// 2. 准备背景图资源（添加到 .rc 文件）

// 3. 在 InitStage 中添加新场景的初始化代码
else if (stageID == STAGE_2) {
    currentStage->bg = bmp_Stage2_Background;
    currentStage->timerOn = true;

    // 创建更多鱼，设置不同的初始位置和类型
    for (int i = 0; i < 5; i++) {
        int x = rand() % WINDOW_WIDTH;
        int y = rand() % WINDOW_HEIGHT;
        units.push_back(CreateUnit(UNIT_SIDE_FISH1, UNIT_FISH_TYPE1, x, y, 50));
    }
}
```

### 3.2 动画系统详解

**位图布局（Fish1.bmp / Fish2.bmp）：**

```
列 →  0    1    2    3    4    5    6    7    8    9   10
行 ↓ ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
0    │    │    │    │    │    │    │    │    │    │    │    │  ← 第0行（预留）
     ├────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
1    │静止│行走│行走│攻击│攻击│攻击│攻击│攻击│攻击│行走│静止│  ← 朝右的动画
     ├────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
2    │静止│行走│行走│攻击│攻击│攻击│攻击│攻击│攻击│行走│静止│  ← 朝左的动画（镜像）
     └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
```

**帧序列定义（FishGame.cpp:42-47）：**

```cpp
// 静止动画：只显示第0列
int FRAMES_HOLD[] = { 0 };
int FRAMES_HOLD_COUNT = 1;

// 行走动画：第1列和第2列交替显示（每列持续8帧）
int FRAMES_WALK[] = { 1, 1, 1, 1, 1, 1, 1, 1,
                      2, 2, 2, 2, 2, 2, 2, 2 };
int FRAMES_WALK_COUNT = 16;

// 攻击动画：第3列→第4列→第5列（不同持续时间）
int FRAMES_ATTACK[] = { 3, 3, 3, 3, 3, 3, 3, 3,
                        4, 4, 4, 4,
                        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
int FRAMES_ATTACK_COUNT = 24;
```

**动画播放机制：**

```cpp
// 每次 UpdateUnits 调用时
unit->frame_id++;                         // 帧ID增加
unit->frame_id = unit->frame_id % unit->frame_count;  // 循环播放

int column = unit->frame_sequence[unit->frame_id];  // 查表获取列号

// 根据方向计算实际列号（朝左时镜像）
unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);

// 绘制时使用 frame_row 和 frame_column 确定位图区域
```

**理解镜像公式：**

```
假设：
- column = 1 (行走第一帧)
- UNIT_LAST_FRAME = 11

朝右时 (direction = 0):
  frame_column = 1 + 0 * (11 - 2*1) = 1

朝左时 (direction = 1):
  frame_column = 1 + 1 * (11 - 2*1) = 1 + 9 = 10
  （对称位置）
```

### 3.3 状态机系统

每条鱼通过 **状态机（State Machine）** 控制行为：

```
       +-------+
       | HOLD  |  静止
       +-------+
       ↓     ↑
    条件1   条件3
       ↓     ↑
       +-------+
       | WALK  |  行走
       +-------+
       ↓     ↑
    条件2   条件4
       ↓     ↑
       +-------+
       |ATTACK |  攻击
       +-------+
```

**Fish1 的状态机逻辑（UnitBehaviour_1）：**

```cpp
// 计算与鼠标的距离
double dirLen = sqrt(dx*dx + dy*dy);

// 根据当前状态判断下一状态
int next_state = unit->state;

switch (unit->state) {
case UNIT_STATE_HOLD:
    if (dirLen < 400) {
        next_state = UNIT_STATE_WALK;  // 鼠标靠近 → 开始行走
    }
    break;

case UNIT_STATE_WALK:
    if (dirLen >= 400) {
        next_state = UNIT_STATE_HOLD;  // 鼠标远离 → 停止
    }
    else if (dirLen < 200) {
        next_state = UNIT_STATE_ATTACK;  // 鼠标很近 → 攻击
    }
    else {
        // 保持行走，更新速度朝向鼠标
        unit->vx = dirX / dirLen * UNIT_SPEED;
        unit->vy = dirY / dirLen * UNIT_SPEED;
    }
    break;

case UNIT_STATE_ATTACK:
    if (dirLen >= 200) {
        next_state = UNIT_STATE_WALK;  // 鼠标远离 → 继续追
    }
    break;
}

// 状态切换时的处理
if (next_state != unit->state) {
    unit->state = next_state;
    unit->frame_id = -1;  // 重置动画

    // 根据新状态设置动画和速度
    switch (unit->state) {
    case UNIT_STATE_HOLD:
        unit->frame_sequence = FRAMES_HOLD;
        unit->frame_count = FRAMES_HOLD_COUNT;
        unit->vx = 0;
        unit->vy = 0;
        break;
    // ... 其他状态
    }
}
```

### 3.4 坐标系统与碰撞检测

**坐标系：**

```
(0, 0) ────────────────────→ X
  │
  │         窗口 (1024 x 768)
  │
  │       Unit (x, y)
  │          ●
  │      ┌───┴───┐
  │      │ 64x64 │
  │      └───────┘
  │
  ↓ Y
```

**注意：**
- `unit->x`, `unit->y` 表示鱼的 **中心位置**
- 绘制时需要减去半个宽高：
  ```cpp
  TransparentBlt(hdc,
      unit->x - 0.5 * UNIT_SIZE_X,  // 左上角 X
      unit->y - 0.5 * UNIT_SIZE_Y,  // 左上角 Y
      UNIT_SIZE_X, UNIT_SIZE_Y, ...);
  ```

**碰撞检测（简单圆形碰撞）：**

```cpp
// 计算两条鱼的距离
double dx = fish1->x - fish2->x;
double dy = fish1->y - fish2->y;
double dist = sqrt(dx * dx + dy * dy);

// 如果距离小于阈值，判定为碰撞
if (dist < 50) {
    // 处理碰撞逻辑
}
```

---

## 4. 开发步骤详解

### 4.1 环境配置与运行

**步骤1：确认开发环境**

- 安装 Visual Studio 2022
- 确保安装了 C++ 桌面开发工作负载

**步骤2：打开项目**

```
打开 Visual Studio
→ 文件 → 打开 → 项目/解决方案
→ 选择 src/FishGame/FishGame.vcxproj
```

**步骤3：选择配置**

- 顶部工具栏选择 `Debug` 和 `x64`
- Debug：便于调试，编译速度快
- x64：64位程序

**步骤4：编译运行**

- 按 `F5`：编译并调试运行
- 按 `Ctrl+F5`：编译并直接运行（不附加调试器）
- 按 `Ctrl+Shift+B`：仅编译，不运行

**常见编译错误：**

| 错误 | 解决方法 |
|------|----------|
| 找不到 `windows.h` | 确认安装了 Windows SDK |
| 链接错误 `Msimg32.lib` | 检查 FishGame.h 第17行是否有 `#pragma comment(lib, "Msimg32.lib")` |
| 字符集错误 | 项目属性 → 字符集 → Unicode |

### 4.2 理解现有代码

**任务1：观察两条鱼的行为**

运行程序后：
1. 点击"开始"按钮
2. 移动鼠标，观察两条鱼的反应
3. 尝试理解 `UnitBehaviour_1` 和 `UnitBehaviour_2` 的区别

**任务2：使用调试器**

1. 在 `UpdateUnits` 函数设置断点（点击行号左侧）
2. 按 `F5` 运行
3. 程序会在断点处暂停
4. 按 `F10` 单步执行，观察变量变化
5. 鼠标悬停在变量上查看值

**任务3：修改参数体验**

尝试修改这些值，看看效果：
```cpp
// FishGame.h
#define UNIT_SPEED  3.0   // 改成 5.0 会更快

// FishGame.cpp
if (dirLen < 400) {  // 改成 200，鱼的反应范围变小
```

### 4.3 添加第一个功能：边界限制

**问题：** 目前鱼可以游出窗口

**解决方案：** 在 `UpdateUnits` 中添加边界检查

**步骤：**

1. **打开 FishGame.cpp**

2. **找到 `UpdateUnits` 函数**（约486行）

3. **在更新完所有 Unit 后，添加边界检查代码：**

```cpp
void UpdateUnits(HWND hWnd)
{
    for (int i = 0; i < units.size(); i++) {
        Unit* unit = units[i];

        // 根据单位类型选择行为函数
        switch (unit->type) {
        case UNIT_FISH_TYPE1:
            UnitBehaviour_1(unit);
            break;
        case UNIT_FISH_TYPE2:
            UnitBehaviour_2(unit);
            break;
        }

        // ====== 新增：边界限制 ======
        int half_width = UNIT_SIZE_X / 2;
        int half_height = UNIT_SIZE_Y / 2;

        // 左边界
        if (unit->x < half_width) {
            unit->x = half_width;
            unit->vx = 0;  // 停止水平移动
        }
        // 右边界
        if (unit->x > WINDOW_WIDTH - half_width) {
            unit->x = WINDOW_WIDTH - half_width;
            unit->vx = 0;
        }
        // 上边界
        if (unit->y < half_height) {
            unit->y = half_height;
            unit->vy = 0;  // 停止垂直移动
        }
        // 下边界
        if (unit->y > WINDOW_HEIGHT - half_height) {
            unit->y = WINDOW_HEIGHT - half_height;
            unit->vy = 0;
        }
        // ====== 边界限制结束 ======
    }

    // ... 原有的碰撞检测代码
}
```

4. **编译并测试** (`F5`)

### 4.4 添加第二个功能：显示鱼的数量

**目标：** 在屏幕上显示当前有多少条鱼

**步骤：**

1. **打开 FishGame.cpp**

2. **找到 `Paint` 函数**（约686行）

3. **在进度条绘制代码后，添加文字显示：**

```cpp
// 在 Paint 函数中，进度条代码（746-766行）之后添加：

// ====== 新增：显示鱼的数量 ======
if (currentStage->stageID == STAGE_1) {
    wchar_t fishCountText[64];
    wsprintf(fishCountText, L"当前鱼的数量: %d", units.size());

    SetBkMode(hdc_memBuffer, TRANSPARENT);  // 透明背景
    SetTextColor(hdc_memBuffer, RGB(255, 255, 0));  // 黄色文字

    TextOut(hdc_memBuffer, 20, 20, fishCountText, wcslen(fishCountText));
}
// ====== 显示数量结束 ======
```

4. **编译测试**

**扩展：** 你还可以显示鼠标坐标：
```cpp
wchar_t mouseText[64];
wsprintf(mouseText, L"鼠标位置: (%d, %d)", mouseX, mouseY);
TextOut(hdc_memBuffer, 20, 50, mouseText, wcslen(mouseText));
```

---

## 5. 实战：实现大鱼吃小鱼核心功能

### 5.1 需求分析

**游戏规则：**
1. 鱼有大小属性
2. 大鱼碰到小鱼，小鱼被吃掉
3. 大鱼吃掉小鱼后自己变大
4. 自动生成新的小鱼
5. 玩家控制一条鱼
6. 目标：成为最大的鱼

### 5.2 Step 1：为鱼添加大小属性

**修改 Unit 结构体：**

```cpp
// FishGame.h 约106行，在 Unit 结构体中添加：
struct Unit
{
    // ... 原有成员
    int health;           // 生命值

    // ====== 新增 ======
    float size;           // 鱼的大小（1.0为正常大小）
    bool isPlayer;        // 是否是玩家控制的鱼
    // ==================
};
```

**修改 CreateUnit 函数：**

```cpp
// FishGame.cpp 约382行
Unit* CreateUnit(int side, int type, int x, int y, int health)
{
    Unit* unit = new Unit();
    // ... 原有代码

    unit->health = health;

    // ====== 新增 ======
    unit->size = 1.0f;    // 默认大小为1.0
    unit->isPlayer = false;  // 默认不是玩家
    // ==================

    return unit;
}
```

**修改初始化代码，创建玩家和AI鱼：**

```cpp
// FishGame.cpp 约468行，在 InitStage 的 STAGE_1 分支中修改：

case STAGE_1:
    // ====== 修改后的代码 ======
    // 创建玩家控制的鱼
    Unit* player = CreateUnit(UNIT_SIDE_FISH1, UNIT_FISH_TYPE1,
                              WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 100);
    player->isPlayer = true;
    player->size = 1.5f;  // 玩家初始稍大
    units.push_back(player);

    // 创建若干AI小鱼
    for (int i = 0; i < 5; i++) {
        int x = rand() % (WINDOW_WIDTH - 100) + 50;
        int y = rand() % (WINDOW_HEIGHT - 100) + 50;
        Unit* aiFish = CreateUnit(UNIT_SIDE_FISH2, UNIT_FISH_TYPE2, x, y, 50);
        aiFish->size = 0.5f + (rand() % 10) * 0.1f;  // 随机大小 0.5~1.4
        units.push_back(aiFish);
    }
    // ==========================
    break;
```

### 5.3 Step 2：根据大小绘制鱼

**修改 Paint 函数的绘制代码：**

```cpp
// FishGame.cpp 约714行，修改单位绘制部分：

// 绘制单位到缓存
for (int i = 0; i < units.size(); i++) {
    Unit* unit = units[i];
    SelectObject(hdc_loadBmp, unit->img);

    // ====== 修改：根据 size 计算实际绘制尺寸 ======
    int drawWidth = (int)(UNIT_SIZE_X * unit->size);
    int drawHeight = (int)(UNIT_SIZE_Y * unit->size);

    TransparentBlt(
        hdc_memBuffer,
        unit->x - drawWidth / 2,      // 根据大小调整位置
        unit->y - drawHeight / 2,
        drawWidth, drawHeight,         // 使用缩放后的尺寸
        hdc_loadBmp,
        UNIT_SIZE_X * unit->frame_column,
        UNIT_SIZE_Y * unit->frame_row,
        UNIT_SIZE_X, UNIT_SIZE_Y,     // 源图像尺寸不变
        RGB(255, 255, 255)
    );
    // ===============================================
}
```

### 5.4 Step 3：实现碰撞与吃鱼逻辑

**修改 UpdateUnits 函数的碰撞检测部分：**

```cpp
// FishGame.cpp 约502行，替换原有的碰撞检测代码：

void UpdateUnits(HWND hWnd)
{
    // ... 更新所有 unit 的代码

    // ====== 替换原有碰撞检测代码 ======

    // 检测所有鱼之间的碰撞
    for (int i = 0; i < units.size(); i++) {
        for (int j = i + 1; j < units.size(); j++) {
            Unit* fish1 = units[i];
            Unit* fish2 = units[j];

            // 计算距离
            double dx = fish1->x - fish2->x;
            double dy = fish1->y - fish2->y;
            double dist = sqrt(dx * dx + dy * dy);

            // 碰撞半径（根据大小计算）
            double radius1 = UNIT_SIZE_X * fish1->size * 0.5;
            double radius2 = UNIT_SIZE_X * fish2->size * 0.5;

            // 判断是否碰撞
            if (dist < (radius1 + radius2) * 0.7) {  // 0.7 是调整系数
                // 判断谁更大
                Unit* bigFish = (fish1->size > fish2->size) ? fish1 : fish2;
                Unit* smallFish = (fish1->size > fish2->size) ? fish2 : fish1;

                // 大小差距要足够大才能吃（避免同大小互吃）
                if (bigFish->size > smallFish->size * 1.2) {
                    // 大鱼吃小鱼
                    bigFish->size += smallFish->size * 0.1f;  // 增长10%小鱼的大小

                    // 从数组中删除小鱼
                    auto it = std::find(units.begin(), units.end(), smallFish);
                    if (it != units.end()) {
                        delete *it;  // 释放内存
                        units.erase(it);
                    }

                    // 如果玩家被吃，游戏结束
                    if (smallFish->isPlayer) {
                        MessageBox(hWnd, L"你被吃掉了！游戏结束", L"Game Over", MB_OK);
                        InitStage(hWnd, STAGE_STARTMENU);  // 返回开始菜单
                        return;
                    }

                    // 重新开始循环（因为数组已改变）
                    i = -1;
                    break;
                }
            }
        }
    }

    // ================================
}
```

**注意事项：**
- 使用 `std::find` 需要在文件开头添加 `#include <algorithm>`（FishGame.h 第22行附近）
- 删除元素后要 `break` 跳出内层循环，并重置外层循环（`i = -1`）

### 5.5 Step 4：添加玩家控制

**创建新的 Behaviour 函数：**

```cpp
// 在 FishGame.h 约187行，添加函数声明：
void UnitBehaviour_Player(Unit* unit);

// 在 FishGame.cpp 约678行后，添加实现：
void UnitBehaviour_Player(Unit* unit) {
    // 根据键盘控制移动
    double vx = 0, vy = 0;

    if (keyUpDown) vy -= UNIT_SPEED;
    if (keyDownDown) vy += UNIT_SPEED;
    if (keyLeftDown) vx -= UNIT_SPEED;
    if (keyRightDown) vx += UNIT_SPEED;

    // 归一化速度（防止斜向移动过快）
    double speed = sqrt(vx * vx + vy * vy);
    if (speed > 0.01) {
        unit->vx = vx / speed * UNIT_SPEED;
        unit->vy = vy / speed * UNIT_SPEED;
    }
    else {
        unit->vx = 0;
        unit->vy = 0;
    }

    // 更新方向
    if (vx > 0) unit->direction = UNIT_DIRECT_RIGHT;
    else if (vx < 0) unit->direction = UNIT_DIRECT_LEFT;

    // 更新状态
    int next_state = (speed > 0.01) ? UNIT_STATE_WALK : UNIT_STATE_HOLD;

    if (next_state != unit->state) {
        unit->state = next_state;
        unit->frame_id = -1;

        if (next_state == UNIT_STATE_WALK) {
            unit->frame_sequence = FRAMES_WALK;
            unit->frame_count = FRAMES_WALK_COUNT;
        }
        else {
            unit->frame_sequence = FRAMES_HOLD;
            unit->frame_count = FRAMES_HOLD_COUNT;
        }
    }

    // 更新位置
    unit->x += unit->vx;
    unit->y += unit->vy;

    // 更新动画帧
    unit->frame_id++;
    unit->frame_id = unit->frame_id % unit->frame_count;
    int column = unit->frame_sequence[unit->frame_id];
    unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);
}
```

**修改 UpdateUnits 调用逻辑：**

```cpp
// FishGame.cpp 约489行，修改：
for (int i = 0; i < units.size(); i++) {
    Unit* unit = units[i];

    // ====== 修改：根据 isPlayer 选择行为 ======
    if (unit->isPlayer) {
        UnitBehaviour_Player(unit);
    }
    else {
        // AI 鱼的行为
        switch (unit->type) {
        case UNIT_FISH_TYPE1:
            UnitBehaviour_1(unit);
            break;
        case UNIT_FISH_TYPE2:
            UnitBehaviour_2(unit);
            break;
        }
    }
    // =========================================

    // ... 边界检查代码
}
```

### 5.6 Step 5：自动生成新鱼

**添加生成计数器：**

```cpp
// FishGame.cpp 约39行，全局变量区域添加：
int spawnCounter = 0;  // 生成计数器
```

**在 TimerUpdate 中添加生成逻辑：**

```cpp
// FishGame.cpp 约348行，修改 TimerUpdate：
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    UpdateUnits(hWnd);

    // ====== 新增：自动生成鱼 ======
    if (currentStage->stageID == STAGE_1) {
        spawnCounter++;

        // 每3秒（100帧）生成一条新鱼
        if (spawnCounter >= 100) {
            spawnCounter = 0;

            // 随机位置（边缘生成）
            int x, y;
            if (rand() % 2 == 0) {  // 从左右边缘生成
                x = (rand() % 2) * (WINDOW_WIDTH - 100) + 50;
                y = rand() % WINDOW_HEIGHT;
            }
            else {  // 从上下边缘生成
                x = rand() % WINDOW_WIDTH;
                y = (rand() % 2) * (WINDOW_HEIGHT - 100) + 50;
            }

            Unit* newFish = CreateUnit(UNIT_SIDE_FISH2, UNIT_FISH_TYPE2, x, y, 50);
            newFish->size = 0.5f + (rand() % 10) * 0.1f;
            units.push_back(newFish);
        }
    }
    // =============================

    InvalidateRect(hWnd, NULL, FALSE);
}
```

### 5.7 Step 6：优化AI行为（追逐玩家）

**创建新的AI行为：**

```cpp
// 在 FishGame.h 约188行添加声明：
void UnitBehaviour_ChasePlayer(Unit* unit);

// 在 FishGame.cpp 约678行后添加实现：
void UnitBehaviour_ChasePlayer(Unit* unit) {
    // 找到玩家
    Unit* player = NULL;
    for (int i = 0; i < units.size(); i++) {
        if (units[i]->isPlayer) {
            player = units[i];
            break;
        }
    }

    if (player == NULL) return;  // 玩家不存在

    // 计算与玩家的距离和方向
    double dirX = player->x - unit->x;
    double dirY = player->y - unit->y;
    double dirLen = sqrt(dirX * dirX + dirY * dirY) + 0.0001;

    // 判断大小关系，决定是追逐还是逃跑
    bool shouldChase = (unit->size > player->size * 1.2);

    if (!shouldChase) {
        // 逃跑：方向相反
        dirX = -dirX;
        dirY = -dirY;
    }

    // 更新方向
    if (dirX > 0) unit->direction = UNIT_DIRECT_RIGHT;
    else unit->direction = UNIT_DIRECT_LEFT;

    // 更新状态和速度
    int next_state = UNIT_STATE_WALK;

    // AI速度稍慢于玩家
    double aiSpeed = UNIT_SPEED * 0.8;
    unit->vx = (dirX / dirLen) * aiSpeed;
    unit->vy = (dirY / dirLen) * aiSpeed;

    // 状态切换
    if (next_state != unit->state) {
        unit->state = next_state;
        unit->frame_id = -1;
        unit->frame_sequence = FRAMES_WALK;
        unit->frame_count = FRAMES_WALK_COUNT;
    }

    // 更新位置
    unit->x += unit->vx;
    unit->y += unit->vy;

    // 更新动画
    unit->frame_id++;
    unit->frame_id = unit->frame_id % unit->frame_count;
    int column = unit->frame_sequence[unit->frame_id];
    unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);
}
```

**应用到 UpdateUnits：**

```cpp
// FishGame.cpp 约489行，修改 AI 鱼的行为选择：
else {
    // ====== 使用新的追逐行为 ======
    UnitBehaviour_ChasePlayer(unit);
    // ==============================
}
```

---

## 6. 常见问题与调试技巧

### 6.1 编译错误

**错误：`'vector': undeclared identifier`**

解决：确保在 FishGame.h 中包含：
```cpp
#include <vector>
```

**错误：`'sqrt': identifier not found`**

解决：确保包含：
```cpp
#include <math.h>
```

**错误：链接错误 LNK2019**

原因：函数声明了但没有实现

解决：检查 .h 文件中声明的函数是否都在 .cpp 中实现了

### 6.2 运行时错误

**程序崩溃：访问冲突**

常见原因：
1. 访问了已删除的对象
2. 指针未初始化

调试方法：
```cpp
// 删除对象后立即置空
delete unit;
unit = NULL;

// 使用前检查
if (unit != NULL && unit->img != NULL) {
    // 使用 unit
}
```

**图像不显示**

检查清单：
1. 资源是否正确加载（LoadBitmap 返回值不为 NULL）
2. 坐标是否在屏幕范围内
3. 透明色是否正确（RGB(255, 255, 255)）
4. 是否调用了 SelectObject

### 6.3 调试技巧

**1. 使用 OutputDebugString 输出日志**

```cpp
#include <sstream>

// 输出调试信息到 Visual Studio 的输出窗口
wchar_t debugMsg[256];
wsprintf(debugMsg, L"Fish position: (%d, %d), size: %.2f\n",
         unit->x, unit->y, unit->size);
OutputDebugString(debugMsg);
```

**2. 使用断点和监视窗口**

- 按 `F9` 在当前行设置断点
- 按 `F5` 运行到断点
- 按 `F10` 单步执行
- 右键变量 → 添加监视，持续观察值的变化

**3. 条件断点**

右键断点 → 条件 → 输入条件（如 `unit->size > 2.0`），只有满足条件时才中断

**4. 绘制调试信息**

```cpp
// 在 Paint 函数中绘制辅助线
if (currentStage->stageID == STAGE_1) {
    for (int i = 0; i < units.size(); i++) {
        Unit* unit = units[i];

        // 绘制碰撞半径
        int radius = (int)(UNIT_SIZE_X * unit->size * 0.5);
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc_memBuffer, pen);

        Ellipse(hdc_memBuffer,
                unit->x - radius, unit->y - radius,
                unit->x + radius, unit->y + radius);

        SelectObject(hdc_memBuffer, oldPen);
        DeleteObject(pen);
    }
}
```

### 6.4 性能优化

**问题：游戏卡顿**

1. **减少不必要的计算**
   ```cpp
   // 不好：每帧都计算平方根
   for (all units) {
       double dist = sqrt(...);
   }

   // 好：先比较平方，只在必要时计算平方根
   double distSq = dx*dx + dy*dy;
   if (distSq < threshold * threshold) {
       double dist = sqrt(distSq);
       // 处理碰撞
   }
   ```

2. **避免在循环中创建和删除对象**
   ```cpp
   // 不好：每次都 new/delete
   void TimerUpdate() {
       HPEN pen = CreatePen(...);
       // ... 使用
       DeleteObject(pen);
   }

   // 好：创建一次，重复使用（或移到绘制函数外）
   ```

3. **限制鱼的数量**
   ```cpp
   if (units.size() < 20) {  // 最多20条鱼
       // 生成新鱼
   }
   ```

---

## 7. 扩展功能建议

### 7.1 增强游戏性

**1. 添加计分系统**

```cpp
// 全局变量
int playerScore = 0;

// 吃掉小鱼时
if (bigFish->isPlayer) {
    playerScore += (int)(smallFish->size * 10);
}

// 绘制分数
wchar_text scoreText[64];
wsprintf(scoreText, L"得分: %d", playerScore);
TextOut(hdc_memBuffer, 20, 20, scoreText, wcslen(scoreText));
```

**2. 添加食物系统**

创建新的 Unit 类型代表食物：
- 不会移动
- 体积小
- 吃掉后增加少量大小

**3. 多关卡系统**

```cpp
// 关卡1：只有小鱼
// 关卡2：有中等大小的鱼
// 关卡3：出现大鱼，挑战性更高
```

**4. 时间限制**

```cpp
// 倒计时
if (currentStage->timeCountDown > 0) {
    currentStage->timeCountDown--;
    if (currentStage->timeCountDown == 0) {
        // 游戏结束，结算分数
    }
}
```

### 7.2 视觉效果

**1. 添加粒子效果（吃鱼时）**

创建简单的粒子结构：
```cpp
struct Particle {
    int x, y;
    double vx, vy;
    int lifetime;
    COLORREF color;
};
```

吃鱼时生成一些粒子，逐渐消失

**2. 摄像机跟随**

```cpp
// 计算偏移量，让玩家始终在屏幕中央
int cameraOffsetX = WINDOW_WIDTH/2 - player->x;
int cameraOffsetY = WINDOW_HEIGHT/2 - player->y;

// 绘制时应用偏移
TransparentBlt(hdc_memBuffer,
    unit->x + cameraOffsetX - ...,
    unit->y + cameraOffsetY - ..., ...);
```

**3. 背景音乐和音效**

使用 `PlaySound` 函数：
```cpp
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// 播放背景音乐（循环）
PlaySound(L"res/bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

// 播放音效（一次）
PlaySound(L"res/eat.wav", NULL, SND_FILENAME | SND_ASYNC);
```

### 7.3 UI 改进

**1. 暂停菜单**

```cpp
bool gamePaused = false;

// 按 ESC 暂停
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (wParam == VK_ESCAPE) {
        gamePaused = !gamePaused;
        currentStage->timerOn = !gamePaused;
    }
}

// 绘制暂停提示
if (gamePaused) {
    // 半透明遮罩
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    SetBkMode(hdc_memBuffer, TRANSPARENT);
    // ... 绘制 "PAUSED" 文字
}
```

**2. 血量条（生命值显示）**

在每条鱼上方绘制血条：
```cpp
int barWidth = 50;
int barHeight = 5;
int barX = unit->x - barWidth/2;
int barY = unit->y - UNIT_SIZE_Y/2 - 10;

// 背景（红色）
HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
RECT bgRect = {barX, barY, barX + barWidth, barY + barHeight};
FillRect(hdc_memBuffer, &bgRect, redBrush);

// 前景（绿色，根据生命值）
int healthWidth = (barWidth * unit->health) / 100;
HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
RECT fgRect = {barX, barY, barX + healthWidth, barY + barHeight};
FillRect(hdc_memBuffer, &fgRect, greenBrush);

DeleteObject(redBrush);
DeleteObject(greenBrush);
```

**3. 开始菜单优化**

添加更多按钮：
- 开始游戏
- 选择关卡
- 设置（调整难度、音量等）
- 退出

### 7.4 高级功能

**1. 存档系统**

```cpp
#include <fstream>

// 保存游戏
void SaveGame() {
    std::ofstream file("save.dat", std::ios::binary);
    file.write((char*)&playerScore, sizeof(playerScore));
    file.write((char*)&player->size, sizeof(float));
    // ... 保存其他数据
    file.close();
}

// 加载游戏
void LoadGame() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        file.read((char*)&playerScore, sizeof(playerScore));
        file.read((char*)&player->size, sizeof(float));
        // ... 加载其他数据
        file.close();
    }
}
```

**2. 排行榜**

使用 `vector` 存储最高分记录，排序后显示

**3. 不同鱼的特殊能力**

```cpp
// 为 Unit 添加技能
enum UnitSkill {
    SKILL_NONE,
    SKILL_SPEED_BOOST,  // 加速
    SKILL_INVINCIBLE    // 无敌
};

struct Unit {
    // ...
    UnitSkill skill;
    int skillCooldown;
};
```

---

## 总结

### 开发流程回顾

1. **理解基础** → 学习 Win32 消息机制和 GDI 绘图
2. **熟悉框架** → 阅读现有代码，理解程序架构
3. **小步迭代** → 一次添加一个功能，及时测试
4. **完善游戏** → 添加核心玩法、优化体验
5. **测试调试** → 修复 bug，提升稳定性
6. **扩展功能** → 添加更多创意功能

### 学习资源

**Windows API 官方文档：**
- [Windows API Index](https://docs.microsoft.com/en-us/windows/win32/apiindex/windows-api-list)
- [Windows GDI](https://docs.microsoft.com/en-us/windows/win32/gdi/windows-gdi)

**调试工具：**
- Visual Studio 调试器
- Debug 输出窗口
- Memory Profiler（检测内存泄漏）

### 最后的建议

1. **不要怕出错** - 编程就是不断试错的过程
2. **善用调试器** - 比盲目猜测高效100倍
3. **注释代码** - 一周后的你会感谢现在的你
4. **版本控制** - 使用 Git 保存每个工作版本
5. **参考但不抄袭** - 理解原理比复制代码更重要

**祝你开发顺利！如果遇到问题，记得：**
- 仔细阅读错误信息
- 使用断点调试
- 查阅文档
- 尝试搜索相似问题

---

*本指南由 Claude Code 生成，基于项目现有框架编写。*
*最后更新：2025年*
