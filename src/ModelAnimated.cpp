#include "ModelAnimated.h"

void ModelAnimated::Init(OPscene* scene, OPmaterialPhong* material) {
	models[0] = (OPmodel*)OPCMAN.LoadGet("firepit.opm");
	models[1] = (OPmodel*)OPCMAN.LoadGet("firepit2.opm");
	models[2] = (OPmodel*)OPCMAN.LoadGet("firepit3.opm");
	materialInstance = material->CreateInstance();
	materialInstance->SetAlbedoMap("firepit.png");
	active = scene->Add(models[0], materialInstance->Base());
}

void ModelAnimated::Update(OPtimer* timer) {
	prevPosition = position;
	active->world.SetTranslate(position)->Scl(0.3);

	boundingBox = OPboundingBox3D(
		OPvec3(position.x - 0.25, position.y, -0.1),
		OPvec3(position.x + 0.25, position.y + 0.5, 0.1));
}