// ContraGame.cpp : 定义应用程序的入口点。
//

#include "FishGame.h"

using namespace std;


#pragma region 全局变量

#define MAX_LOADSTRING			100		

// 全局变量: 
HINSTANCE hInst; // 当前窗体实例
WCHAR szTitle[MAX_LOADSTRING]; // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING]; // 主窗口类名



HBITMAP bmp_start_bckground;	    //游戏开始前背景图像资源
HBITMAP bmp_game_bckground; //进入游戏后背景图像资源
HBITMAP bmp_StartButton;	//开始按钮图像资源
HBITMAP bmp_Unit_Fish1;		//小鱼1图像资源
HBITMAP bmp_Unit_Fish2;		//小鱼2图像资源

HBITMAP bmp_Start_Background;		//生成的背景图像

HBITMAP bmp_Stage_Background;		//生成的背景图像


Stage* currentStage = NULL; //当前场景状态
vector<Unit*> units;		//单位
vector<Button*> buttons;	//按钮

int mouseX = 0;
int mouseY = 0;
bool mouseDown = false;
bool keyUpDown = false;
bool keyDownDown = false;
bool keyLeftDown = false;
bool keyRightDown = false;
int progressValue = 0;//进度条值初始化

//帧
int FRAMES_HOLD[] = { 0 };
int FRAMES_HOLD_COUNT = 1;
int FRAMES_WALK[] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 };
int FRAMES_WALK_COUNT = 16;
int FRAMES_ATTACK[] = { 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
int FRAMES_ATTACK_COUNT = 24;



// TODO: 在此添加其它全局变量





double const PI = acos(double(-1));


#pragma endregion


#pragma region Win32程序框架



// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CONTRAGAME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CONTRAGAME));

	MSG msg;

	// 主消息循环: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CONTRAGAME);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindow(szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // 设置窗口样式，不可改变大小，不可最大化,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT + WINDOW_TITLEBARHEIGHT,
		nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		// 初始化游戏窗体
		InitGame(hWnd, wParam, lParam);
		break;
	case WM_KEYDOWN:
		// 键盘按下事件
		KeyDown(hWnd, wParam, lParam);
		break;
	case WM_KEYUP:
		// 键盘松开事件
		KeyUp(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		// 鼠标移动事件
		MouseMove(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		// 鼠标左键按下事件
		LButtonDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		// 鼠标左键松开事件
		LButtonUp(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		// 定时器事件
		if (currentStage != NULL && currentStage->timerOn) TimerUpdate(hWnd, wParam, lParam);
		break;
	case WM_PAINT:
		// 绘图
		Paint(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


#pragma endregion


#pragma region 事件处理函数

// 初始化游戏窗体函数
void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//加载图像资源
	bmp_start_bckground = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_START_BG));
	bmp_game_bckground = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_STAGE_BG));
	bmp_StartButton = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_START));
	bmp_Unit_Fish1 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish1));
	bmp_Unit_Fish2 = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_Fish2));

	//添加按钮

	Button* startButton = CreateButton(BUTTON_STARTGAME, bmp_StartButton, BUTTON_STARTGAME_WIDTH, BUTTON_STARTGAME_HEIGHT,
		(WINDOW_WIDTH - BUTTON_STARTGAME_WIDTH) / 2, (WINDOW_WIDTH - BUTTON_STARTGAME_HEIGHT) / 2);
	buttons.push_back(startButton);

	//初始化背景
	bmp_Start_Background = InitBackGround(hWnd, bmp_start_bckground);
	bmp_Stage_Background = InitBackGround(hWnd, bmp_game_bckground);

	//初始化开始场景
	InitStage(hWnd, STAGE_STARTMENU);

	//初始化主计时器
	SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
}

// 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_UP:
		keyUpDown = true;
		break;
	case VK_DOWN:
		keyDownDown = true;
		break;
	case VK_LEFT:
		keyLeftDown = true;
		break;
	case VK_RIGHT:
		keyRightDown = true;
		break;
	default:
		break;
	}
}

// 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// TODO
	switch (wParam)
	{
	case VK_UP:
		keyUpDown = false;
		break;
	case VK_DOWN:
		keyDownDown = false;
		break;
	case VK_LEFT:
		keyLeftDown = false;
		break;
	case VK_RIGHT:
		keyRightDown = false;
		break;
	default:
		break;
	}
}

