#pragma once

#include "./OPengine.h"

struct ModelAnimated {
	OPsceneEntity* active;
	OPmodel* models[3];
	OPvec3 position, prevPosition;
	OPboundingBox3D boundingBox;
	OPmaterialPhongInstance* materialInstance;

	void Init(OPscene* scene, OPmaterialPhong* material);
	void Update(OPtimer* timer);
};