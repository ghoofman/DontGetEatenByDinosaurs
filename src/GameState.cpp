#include "./GameState.h"
#include "Player.h"
#include "Enemy.h"
#include "StaticEntity.h"
#include "ModelAnimated.h"
#include "OPimgui.h"

OPscene scene;
OPrendererForward* renderer;
OPmodel* model;
OPmodel* modelRock;
OPmaterialPhong materialPhong;
OPmaterialPhongInstance* materialInstance;
OPmaterialPhongInstance* materialInstanceRock;
OPcamFreeFlight camera;

Player player;
Enemy enemy[10];
ui32 enemyIndex = 0;
StaticEntity controls;
StaticEntity staticEntity;
StaticEntity trees[20];
ui32 treeCount = 20;
StaticEntity bush, sky;
ModelAnimated modelAnimated;
OPvec3 cameraPrevPosition;
bool useLight = false;
float attenuation = 3.0f;
float MaxAttenuation = 4.0f;
float MinAttenuation = 2.0f;
float ambient = 0.1;
float useLightAmount = 0.0f;
OPvec3 lightPos;

i64 monsterTime = 10000;

i64 dayCycle = 60000;
i64 dayCycleHalf = 30000;
i64 rabbitTimer = 5000;
i64 dayTimer = 0;

OPfmodSound* bg;
OPfmodSound* hit;
OPfmodSound* cook;
OPfmodSound* bite;
OPfmodSound* rockStrike;
OPfmodSound* missed;
OPfmodSound* shakeTree;
OPfmodSound* powerup;

i64 day = 1;
i64 showDayTimer = 0;
i64 showNomTimer = 0;
i64 cameraShakeTimer = 0;

OPfontManager fontManager;
i32 fireAttempts = 0;

#define USE_FREE_FORM_CAMERA 0