// 鼠标移动事件处理函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
}

// 鼠标左键按下事件处理函数
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
	mouseDown = true;

	for (int i = 0; i < buttons.size(); i++)
	{
		Button* button = buttons[i];
		if (button->visible)
		{
			if (button->x <= mouseX
				&& button->x + button->width >= mouseX
				&& button->y <= mouseY
				&& button->y + button->height >= mouseY)
			{
				switch (button->buttonID) {
				case BUTTON_STARTGAME:
				{
					//TODO：判断进入哪个关卡
					InitStage(hWnd, STAGE_1);
				}
				break;
				}
			}
		}
	}

}

// 鼠标左键松开事件处理函数
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam);
	mouseY = GET_Y_LPARAM(lParam);
	mouseDown = false;
}

// 定时器事件处理函数
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UpdateUnits(hWnd);

	// STAGE_1 随机生成小鱼
	if (currentStage != NULL && currentStage->stageID == STAGE_1) {
		// 约 3% 的概率生成一条新鱼 (30ms * 33 ≈ 1秒生成一条)
		if (rand() % 100 < 3) {
			int side = rand() % 2; // 0: 左边生成, 1: 右边生成
			int y = rand() % (WINDOW_HEIGHT - 100) + 50;
			int x, direction;
			double vx;

			if (side == 0) { // 左边生成，向右游
				x = -80;
				vx = 2.0 + (rand() % 20) / 10.0; // 速度 2.0 ~ 4.0
				direction = UNIT_DIRECT_RIGHT;
			}
			else { // 右边生成，向左游
				x = WINDOW_WIDTH + 80;
				vx = -(2.0 + (rand() % 20) / 10.0);
				direction = UNIT_DIRECT_LEFT;
			}

			Unit* fish = CreateUnit(UNIT_SIDE_FISH2, UNIT_FISH_TYPE2, x, y, 100);
			fish->vx = vx;
			fish->vy = 0; // 垂直由 sin 控制
			fish->direction = direction;
			fish->state = UNIT_STATE_WALK; // 始终为行走状态
			fish->frame_sequence = FRAMES_WALK;
			fish->frame_count = FRAMES_WALK_COUNT;
			
			// 设置波浪运动参数
			fish->initial_y = y;
			fish->wave_timer = (rand() % 314) / 100.0; // 随机初始相位

			// 设置正确的贴图方向
			fish->frame_column = fish->type + fish->direction * (UNIT_LAST_FRAME - 2 * fish->type);

			units.push_back(fish);
		}
	}

	//刷新显示
	InvalidateRect(hWnd, NULL, FALSE);
}


#pragma endregion


#pragma region 其它游戏状态处理函数



//TODO: 添加游戏需要的更多函数

// 添加按钮函数
Button* CreateButton(int buttonID, HBITMAP img, int width, int height, int x, int y)
{
	Button* button = new Button();
	button->buttonID = buttonID;
	button->img = img;
	button->width = width;
	button->height = height;
	button->x = x;
	button->y = y;

	button->visible = false;
	return button;
}

// 添加主角函数
Unit* CreateUnit(int side, int type, int x, int y, int health)
{
	Unit* unit = new Unit();
	unit->side = side;
	if (side == UNIT_SIDE_FISH1) {
		unit->img = bmp_Unit_Fish1;
		unit->direction = UNIT_DIRECT_LEFT;
	}
	else if (side == UNIT_SIDE_FISH2) {
		unit->img = bmp_Unit_Fish2;
		unit->direction = UNIT_DIRECT_RIGHT;
	}

	unit->type = type;
	unit->state = UNIT_STATE_HOLD;


	unit->frame_row = type;
	unit->frame_column = UNIT_LAST_FRAME * unit->direction;

	unit->frame_sequence = FRAMES_HOLD;
	unit->frame_count = FRAMES_HOLD_COUNT;
	unit->frame_id = 0;

	unit->x = x;
	unit->y = y;
	unit->vx = 0;
	unit->vy = 0;
	unit->health = health;

	unit->size = 1.0f;
	unit->isPlayer = false;

	unit->wave_timer = 0;
	unit->initial_y = y;

	return unit;
}



