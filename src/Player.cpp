#include "Player.h"
#include "OPimgui.h"
#include "GameState.h"

void Player::Init() {
	OPCMAN.Load("swordsman.opss");
	OPCMAN.Load("caveman.opss");
	sprites[0] = (OPsprite*)OPCMAN.Get("caveman/Idle");
	sprites[1] = (OPsprite*)OPCMAN.Get("caveman/Hit");
	sprites[2] = (OPsprite*)OPCMAN.Get("caveman/Walk");
	sprites[3] = (OPsprite*)OPCMAN.Get("caveman/Drag");
	sprite3D = OPsprite3DCreate(sprites, NULL);
	sprite3D->FrameRate = 12.0f;
	sprite3D->Scale = OPvec3(1.0 / 42.0);
	hurtTimer = 0;
	pushAmount = 0;
	stamina = 30;
	running = false;
	attackRange = 0.25f;
	damage = 2;
	staminaUsage = 2;
}

void Player::Update(OPtimer* timer) {
	OPsprite3DUpdate(sprite3D, timer->Elapsed);

	prevPosition = position;
	hurtTimer -= timer->Elapsed;

	// Allow running by increasing speed
	if (RunPressed() && stamina > 1) {
		speed = 3.0;
		framerate = 2.0;
		running = true;
	}
	else {
		speed = 1.0;
		framerate = 1.0;
		running = false;
	}

	position.x += pushAmount;
	pushAmount *= 0.8;
	if (OPabs(pushAmount) < 0.001) {
		pushAmount = 0;
	}

	// If attacking, and attacking animation has finished
	// playing back, then go idle
	if (!canAttackAgain && sprite3D->LoopsCompleted > 0) {
		canAttackAgain = true;
		if (attacking) {
			OPfmodPlay(missed);
		}
		attacking = false;
		SetIdle();
	}


	// Move to the left
	if (MoveLeftPressed()) {
		position.x -= speed * timer->Elapsed * 0.001f * OPKEYBOARD.IsDown(OPkeyboardKey::A);
		position.x += speed * timer->Elapsed * 0.001f * OPGAMEPADS[0]->LeftThumbX();
		sprite3D->Direction = -1;
		SetWalking();
	}

	// Move to the right
	if (MoveRightPressed()) {
		position.x += speed * timer->Elapsed * 0.001f * OPKEYBOARD.IsDown(OPkeyboardKey::D);
		position.x += speed * timer->Elapsed * 0.001f * OPGAMEPADS[0]->LeftThumbX();
		sprite3D->Direction = 1;
		SetWalking();
	}


	actionPressed = AttackPressed();



	if (running) {
		// Use up stamina ( 1 per second )
		stamina -= timer->Elapsed * 0.001f * 2.0f;
	}
	

	if (restrictMovement) {
		// Prevent going all the way to the left and to the right
		if (position.x < 0) position.x = 0;
		if (position.x > 24) position.x = 24;
	}


	// Setup the bounds
	boundingBox = OPboundingBox3D(
		OPvec3(position.x - 0.25, position.y, -0.1),
		OPvec3(position.x + 0.25, position.y + 0.5, 0.1));
	attackBoundingBox = OPboundingBox3D(
		OPvec3(position.x - attackRange, position.y, -0.1),
		OPvec3(position.x + attackRange, position.y + 0.5, 0.1));
}

void Player::UseAction() {


	// If we can attack, and we're not dragging anything
	if (canAttackAgain && !drag && actionPressed) {
		SetAttacking();
		stamina-= staminaUsage;
	}


	// If not moving, go idle
	if (!MovePressed() && !attacking) {
		SetIdle();
	}
}

void Player::Render(OPfloat delta, OPcam* camera, OPvec3 lightPos, bool useLight, f32 attenuation, f32 ambient) {
	sprite3D->Position = OPvec3Tween(prevPosition, position, delta);
	sprite3D->Position.y -= 5.0 / 42.0;
	sprite3D->Position.z += 0.05;

	OPsprite3DPrepRender(sprite3D, camera, OPVEC3_ZERO, 0);
	OPeffectSet("uLightPos", &lightPos);
	OPeffectSet("uUseLight", useLight);
	OPeffectSet("uAttenuation", attenuation);
	OPeffectSet("uAmbient", ambient);
	OPeffectSet("uCamPos", &camera->pos);
	if ((hurtTimer > 0 && hurtTimer < 200) || hurtTimer > 300) {
		OPeffectSet("uShowHurt", true);
	}
	else {
		OPeffectSet("uShowHurt", false);
	}
	OPrenderDrawBufferIndexed(0);
}

void Player::RenderDebug() {
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::Begin("Player");
	ImGui::Checkbox("Attacking", &attacking);
	ImGui::Checkbox("Can Attack Again", &canAttackAgain);
	ImGui::Checkbox("Dragging", &drag);
	ImGui::Checkbox("Moving", &moving);
	ImGui::Checkbox("Running", &running);
	ImGui::End();
}