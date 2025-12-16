#include "EntityManager.h"
#include "GameState.h"
#include <cmath>
#include <cstdlib>

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
	button->locked = false;  // 默认不锁定
	return button;
}

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
	else if (side == UNIT_SIDE_FISH3) {
		unit->img = bmp_Unit_Fish3;
		unit->direction = UNIT_DIRECT_RIGHT;
	}

	unit->type = type;
	unit->state = UNIT_STATE_HOLD;

	if (type == UNIT_FISH_TYPE1) {
		unit->frame_width = FISH_TYPE1_FRAME_WIDTH;
		unit->frame_height = FISH_TYPE1_FRAME_HEIGHT;
	}
	else if (type == UNIT_FISH_TYPE2) {
		unit->frame_width = FISH_TYPE2_FRAME_WIDTH;
		unit->frame_height = FISH_TYPE2_FRAME_HEIGHT;
	}
	else if (type == UNIT_FISH_TYPE3) {
		unit->frame_width = FISH_TYPE3_FRAME_WIDTH;
		unit->frame_height = FISH_TYPE3_FRAME_HEIGHT;
	}
	else {
		unit->frame_width = 64;
		unit->frame_height = 64;
	}

	unit->frame_row = 0;
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

void UpdateUnits(HWND hWnd)
{
	for (int i = 0; i < units.size(); i++) {
		Unit* unit = units[i];

		if (unit->isPlayer) {
			UnitBehaviour_Player(unit);
		}
		else {
			switch (unit->type) {
			case UNIT_FISH_TYPE1:
				break;
			case UNIT_FISH_TYPE2:
				UnitBehaviour_SwimAcross(unit);
				break;
			}
		}
	}

	// 清理已死亡单位
	for (int i = 0; i < units.size(); ) {
		if (units[i]->health <= 0) {
			delete units[i];
			units.erase(units.begin() + i);
		}
		else {
			i++;
		}
	}

	// 碰撞检测
	Unit* player = NULL;
	for (int i = 0; i < units.size(); i++) {
		if (units[i]->isPlayer) {
			player = units[i];
			break;
		}
	}

	if (player) {
		int currentFrame = 0;
		if (player->frame_sequence && player->frame_count > 0) {
			currentFrame = player->frame_sequence[player->frame_id];
		}

		if (player->state == UNIT_STATE_ATTACK && (currentFrame >= 3)) {
			for (int i = 0; i < units.size(); ) {
				Unit* other = units[i];
				if (other != player && other->side != player->side) {
					double dx = player->x - other->x;
					double dy = player->y - other->y;
					double dist = sqrt(dx * dx + dy * dy);
					double eatDist = (player->frame_width * player->size + other->frame_width * other->size) * 0.5;

					if (dist < eatDist) {
						player->size += 0.01f;
						if (player->size > 3.0f) player->size = 3.0f;
						progressValue += 5;
						if (progressValue > 100) progressValue = 100;
						delete other;
						units.erase(units.begin() + i);
						continue;
					}
				}
				i++;
			}
		}
	}
}

void UnitBehaviour_SwimAcross(Unit* unit)
{
	unit->wave_timer += 0.05;
	unit->x += unit->vx;
	unit->y = unit->initial_y + sin(unit->wave_timer) * 30;

	unit->frame_id++;
	unit->frame_id = unit->frame_id % unit->frame_count;
	int column = unit->frame_sequence[unit->frame_id];
	unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);

	if (unit->vx > 0 && unit->x > WINDOW_WIDTH + 100) {
		unit->health = 0;
	}
	else if (unit->vx < 0 && unit->x < -100) {
		unit->health = 0;
	}
}