// 初始化游戏场景函数
void InitStage(HWND hWnd, int stageID)
{
	// 初始化场景实例
	if (currentStage != NULL) delete currentStage;
	currentStage = new Stage();
	currentStage->stageID = stageID;


	if (stageID == STAGE_STARTMENU) {
		currentStage->bg = bmp_Start_Background;
		currentStage->timeCountDown = 0;
		currentStage->timerOn = false;

		//显示开始界面的按钮
		for (int i = 0; i < buttons.size(); i++)
		{
			Button* button = buttons[i];
			if (button->buttonID == BUTTON_STARTGAME)
			{
				button->visible = true;
			}
			else
			{
				button->visible = false;
			}
		}


	}
	else if (stageID >= STAGE_1 && stageID <= STAGE_1) //TODO：添加多个游戏场景
	{
		currentStage->stageID = stageID;
		currentStage->bg = bmp_Stage_Background;
		currentStage->timeCountDown = 10000;
		currentStage->timerOn = true;

		//显示游戏界面的按钮
		for (int i = 0; i < buttons.size(); i++)
		{
			Button* button = buttons[i];
			if (false) //TODO：加载游戏界面需要的按钮
			{
				button->visible = true;
			}
			else
			{
				button->visible = false;
			}
		}


		// 按场景初始化单位
		switch (stageID) {
		case STAGE_1:
		{
			Unit* player = CreateUnit(UNIT_SIDE_FISH1, UNIT_FISH_TYPE1,
				WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 100);
			player->isPlayer = true;
			player->size = 1.5f;  // 玩家初始稍大
			units.push_back(player);

			// 移除初始生成的5条小鱼，改为动态生成
			break;
		}
		default:
			break;
		}


	}

	//刷新显示
	InvalidateRect(hWnd, NULL, FALSE);
}


// 刷新单位状态函数
void UpdateUnits(HWND hWnd)
{
	for (int i = 0; i < units.size(); i++) {
		Unit* unit = units[i];

		//根据单位类型选择行为函数
		switch (unit->type) {
		case UNIT_FISH_TYPE1:
			UnitBehaviour_1(unit);
			break;
		case UNIT_FISH_TYPE2:
			UnitBehaviour_SwimAcross(unit); // 改用横向游动行为
			break;
		}
	}

	// 清理已死亡（health <= 0）的单位 (包含被吃掉或游出边界的)
	for (int i = 0; i < units.size(); ) {
		if (units[i]->health <= 0) {
			delete units[i];
			units.erase(units.begin() + i);
		}
		else {
			i++;
		}
	}

	// 查找玩家单位
	Unit* player = NULL;
	for (int i = 0; i < units.size(); i++) {
		if (units[i]->isPlayer) {
			player = units[i];
			break;
		}
	}

	// 吃鱼碰撞检测
	if (player) {
		// 获取玩家当前显示的图片帧索引
		int currentFrame = 0;
		if (player->frame_sequence && player->frame_count > 0) {
			currentFrame = player->frame_sequence[player->frame_id];
		}

		// 只要攻击状态开始（帧索引>=3），且处于攻击状态，就进行吃鱼判定
		if (player->state == UNIT_STATE_ATTACK && (currentFrame >= 3)) {
			for (int i = 0; i < units.size(); ) {
				Unit* other = units[i];
				// 只有非玩家且不同阵营的鱼才会被吃
				if (other != player && other->side != player->side) {
					double dx = player->x - other->x;
					double dy = player->y - other->y;
					double dist = sqrt(dx * dx + dy * dy);

					// 判定距离：基于双方大小，随玩家体型增大而增加判定范围
					double eatDist = (UNIT_SIZE_X * player->size + UNIT_SIZE_X * other->size) * 0.5;

					if (dist < eatDist) {
						// 玩家长大
						player->size += 0.1f;
						if (player->size > 3.0f) player->size = 3.0f; // 限制最大大小

						// 增加进度
						progressValue += 5;
						if (progressValue > 100) progressValue = 100;

						// 删除被吃掉的鱼
						delete other;
						units.erase(units.begin() + i);
						continue; // 删除后索引自动指向下一个，无需递增
					}
				}
				i++;
			}
		}
	}
}

