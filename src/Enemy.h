#pragma once

struct Enemy;

#include "./OPengine.h"
struct Player;

struct Enemy {
	OPsprite3D* sprite3D;
	OPsprite* sprites[3];
	OPvec3 position, prevPosition;
	OPvec3 target, from;
	OPboundingBox3D boundingBox;
	OPboundingBox3D visualBoundingBox;
	int health = 3;
	i32 startingHealth = 3;
	int staminaGain = 3;
	i64 attackingTimer = 1000;
	i64 freakOutTimer = 5000;
	i64 hurtTimer = 500;
	OPfloat pushAmount = 0;
	bool attacks = false;
	bool attacking = false;
	bool biting = false;
	Player* player = NULL;
	bool visible = true;
	OPfloat speed = 0.5;
	OPfloat normalSpeed = 0.5f;
	OPfloat hitSpeed = 4.0;
	f32 startingPoint;

	void Init(ui32 enemyType);
	void Update(OPtimer* timer);
	void Render(OPfloat delta, OPcam* camera, OPvec3 lightPos, bool useLight, f32 attenuation, f32 ambient);
};