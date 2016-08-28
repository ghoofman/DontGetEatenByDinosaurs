#include "Enemy.h"
#include "Player.h"
#include "GameState.h"

void Enemy::Init(ui32 enemyType) {

	if (enemyType == 1) { // Bunny
		OPCMAN.Load("chicken.opss");
		sprites[0] = (OPsprite*)OPCMAN.Get("chicken/Move");
		sprites[1] = (OPsprite*)OPCMAN.Get("chicken/Dead");
		sprites[2] = (OPsprite*)OPCMAN.Get("chicken/Idle");
		attacks = false;
		staminaGain = 16;
		hitSpeed = 3.0;
		position.x = startingPoint = 2.0;
		health = startingHealth = 6;
	}
	else { // TRex
		OPCMAN.Load("trex.opss");
		sprites[0] = (OPsprite*)OPCMAN.Get("trex/Move");
		sprites[1] = (OPsprite*)OPCMAN.Get("trex/dead");
		sprites[2] = (OPsprite*)OPCMAN.Get("trex/Hit");
		attacks = true;
		staminaGain = 40;
		hitSpeed = 2.0;
		position.x = startingPoint = 8.0;
		health = startingHealth = 18;
	}

	sprite3D = OPsprite3DCreate(sprites, NULL);
	sprite3D->FrameRate = 12.0f;
	sprite3D->Scale = OPvec3(1.0 / 42.0);
	attackingTimer = 1000;
	freakOutTimer = 0;
	hurtTimer = 0;
	visible = true;
}

void Enemy::Update(OPtimer* timer) {
	OPsprite3DUpdate(sprite3D, timer->Elapsed);

	prevPosition = position;
	hurtTimer -= timer->Elapsed;	
	
	position.x += pushAmount;
	pushAmount *= 0.8;
	if (OPabs(pushAmount) < 0.001) {
		pushAmount = 0;
	}

	freakOutTimer -= timer->Elapsed;
	if (attacking) {
		attackingTimer -= timer->Elapsed;
	}

	if (attacking && attacks && player != NULL && attackingTimer <= 0 && attacking) {
		target.x = player->position.x;	
		
		// If attacking, and attacking animation has finished

		if (biting && attacks && boundingBox.Collision(player->boundingBox) != OPintersectionType::NONE && sprite3D->CurrentFrame == 1) {
			player->stamina -= 16;
			player->hurtTimer = 500;
			player->pushAmount = 0.1 * sprite3D->Direction;
			player = NULL;
			attacking = false;
			OPfmodPlay(bite);
			attackingTimer = 1000;
			cameraShakeTimer = 300;
		}
	}


	if (health > 0) {
		if (attacks && attacking && attackingTimer <= 0 && boundingBox.Collision(player->boundingBox) != OPintersectionType::NONE) {
			OPsprite3DSetSprite(sprite3D, 2, false);
			sprite3D->FrameRate = 6.0f;
			biting = true;
		}
		else {
			biting = false;
			sprite3D->FrameRate = 12.0f;
			OPsprite3DSetSprite(sprite3D, 0, false);
			if (freakOutTimer > 0) {
				speed = hitSpeed;
			}
			else {
				speed = normalSpeed;
			}

			if (OPabs(target.x - position.x) < 0.01 * speed) {
				target.x = startingPoint + OPrandom() * 4;
				from = position;
			}

			if (target.x < position.x) {
				position.x -= 0.001 * timer->Elapsed * speed;
				sprite3D->Direction = -1;
			}
			else if (target.x > position.x) {
				position.x += 0.001 * timer->Elapsed * speed;
				sprite3D->Direction = 1;
			}
		}
	}
	else {
		// dead
		OPsprite3DSetSprite(sprite3D, 1, false);
	}

	boundingBox = OPboundingBox3D(
		OPvec3(position.x - 0.25, position.y, -0.1),
		OPvec3(position.x + 0.25, position.y + 0.5, 0.1));

	if (sprite3D->Direction == 1) {
		visualBoundingBox = OPboundingBox3D(
			OPvec3(position.x - 0.25, position.y, -0.1),
			OPvec3(position.x + 2.0, position.y + 0.5, 0.1));
	}
	else {
		visualBoundingBox = OPboundingBox3D(
			OPvec3(position.x - 2.0, position.y, -0.1),
			OPvec3(position.x + 0.25, position.y + 0.5, 0.1));
	}
}

void Enemy::Render(OPfloat delta, OPcam* camera, OPvec3 lightPos, bool useLight, f32 attenuation, f32 ambient) {
	if (!visible) return;
	sprite3D->Position = OPvec3Tween(prevPosition, position, delta);
	sprite3D->Position.y -= 5.0 / 42.0;

	OPsprite3DPrepRender(sprite3D, camera, OPVEC3_ZERO, 0);
	OPeffectSet("uLightPos", &lightPos);
	OPeffectSet("uUseLight", useLight);
	OPeffectSet("uAttenuation", attenuation);
	OPeffectSet("uAmbient", ambient);
	OPeffectSet("uCamPos", &camera->pos);
	if ((hurtTimer > 0 && hurtTimer < 200) || hurtTimer > 300) {
		OPeffectSet("uShowHurt", true);
	} else {
		OPeffectSet("uShowHurt", false);
	}
	OPrenderDrawBufferIndexed(0);
}