//单位行为函数
void UnitBehaviour_2(Unit* unit) {

	// 简单的随机游走逻辑
	int change_prob = rand() % 100;
	int next_state = unit->state;

	// 边界检查与反弹
	if (unit->x < 0 || unit->x > WINDOW_WIDTH) {
		unit->vx = -unit->vx;
		unit->x += unit->vx * 2;
	}
	if (unit->y < 0 || unit->y > WINDOW_HEIGHT) {
		unit->vy = -unit->vy;
		unit->y += unit->vy * 2;
	}

	// 状态转换逻辑
	if (unit->state == UNIT_STATE_HOLD) {
		// 在 HOLD 状态，有 5% 概率开始移动
		if (change_prob < 5) {
			next_state = UNIT_STATE_WALK;
			double angle = (rand() % 360) * 3.14159 / 180.0;
			unit->vx = cos(angle) * UNIT_SPEED;
			unit->vy = sin(angle) * UNIT_SPEED;
		}
	}
	else if (unit->state == UNIT_STATE_WALK) {
		// 在 WALK 状态，有 2% 概率停下来
		if (change_prob < 2) {
			next_state = UNIT_STATE_HOLD;
		}
		// 或者有 2% 概率改变方向
		else if (change_prob < 4) {
			double angle = (rand() % 360) * 3.14159 / 180.0;
			unit->vx = cos(angle) * UNIT_SPEED;
			unit->vy = sin(angle) * UNIT_SPEED;
		}
	}

	// 更新方向
	if (unit->vx > 0) {
		unit->direction = UNIT_DIRECT_RIGHT;
	}
	else if (unit->vx < 0) {
		unit->direction = UNIT_DIRECT_LEFT;
	}

	if (next_state != unit->state) {
		//状态变化
		unit->state = next_state;
		unit->frame_id = -1;

		switch (unit->state) {
		case UNIT_STATE_HOLD:
			unit->frame_sequence = FRAMES_HOLD;
			unit->frame_count = FRAMES_HOLD_COUNT;
			unit->vx = 0;
			unit->vy = 0;
			break;
		case UNIT_STATE_WALK:
			unit->frame_sequence = FRAMES_WALK;
			unit->frame_count = FRAMES_WALK_COUNT;
			if (unit->vx == 0 && unit->vy == 0) {
				double angle = (rand() % 360) * 3.14159 / 180.0;
				unit->vx = cos(angle) * UNIT_SPEED;
				unit->vy = sin(angle) * UNIT_SPEED;
			}
			break;
		case UNIT_STATE_ATTACK:
			unit->frame_sequence = FRAMES_ATTACK;
			unit->frame_count = FRAMES_ATTACK_COUNT;
			unit->vx = 0;
			unit->vy = 0;
			break;
		};
	}

	//动画运行到下一帧
	unit->x += unit->vx;
	unit->y += unit->vy;

	unit->frame_id++;
	unit->frame_id = unit->frame_id % unit->frame_count;

	int column = unit->frame_sequence[unit->frame_id];
	unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);


}

// 横向游动行为：直线移动 + 上下浮动
void UnitBehaviour_SwimAcross(Unit* unit) {
	// 更新波浪计时器
	unit->wave_timer += 0.05;

	// 水平移动
	unit->x += unit->vx;

	// 垂直浮动 (振幅 30，基于初始高度)
	unit->y = unit->initial_y + sin(unit->wave_timer) * 30;

	// 动画帧更新
	unit->frame_id++;
	unit->frame_id = unit->frame_id % unit->frame_count;

	int column = unit->frame_sequence[unit->frame_id];
	unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);

	// 边界检查与销毁
	// 向右游（vx > 0），超出右边界销毁
	if (unit->vx > 0 && unit->x > WINDOW_WIDTH + 100) {
		unit->health = 0; // 标记销毁
	}
	// 向左游（vx < 0），超出左边界销毁
	else if (unit->vx < 0 && unit->x < -100) {
		unit->health = 0; // 标记销毁
	}
}