OPfmodChannel* bgChannel;
void ExampleStateInit(OPgameState* last) {
	bg = OPfmodLoadStream("Audio/BoundlessThunder2.mp3");
	hit = OPfmodLoad("Audio/Hit.wav");
	cook = OPfmodLoad("Audio/Cook.wav");
	bite = OPfmodLoad("Audio/Bite.wav");
	missed = OPfmodLoad("Audio/Missed.wav");
	rockStrike = OPfmodLoad("Audio/RockStrike.wav");
	shakeTree = OPfmodLoad("Audio/ShakeTree.wav");
	powerup = OPfmodLoad("Audio/PowerUp.wav");
	
	//OPfmodChannelGroup* fmodGroup = OPfmodCreateChannelGroup("bg");
	//OPfmodSetVolume(fmodGroup, 1.0);
	bgChannel = OPfmodPlay(bg);// , fmodGroup);
	//bgChannel->setChannelGroup(fmodGroup);
	//bgChannel->getPaused(false);
	//fmodGroup->setPaused(false);

	fontManager.Init("Minecrafter.opf", NULL, 0);

    renderer = OPNEW(OPrendererForward());
    scene.Init(renderer->GetBase(), 1000, 100);
	scene.camera->pos.x = 0;
	scene.camera->pos.y = 1;
	scene.camera->Update();
	materialPhong.Init();
	renderer->SetMaterial(&materialPhong.rootMaterial, 0);
	camera.Init(0.25f, 1.0f, OPvec3(0, 2, 5), 0.1f, 100.0f);
	if (USE_FREE_FORM_CAMERA) {
		scene.camera = &camera.Camera;
	}
	materialPhong.rootMaterial.AddParam("uUseLight", &useLight);
	materialPhong.rootMaterial.AddParam("uAttenuation", &attenuation);
	materialPhong.rootMaterial.AddParam("uAmbient", &ambient);
	materialPhong.rootMaterial.AddParam("uUseLightAmount", &useLightAmount);
	materialPhong.SetCamera(scene.camera);
	lightPos = OPvec3(1, 2, 2);
	materialPhong.SetLightPos(lightPos);
	materialInstance = materialPhong.CreateInstance();
	materialInstance->SetAlbedoMap("Dirt.png");
    model = (OPmodel*)OPCMAN.LoadGet("uvbox.opm");
	for (ui32 i = 0; i < 60; i++) {
		scene.Add(model, &materialInstance->rootMaterialInstance)->world.SetScl(0.5)->Translate(0.5 * i, -0.25, 0);
	}
	for (ui32 i = 0; i < 60; i++) {
		scene.Add(model, &materialInstance->rootMaterialInstance)->world.SetScl(0.5)->Translate(0.5 * i, -0.25, -0.5);
	}

	materialInstanceRock = materialPhong.CreateInstance();
	materialInstanceRock->SetAlbedoMap("Rock.png");
	for (ui32 j = 0; j < 3; j++) {
		for (ui32 i = 0; i < 4; i++) {
			scene.Add(model, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-0.5 - 0.5 * i, -0.25 + j * 0.5, 0);
		}
		for (ui32 i = 0; i < 4; i++) {
			scene.Add(model, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-0.5 - 0.5 * i, -0.25 + j * 0.5, -0.5);
		}
	}
	for (ui32 i = 1; i < 4; i++) {
		scene.Add(model, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-0.5 - 0.5 * i, -0.25 + 3 * 0.5, 0);
	}
	for (ui32 i = 1; i < 4; i++) {
		scene.Add(model, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-0.5 - 0.5 * i, -0.25 + 3 * 0.5, -0.5);
	}
	
	//modelRock = (OPmodel*)OPCMAN.LoadGet("rockwall.opm");
/*	
	scene.Add(model, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-0.5, 0.25, 0);
	scene.Add(model, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-0.5, 0.25, -0.5); */

	//scene.Add(modelRock, &materialInstanceRock->rootMaterialInstance)->world.SetScl(0.5)->Translate(-1.8, 0, -0.25);

	OPsprite3DInit(NULL);
	player.Init();
	enemyIndex = 1;
	for (ui32 i = 0; i < enemyIndex; i++) {
		enemy[i].Init(1);
		enemy[i].position.x += 6.5f;
		enemy[i].sprite3D->Direction = -1;
		enemy[i].target = enemy[i].position;
	}

	const OPchar* fireSprites[4] = {
		"fire/Empty",
		"fire/Fire",
		"fire/EmptySelect",
		"fire/Selectable",
	};
	staticEntity.Init("fire.opss", fireSprites, 4);
	staticEntity.position.x = 1;
	staticEntity.position.y = -0.1;
	staticEntity.position.z = 0;
	//modelAnimated.Init(&scene, &materialPhong);
	//modelAnimated.position.x = 2;

	const OPchar* controlSprites[1] = {
		"controls/X",
	};
	controls.Init("controls.opss", controlSprites, 1);
	controls.scale = OPvec3(0.25f);
	controls.sprite3D->Scale *= 0.25f;

	const OPchar* treeSprites[2] = {
		"trees/Tree1",
		"trees/Tree2"
	};
	for (ui32 i = 0; i < 20; i++) {
		trees[i].Init("trees.opss", treeSprites, 2);
		trees[i].position.x = 5 + (i * 2 ) + -0.25 + 0.5f * OPrandom();
		trees[i].position.z = -0.35 + 0.2f * OPrandom();
		f32 scale = 3.0f + OPrandom() * 2.0;
		trees[i].scale = OPvec3(scale);
		trees[i].sprite3D->Scale *= scale;
	}

	
	bush.Init("trees.opss", treeSprites, 2);
	bush.position.x = 6;
	bush.position.z = 0.2;
	bush.scale = OPvec3(1.0f);
	bush.sprite3D->Scale *= 1.0f;
	OPsprite3DSetSprite(bush.sprite3D, 1, false);

	const OPchar* skySprites[1] = {
		"sky/Sky"
	};
	sky.Init("sky.opss", skySprites, 1);
	sky.position.x = 4;
	sky.position.y = -2;
	sky.position.z = -1.0;
	sky.scale = OPvec3(0.5f);
	sky.sprite3D->Scale *= 0.5f;

	showDayTimer = 8000;
	fireAttempts = 0;
}

bool showControl = false;
f32 rabbitPerc = 0.3;
bool lost = false;

