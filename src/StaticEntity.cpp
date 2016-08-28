#include "StaticEntity.h"

void StaticEntity::Init(const OPchar* spriteSheet, const OPchar** spriteFrames, ui32 count) {

	OPCMAN.Load(spriteSheet);
	sprites = OPALLOC(OPsprite*, count);
	for (ui32 i = 0; i < count; i++) {
		sprites[i] = (OPsprite*)OPCMAN.Get(spriteFrames[i]);
	}
	sprite3D = OPsprite3DCreate(sprites, NULL);
	sprite3D->FrameRate = 12.0f;
	sprite3D->Scale = OPvec3(1.0 / 42.0);
	health = 3;
	visible = true;
}

void StaticEntity::Update(OPtimer* timer) {
	OPsprite3DUpdate(sprite3D, timer->Elapsed);

	prevPosition = position;

	boundingBox = OPboundingBox3D(
		OPvec3(position.x - 0.25, position.y, -0.1),
		OPvec3(position.x + 0.25, position.y + 0.5, 0.1));
}

void StaticEntity::Render(OPfloat delta, OPcam* camera, OPvec3 lightPos, bool useLight, f32 attenuation, f32 ambient) {
	sprite3D->Position = OPvec3Tween(prevPosition, position, delta);
	sprite3D->Position.y -= (5.0 / 42.0) * scale.y;

	if (!visible) return;

	OPsprite3DPrepRender(sprite3D, camera, OPVEC3_ZERO, 0);
	OPeffectSet("uLightPos", &lightPos);
	OPeffectSet("uUseLight", useLight);
	OPeffectSet("uAttenuation", attenuation);
	OPeffectSet("uAmbient", ambient);
	OPeffectSet("uCamPos", &camera->pos);
	OPeffectSet("uShowHurt", false);
	OPrenderDrawBufferIndexed(0);
	//OPsprite3DRender(sprite3D, camera);
}