void UnitBehaviour_1(Unit* unit) {

	double dirX = mouseX - unit->x;
	double dirY = mouseY - unit->y;
	double dirLen = sqrt(dirX * dirX + dirY * dirY) + 0.0001;

	// 寻找最近的敌对小鱼
	Unit* nearestEnemy = NULL;
	double minEnemyDist = 100000.0;

	for (int i = 0; i < units.size(); i++) {
		Unit* other = units[i];
		if (other != unit && other->side != unit->side) {
			double dx = other->x - unit->x;
			double dy = other->y - unit->y;
			double dist = sqrt(dx * dx + dy * dy);
			if (dist < minEnemyDist) {
				minEnemyDist = dist;
				nearestEnemy = other;
			}
		}
	}

	// 定义最大速度和加速度
	double maxSpeed = UNIT_SPEED * 1.5; // 略微加快速度 (原3.0 -> 4.5)
	double acceleration = 0.2; // 加速度因子
	double friction = 0.99;    // 摩擦力/减速因子 (减小摩擦，避免攻击时速度骤降追不上鱼)

	if (dirX > 0) {
		unit->direction = UNIT_DIRECT_RIGHT;
	}
	else {
		unit->direction = UNIT_DIRECT_LEFT;
	}


	//判断当前状态, 以及判断是否需要状态变化
	int next_state = unit->state;
	switch (unit->state) {
	case UNIT_STATE_HOLD:
		next_state = UNIT_STATE_WALK;
		break;
	case UNIT_STATE_WALK:
		// 攻击判定改为：如果有最近的敌人且距离小于阈值（例如 50 + 双方体型修正），则攻击
		// 这里使用一个简单的固定阈值配合体型修正
		if (nearestEnemy && minEnemyDist < (32 + UNIT_SIZE_X * unit->size * 0.5 + UNIT_SIZE_X * nearestEnemy->size * 0.5)) {
			next_state = UNIT_STATE_ATTACK;
		}
		else {
			// 平滑加速逻辑
			double targetVx = (dirX / dirLen) * maxSpeed;
			double targetVy = (dirY / dirLen) * maxSpeed;

			// 简单的插值实现加速
			unit->vx += (targetVx - unit->vx) * acceleration;
			unit->vy += (targetVy - unit->vy) * acceleration;
		}
		break;
	case UNIT_STATE_ATTACK:
		// 攻击时减速
		unit->vx *= friction;
		unit->vy *= friction;

		// 只有当攻击动画播放完毕（最后一帧）才考虑切换状态
		if (unit->frame_id >= unit->frame_count - 1) {
			// 如果附近没有敌人，或者最近敌人距离拉开，切换回行走
			if (!nearestEnemy || minEnemyDist >= (32 + UNIT_SIZE_X * unit->size * 0.5 + UNIT_SIZE_X * nearestEnemy->size * 0.5)) {
				next_state = UNIT_STATE_WALK;
			}
			// 否则继续攻击（循环播放）
		}
		break;
	};

	if (next_state != unit->state) {
		//状态变化
		unit->state = next_state;
		unit->frame_id = -1;

		switch (unit->state) {
		case UNIT_STATE_HOLD:
			unit->frame_sequence = FRAMES_HOLD;
			unit->frame_count = FRAMES_HOLD_COUNT;
			unit->vx = 0;
			unit->vy = 0;
			break;
		case UNIT_STATE_WALK:
			unit->frame_sequence = FRAMES_WALK;
			unit->frame_count = FRAMES_WALK_COUNT;
			// 注意：进入 WALK 状态时不重置速度为0，保留当前惯性以便平滑过渡
			break;
		case UNIT_STATE_ATTACK:
			unit->frame_sequence = FRAMES_ATTACK;
			unit->frame_count = FRAMES_ATTACK_COUNT;
			// 攻击开始时不强制停止，依靠 friction 减速
			break;
		};
	}

	//动画运行到下一帧
	unit->x += unit->vx;
	unit->y += unit->vy;

	unit->frame_id++;
	unit->frame_id = unit->frame_id % unit->frame_count;

	int column = unit->frame_sequence[unit->frame_id];
	unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);


}


#pragma endregion