OPint ExampleStateUpdate(OPtimer* timer) {
	if (player.stamina <= 0) {
		return 0;
	}

	cameraShakeTimer -= timer->Elapsed;

	rabbitTimer -= timer->Elapsed;
	dayTimer += timer->Elapsed;
	showDayTimer -= timer->Elapsed;
	showNomTimer -= timer->Elapsed;

	if (useLight) {
		useLightAmount += timer->Elapsed * 0.001;
		if (useLightAmount > 1) {
			useLightAmount = 1;
		}
	}

	if (dayTimer > dayCycle) {
		dayTimer -= dayCycle;
		day++;
		if (day >= 7) {
			lost = true;
			return 0;
		}
		showDayTimer = 8000;
	}

	if (dayTimer > dayCycleHalf) {
		ambient = (dayCycle - dayTimer) / (f32)dayCycleHalf;
	}
	else {
		ambient = dayTimer / (f32)dayCycleHalf;
	}
	
	

	attenuation += (-0.5 + OPrandom()) * 0.2;
	if (attenuation < MinAttenuation) {
		attenuation = MinAttenuation;
	}
	if (attenuation > MaxAttenuation) {
		attenuation = MaxAttenuation;
	}
	if (rabbitTimer < 0) {
		rabbitTimer = monsterTime;
		bool found = false;
		for (ui32 i = 0; i < enemyIndex; i++) {
			if (!enemy[i].visible) {
				enemy[i].Init(OPrandom() > rabbitPerc);
				enemy[i].position.x += bush.position.x;
				enemy[i].sprite3D->Direction = -1;
				enemy[i].target = enemy[enemyIndex].position;
				found = true;
				break;
			}
		}
		// All enemies are alive, move to the next one
		if (enemyIndex < 10 && !found) {
			enemy[enemyIndex].Init(OPrandom() > 0.3);
			enemy[enemyIndex].position.x += bush.position.x;
			enemy[enemyIndex].sprite3D->Direction = -1;
			enemy[enemyIndex].target = enemy[enemyIndex].position;
			enemyIndex++;
		}

	}
	camera.Update(timer);
    scene.Update(timer);
	player.Update(timer);
	for (ui32 i = 0; i < enemyIndex; i++) {
		enemy[i].Update(timer);
	}
	//modelAnimated.Update(timer);
	staticEntity.Update(timer);
	for (ui32 i = 0; i < 10; i++) {
		trees[i].Update(timer);
	}
	bush.Update(timer);
	sky.Update(timer);
	controls.Update(timer);


	showControl = false;

	if (OPGAMEPADS[0]->WasPressed(OPgamePadButton::Y)) {
		cameraShakeTimer = 200;
	}

	if (!player.restrictMovement) {
		player.stamina = 100;
	}
	if (player.stamina <= 0 || lost || player.stamina >= 100) {
	}
	else {
		if (!useLight && player.position.x > 1.0f) {
			player.position.x = 1.0f;
		}


		// Turn the fire on
		if (player.actionPressed && !useLight && player.boundingBox.Collision(staticEntity.boundingBox) != OPintersectionType::NONE) {
			OPfmodPlay(rockStrike);
			player.actionPressed = false;
			player.stamina--;
			fireAttempts++;
			if (fireAttempts > 2) {
				OPsprite3DSetSprite(staticEntity.sprite3D, 1, false);
				useLight = true;
			}
		}

		// If the fire isn't on, set the outline on the fire
		if (!useLight && player.boundingBox.Collision(staticEntity.boundingBox) != OPintersectionType::NONE) {
			OPsprite3DSetSprite(staticEntity.sprite3D, 2, false);
			showControl = true;
			controls.position = staticEntity.position + OPvec3(0, 1, 0);
		}
		// If the first isn't on, make sure it shows normally
		else if (!useLight) {
			OPsprite3DSetSprite(staticEntity.sprite3D, 0, false);
		}
		// If the player is dragging something
		// And we're on the firepit, set to selectable
		else if (player.drag && player.boundingBox.Collision(staticEntity.boundingBox) != OPintersectionType::NONE) {
			OPsprite3DSetSprite(staticEntity.sprite3D, 3, false);
			showControl = true;
			controls.position = staticEntity.position + OPvec3(0, 1, 0);
		}
		else {
			// Otherwise, just show a normal firepit
			OPsprite3DSetSprite(staticEntity.sprite3D, 1, false);
		}




		if (player.drag && player.dragging != NULL) {
			player.dragging->position.x = player.position.x;
			player.dragging->sprite3D->Direction = player.sprite3D->Direction;

			if (player.sprite3D->Direction == 1) {
				player.dragging->position.x -= 0.4f;
			}
			else {
				player.dragging->position.x += 0.4f;
			}

			// If interacting with the fire
			if (player.actionPressed && player.boundingBox.Collision(staticEntity.boundingBox) != OPintersectionType::NONE) {
				player.dragging->visible = false;
				player.stamina += player.dragging->staminaGain;
				player.actionPressed = false;
				showNomTimer = 3000;
				player.dragging = NULL;
				player.drag = false;
				OPfmodPlay(cook);
			}
			// Otherwise, drop the enemy
			else if (player.actionPressed) {
				player.dragging = NULL;
				player.drag = false;
				player.actionPressed = false;
			}
		}
		else {
			player.drag = false;
		}

		for (ui32 i = 0; i < enemyIndex; i++) {
			if (enemy[i].visible && enemy[i].health <= 0 && player.actionPressed && player.boundingBox.Collision(enemy[i].boundingBox) != OPintersectionType::NONE) {
				player.SetDragging(&enemy[i]);
				player.actionPressed = false;
			}

			if (enemy[i].position.x < 3) {
				enemy[i].target.x = 4.5 + OPrandom() * 4;
				enemy[i].attacking = false;
				enemy[i].player = NULL;
			}
			else {

				if (enemy[i].attacks && enemy[i].health > 0 && player.boundingBox.Collision(enemy[i].visualBoundingBox) != OPintersectionType::NONE) {
					enemy[i].freakOutTimer = 5000;
					enemy[i].attacking = true;
					enemy[i].player = &player;
				}

				// If player is attacking,
				if (player.attacking && player.sprite3D->CurrentFrame == 1 && enemy[i].health > 0 && player.attackBoundingBox.Collision(enemy[i].boundingBox) != OPintersectionType::NONE) {
					OPrenderClear(0.2, 0, 0);
					player.attacking = false;
					player.actionPressed = false;
					enemy[i].health -= player.damage;
					enemy[i].hurtTimer = 500;
					enemy[i].pushAmount = player.sprite3D->Direction * 0.2f;
					enemy[i].freakOutTimer = 5000;
					enemy[i].attacking = true;
					enemy[i].player = &player;
					OPfmodPlay(hit);
				}
			}
		}

		if (!player.moving) {
			for (ui32 i = 0; i < 10; i++) {
				if (trees[i].visible && player.boundingBox.Collision(trees[i].boundingBox) != OPintersectionType::NONE) {
					showControl = true;
					controls.position = trees[i].position + OPvec3(0, 1, 0);

					if (player.actionPressed) {
						cameraShakeTimer = 500;
						player.actionPressed = false;
						OPfmodPlay(shakeTree);
						player.stamina--;
						trees[i].health--;
						if (trees[i].health <= 0) {
							trees[i].visible = false;
							OPfmodPlay(shakeTree);
							player.SetClub();
							OPfmodPlay(powerup);
							monsterTime -= 1000;
							rabbitPerc += 0.2;
						}
					}
					break;
				}
			}
		}
	}

	player.UseAction();

	if (!OPfmodIsPlaying(bgChannel)) {
		bgChannel = OPfmodPlay(bg);
	}

	return false;
}

