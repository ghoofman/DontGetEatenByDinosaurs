#pragma once

struct Player;

#include "./OPengine.h"
#include "Enemy.h"

struct Player {
	OPsprite3D* sprite3D;
	OPsprite* sprites[4];
	OPvec3 position, prevPosition;
	OPboundingBox3D boundingBox;
	OPboundingBox3D attackBoundingBox;
	OPfloat attackRange = 0.25f;
	bool attacking = false;
	bool actionPressed = false;
	bool canAttackAgain = true;
	bool drag = false;
	bool moving = false;
	bool running = false;
	bool restrictMovement = true;
	OPfloat speed = 1.0f;
	OPfloat framerate = 1.0f;
	OPfloat stamina = 100;
	OPfloat pushAmount = 0;
	i64 hurtTimer;
	i32 damage = 1;
	i32 staminaUsage = 1;
	Enemy* dragging = NULL;

	void Init();
	void Update(OPtimer* timer);
	void UseAction();
	void Render(OPfloat delta, OPcam* camera, OPvec3 lightPos, bool useLight, f32 attenuation, f32 ambient);
	void RenderDebug();

	inline void SetIdle() {
		sprite3D->FrameRate = 3.0f;
		OPsprite3DSetSprite(sprite3D, 0, false);
		moving = false;
	}

	inline void SetAttacking() {
		sprite3D->FrameRate = 12.0f;
		OPsprite3DSetSprite(sprite3D, 1, false);
		canAttackAgain = false;
		attacking = true;
	}

	inline void SetWalking() {
		if (attacking) return;
		if (drag) {
			OPsprite3DSetSprite(sprite3D, 3, false);
		}
		else {
			OPsprite3DSetSprite(sprite3D, 2, false);
		}
		sprite3D->FrameRate = 6.0f * framerate * 2.0 * (OPabs(OPGAMEPADS[0]->LeftThumbX()) + MoveLeftPressed() + MoveRightPressed());
		moving = true;
	}

	inline void SetDragging(Enemy* enemy) {
		drag = true;
		dragging = enemy;
		OPsprite3DSetSprite(sprite3D, 3, false);
	}

	inline bool RunPressed() {
		return OPKEYBOARD.IsDown(OPkeyboardKey::LSHIFT) || OPGAMEPADS[0]->RightTriggerIsDown() || OPMOUSE.WasPressed(OPmouseButton::RBUTTON);
	}

	inline bool AttackPressed() {
		return OPKEYBOARD.WasPressed(OPkeyboardKey::E) || OPGAMEPADS[0]->WasPressed(OPgamePadButton::X) || OPMOUSE.WasPressed(OPmouseButton::LBUTTON);
	}

	inline bool MoveLeftPressed() {
		return OPKEYBOARD.IsDown(OPkeyboardKey::A) || OPGAMEPADS[0]->LeftThumbIsLeft();
	}

	inline bool MoveRightPressed() {
		return OPKEYBOARD.IsDown(OPkeyboardKey::D) || OPGAMEPADS[0]->LeftThumbIsRight();
	}

	inline bool MovePressed() {
		return MoveLeftPressed() || MoveRightPressed();
	}

	inline void SetClub() {
		sprites[0] = (OPsprite*)OPCMAN.Get("caveman/IdleClub");
		sprites[1] = (OPsprite*)OPCMAN.Get("caveman/HitClub");
		sprites[2] = (OPsprite*)OPCMAN.Get("caveman/WalkClub");
		sprites[3] = (OPsprite*)OPCMAN.Get("caveman/DragClub");
		damage += 4;
		staminaUsage += 2;
		attackRange = 0.5f;
	}

	inline void SetAllowMovement() {
		if (restrictMovement) {
			position.x = 0;
		}
		restrictMovement = false;
	}
};