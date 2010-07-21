#pragma once
#include "Structs.h"
#include "Volume.h"

class FractureManager
{
public:
	static float CalculateWeight(ProjectStructs::PHYXEL_NODE *xi, ProjectStructs::PHYXEL_NODE *xj);

	static void CalculateJacobian(ProjectStructs::Phyxel_Grid_Cell *cell);
	
	static void CalculateShape(ProjectStructs::Phyxel_Grid_Cell *cell);

	static void CalculateStrain(ProjectStructs::Phyxel_Grid_Cell *cell);

	static void CalculateStress(ProjectStructs::Phyxel_Grid_Cell *cell);

	static void PreStepFractureAlgorithm(Volume *volume, float dt);

	static void StepFractureAlgorithm(Volume *volume);
private:
	static D3DXVECTOR3 u, v, w;
	static D3DXMATRIX displacementGradientTranspose, B;
	static float threshold;

};