void RenderStaticEntity(StaticEntity* entity, OPfloat delta) {
	entity->Render(delta, scene.camera, lightPos, useLight, attenuation, ambient);
	//OPsprite3DPrepRender(entity->sprite3D, scene.camera, OPVEC3_ZERO, 0);
	//OPrenderDrawBufferIndexed(0);
	//OPeffectSet("uLightPos", &lightPos);
	//OPeffectSet("uUseLight", &useLight);
	//OPeffectSet("uAttenuation", &attenuation);
	//OPeffectSet("uAmbient", &ambient);
	//OPeffectSet("uCamPos", &scene.camera->pos);
	//OPrenderDrawBufferIndexed(0);
}


void ExampleStateRender(OPfloat delta) {

	if (!player.restrictMovement) {
		player.stamina = 100;
	}

	if (player.stamina <= 0 || lost || player.stamina >= 100) {
		OPrenderCull(true);
		OPrenderDepth(true);
		OPrenderDepthWrite(true);
		OPrenderBlend(false);
		OPrenderClear(0);

		fontManager.SetAlign(OPfontAlign::CENTER);
		OPfontRenderBegin(&fontManager);
		if (player.stamina >= 100) {
			OPfontRender("caveman win!", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2));
		} else  if (lost) {
			OPfontRender("7 days have past. You lost.", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2));
		}
		else {
			OPfontRender("You Dead", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2));
		}
		OPfontRenderEnd();
		if (player.stamina >= 100) {
			player.SetAllowMovement();
			OPrenderCull(false);
			OPrenderDepth(true);
			OPrenderDepthWrite(false);
			OPrenderBlend(true);
			scene.camera->pos = OPvec3(0, 2, 5);
			scene.camera->target = OPvec3(0, 2, 0);
			scene.camera->Update();

			player.Render(delta, scene.camera, lightPos, false, attenuation, 0.9);

		}

		OPrenderPresent();
		return;
	}



	// Update camera position
	if (!USE_FREE_FORM_CAMERA) {
		scene.camera->pos = OPvec3(0, 1, 3.5f) + OPvec3Tween(player.prevPosition, player.position, delta);
		scene.camera->target = OPvec3Tween(player.prevPosition, player.position, delta);
		if (scene.camera->pos.x < 2.0) {
			scene.camera->pos.x = 2.0f;
			scene.camera->target.x = 2.0f;
		}
		if (scene.camera->pos.x > 14.0) {
			scene.camera->pos.x = 14.0f;
			scene.camera->target.x = 14.0f;
		}

		scene.camera->pos.y += 1.0;
		scene.camera->target.y += 1.0;

		if (cameraShakeTimer > 0) {
			f32 percent = OPpi2 * (150 - cameraShakeTimer) / 150.0f;
			scene.camera->pos.x += OPsin(percent) * 0.025f;
			scene.camera->target.x += OPsin(percent) * 0.025f;
			scene.camera->pos.y += OPsin(percent) * 0.025f;
			scene.camera->target.y += OPsin(percent) * 0.025f;
		}

		scene.camera->Update();
	}

	OPrenderCull(true);
	OPrenderDepth(true);
	OPrenderDepthWrite(true);
	OPrenderBlend(false);
	OPrenderClear(0.0);

	scene.Render(delta);

	OPrenderCull(false);
	OPrenderDepth(true);
	OPrenderDepthWrite(false);
	OPrenderBlend(true);
	OPRENDERER_ACTIVE->SetBlendModeAlpha();
	//sky.Render(delta, scene.camera);
	RenderStaticEntity(&sky, delta);

	sky.position.x = 4;
	sky.Render(delta, scene.camera, lightPos, false, 0, ambient);
	sky.position.x = 16;
	sky.Render(delta, scene.camera, lightPos, false, 0, ambient);

	//staticEntity.Render(delta, scene.camera);
	RenderStaticEntity(&staticEntity, delta);

	for (ui32 i = 0; i < 10; i++) {
		//trees[i].Render(delta, scene.camera);
		RenderStaticEntity(&trees[i], delta);
	}

	for (ui32 i = 0; i < enemyIndex; i++) {
		enemy[i].Render(delta, scene.camera, lightPos, useLight, attenuation, ambient);
	}

	player.Render(delta, scene.camera, lightPos, useLight, attenuation, ambient);
	RenderStaticEntity(&bush, delta);

	if (showControl) {
		//controls.Render(delta, scene.camera);
		controls.Render(delta, scene.camera, lightPos, useLight, attenuation, 1.0);
	}

	if (OPGAMEPADS[0]->WasPressed(OPgamePadButton::Y)) {
		showDayTimer = 8000;
	}

		fontManager.SetAlign(OPfontAlign::CENTER);
		OPfontRenderBegin(&fontManager);
		char buffer[128];
		//if (showDayTimer > 0) {
			sprintf(buffer, "Day %d", day);
			OPfloat yPos = 20; // OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2;
			OPfloat halfHeight = OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2.0f;
			if (showDayTimer > 0) {
				OPfloat percentageDayShow = (8000 - showDayTimer) / 8000.0f;

				if (percentageDayShow < 0.25) {
					OPfloat perc = (percentageDayShow * 4.0f);
					yPos = 30 + (halfHeight - 30) * OPtween_quadraticEaseIn(perc);
				}
				else if (percentageDayShow > 0.75) {
					OPfloat perc = (1.0 - ((percentageDayShow - 0.75) * 4.0f));
					yPos = 30 + (halfHeight - 30) * OPtween_quadraticEaseOut(perc);
				}
				else {
					yPos = 30 + halfHeight - 30;
				}
			}
			else {
				yPos = 30;
			}
			OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2, yPos));
		
		if (showNomTimer > 0) {
			fontManager.scale = 0.5;
			OPfontRender("NOM NOM NOM", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2 + 20));
			fontManager.scale = 1.0;
		}

		sprintf(buffer, "Stamina %d", (int)player.stamina);
		OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height - 40));

		OPfontRenderEnd();
	

	OPimguiNewFrame();
	OPVISUALDEBUGINFO.DrawWindows(delta);

	//player.RenderDebug();

	bool showHealth = true;
	//ImGui::SetNextWindowPos(ImVec2(10, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height - 70));
	//{
	//	ImGui::Begin("Stamina", &showHealth, ImVec2(100, 50), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	//
	//	ImGui::Text("Stamina: %d", (int)player.stamina);

	//	ImGui::End();
	//}
	ImGui::Render();



	OPrenderPresent();
}

OPint ExampleStateExit(OPgameState* next) {
	return 0;
}


OPgameState GS_EXAMPLE = {
	ExampleStateInit,
	ExampleStateUpdate,
	ExampleStateRender,
	ExampleStateExit
};