void UnitBehaviour_Player(Unit* unit)
{
	double dirX = mouseX - unit->x;
	double dirY = mouseY - unit->y;
	double dirLen = sqrt(dirX * dirX + dirY * dirY) + 0.0001;

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

	double maxSpeed = (UNIT_SPEED * 3) + (unit->size * 3);
	double acceleration = 0.2;
	double friction = 0.99;
	double stopRadius = 8.0;

	double targetVx = 0;
	double targetVy = 0;
	bool nearMouse = (dirLen < stopRadius);

	if (!nearMouse) {
		if (dirX > 0) {
			unit->direction = UNIT_DIRECT_RIGHT;
		}
		else {
			unit->direction = UNIT_DIRECT_LEFT;
		}
		targetVx = (dirX / dirLen) * maxSpeed;
		targetVy = (dirY / dirLen) * maxSpeed;
	}
	else {
		targetVx = 0;
		targetVy = 0;
	}

	int next_state = unit->state;
	switch (unit->state) {
	case UNIT_STATE_HOLD:
		next_state = UNIT_STATE_WALK;
		break;
	case UNIT_STATE_WALK:
		if (nearestEnemy && minEnemyDist < (32 + unit->frame_width * unit->size * 0.5 + nearestEnemy->frame_width * nearestEnemy->size * 0.5)) {
			next_state = UNIT_STATE_ATTACK;
		}
		else {
			if (nearMouse) {
				unit->vx *= 0.6;
				unit->vy *= 0.6;
				if (abs(unit->vx) < 0.1) unit->vx = 0;
				if (abs(unit->vy) < 0.1) unit->vy = 0;
			}
			else {
				unit->vx += (targetVx - unit->vx) * acceleration;
				unit->vy += (targetVy - unit->vy) * acceleration;
			}
		}
		break;
	case UNIT_STATE_ATTACK:
	{
		double attackAccel = acceleration * 0.8;
		if (nearMouse) {
			unit->vx *= 0.9;
			unit->vy *= 0.9;
		}
		else {
			unit->vx += (targetVx - unit->vx) * attackAccel;
			unit->vy += (targetVy - unit->vy) * attackAccel;
		}
		if (unit->frame_id >= unit->frame_count - 1) {
			if (!nearestEnemy || minEnemyDist >= (32 + unit->frame_width * unit->size * 0.5 + nearestEnemy->frame_width * nearestEnemy->size * 0.5)) {
				next_state = UNIT_STATE_WALK;
			}
		}
		break;
	}
	};

	if (next_state != unit->state) {
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
			break;
		case UNIT_STATE_ATTACK:
			unit->frame_sequence = FRAMES_ATTACK;
			unit->frame_count = FRAMES_ATTACK_COUNT;
			break;
		};
	}

	unit->x += unit->vx;
	unit->y += unit->vy;
	unit->frame_id++;
	unit->frame_id = unit->frame_id % unit->frame_count;
	int column = unit->frame_sequence[unit->frame_id];
	unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);
}

void UnitBehaviour_2(Unit* unit)
{
	int change_prob = rand() % 100;
	int next_state = unit->state;

	if (unit->x < 0 || unit->x > WINDOW_WIDTH) {
		unit->vx = -unit->vx;
		unit->x += unit->vx * 2;
	}
	if (unit->y < 0 || unit->y > WINDOW_HEIGHT) {
		unit->vy = -unit->vy;
		unit->y += unit->vy * 2;
	}

	if (unit->state == UNIT_STATE_HOLD) {
		if (change_prob < 5) {
			next_state = UNIT_STATE_WALK;
			double angle = (rand() % 360) * 3.14159 / 180.0;
			unit->vx = cos(angle) * UNIT_SPEED;
			unit->vy = sin(angle) * UNIT_SPEED;
		}
	}
	else if (unit->state == UNIT_STATE_WALK) {
		if (change_prob < 2) {
			next_state = UNIT_STATE_HOLD;
		}
		else if (change_prob < 4) {
			double angle = (rand() % 360) * 3.14159 / 180.0;
			unit->vx = cos(angle) * UNIT_SPEED;
			unit->vy = sin(angle) * UNIT_SPEED;
		}
	}

	if (unit->vx > 0) {
		unit->direction = UNIT_DIRECT_RIGHT;
	}
	else if (unit->vx < 0) {
		unit->direction = UNIT_DIRECT_LEFT;
	}

	if (next_state != unit->state) {
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

	unit->x += unit->vx;
	unit->y += unit->vy;
	unit->frame_id++;
	unit->frame_id = unit->frame_id % unit->frame_count;
	int column = unit->frame_sequence[unit->frame_id];
	unit->frame_column = column + unit->direction * (UNIT_LAST_FRAME - 2 * column);
}
