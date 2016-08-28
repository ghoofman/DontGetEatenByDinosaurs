#pragma once

#include "./OPengine.h"

struct StaticEntity {
	OPsprite3D* sprite3D;
	OPsprite** sprites;
	OPvec3 position, prevPosition;
	OPvec3 scale = OPVEC3_ONE;
	OPboundingBox3D boundingBox;
	ui32 health;
	bool visible;

	void Init(const OPchar* spriteSheet, const OPchar** sprites, ui32 count);
	void Update(OPtimer* timer);
	void Render(OPfloat delta, OPcam* camera, OPvec3 lightPos, bool useLight, float attenuation, float ambient);
};