#pragma region 绘图函数
// 绘图函数
void Paint(HWND hWnd)
{

	PAINTSTRUCT ps;
	HDC hdc_window = BeginPaint(hWnd, &ps);

	HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
	HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);

	//初始化缓存
	HBITMAP	blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_memBuffer, blankBmp);

	// 绘制背景到缓存
	//SelectObject(hdc_loadBmp, currentStage->bg);



	// 按场景分类绘制内容到缓存
	if (currentStage->stageID == STAGE_STARTMENU) {
		SelectObject(hdc_loadBmp, bmp_Start_Background);
		BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);
	}
	else if (currentStage->stageID >= STAGE_1 && currentStage->stageID <= STAGE_1) //TODO：添加多个游戏场景
	{
		SelectObject(hdc_loadBmp, bmp_Stage_Background);
		BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);
		// 绘制单位到缓存
		for (int i = 0; i < units.size(); i++) {
			Unit* unit = units[i];
			SelectObject(hdc_loadBmp, unit->img);

			//根据size调整绘制尺寸
			int drawWidth = (int)(UNIT_SIZE_X * unit->size);
			int drawHeight = (int)(UNIT_SIZE_Y * unit->size);

			TransparentBlt(
				hdc_memBuffer, unit->x - 0.5 * drawWidth, unit->y - 0.5 * drawHeight,
				drawWidth, drawHeight,
				hdc_loadBmp, UNIT_SIZE_X * unit->frame_column, UNIT_SIZE_Y * unit->frame_row, UNIT_SIZE_X, UNIT_SIZE_Y,
				RGB(255, 255, 255)
			);
		}



	}


	// 绘制按钮到缓存

	for (int i = 0; i < buttons.size(); i++)
	{
		Button* button = buttons[i];
		if (button->visible)
		{
			SelectObject(hdc_loadBmp, button->img);
			TransparentBlt(
				hdc_memBuffer, button->x, button->y,
				button->width, button->height,
				hdc_loadBmp, 0, 0, button->width, button->height,
				RGB(255, 255, 255)
			);
		}
	}
	//构造进度条
	if (currentStage->stageID == STAGE_1) {
		int barWidth = 400;
		int barHeight = 20;
		int barX = (WINDOW_WIDTH - barWidth) / 2;
		int barY = WINDOW_HEIGHT - 120;
		HBRUSH brushBorder = CreateSolidBrush(RGB(0, 0, 0));//初始化笔刷工具
		RECT rectBorder = { barX, barY, barX + barWidth, barY + barHeight };//设置进度条位置
		FrameRect(hdc_memBuffer, &rectBorder, brushBorder);
		DeleteObject(brushBorder);
		int filledWidth = (barWidth * progressValue) / 100;
		RECT filledRect = { barX + 1, barY + 1, barX + filledWidth - 1, barY + barHeight - 1 };
		HBRUSH brushFill = CreateSolidBrush(RGB(0, 200, 0));  // 用绿色填充进度条
		FillRect(hdc_memBuffer, &filledRect, brushFill);
		DeleteObject(brushFill);
		wchar_t text[32];
		wsprintf(text, L"当前进度:%d%%", progressValue);
		SetBkMode(hdc_memBuffer, TRANSPARENT);
		SetTextColor(hdc_memBuffer, RGB(255, 255, 255));
		TextOut(hdc_memBuffer, barX + barWidth / 2 - 15, barY - 20, text, wcslen(text));
	}
	//

	// 最后将所有的信息绘制到屏幕上
	BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);

	// 回收资源所占的内存（非常重要）
	DeleteObject(blankBmp);
	DeleteDC(hdc_memBuffer);
	DeleteDC(hdc_loadBmp);

	// 结束绘制
	EndPaint(hWnd, &ps);
}



// 初始化背景函数
HBITMAP InitBackGround(HWND hWnd, HBITMAP bmp_src) {
	PAINTSTRUCT ps;
	HDC hdc_window = BeginPaint(hWnd, &ps);

	HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
	HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);

	// 初始化缓存
	HBITMAP bmp_output = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_memBuffer, bmp_output);

	// 加载资源
	SelectObject(hdc_loadBmp, bmp_src);

	StretchBlt(
		hdc_memBuffer,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, // 目标尺寸
		hdc_loadBmp,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		SRCCOPY                    // 拷贝整图
	);

	// 回显到窗口（可选，仅初始化时）
	BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);

	DeleteDC(hdc_memBuffer);
	DeleteDC(hdc_loadBmp);
	EndPaint(hWnd, &ps);

	return bmp_output;
}


#pragma endregion