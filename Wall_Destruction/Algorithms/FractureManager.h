#pragma once
#include "Structs.h"
#include "Volume.h"

class FractureManager
{
public:
	static float CalculateWeight(ProjectStructs::PHYXEL_NODE *xi, ProjectStructs::PHYXEL_NODE *xj);
	static float CalculateWeight(D3DXVECTOR3 x1, D3DXVECTOR3 x2, float supportRadius);

	static void CalculateAndInitiateFractures(Volume *volume, float dt);

	static void CalculateStressAndStrain( ProjectStructs::PHYXEL_NODE* node, Volume * volume);
private:
	static void StepFractureAlgorithm(Volume *volume, std::vector<CRACK*> cracks);

	static void AddSamples( ProjectStructs::CRACK* crack, Volume* volume );
	static D3DXVECTOR3 u, v, w;
	static D3DXMATRIX displacementGradientTranspose, B;
	static float threshold;

};
