
#ifndef RMLS_H
#define RMLS_H

#include "Structs.h"
#include "PointGrid.h"
#include <D3DX10.h>

class Algorithms
{
public:
	static std::vector<ProjectStructs::SURFEL*> Splat(std::vector<ProjectStructs::SURFEL*> surfelsWithoutMinorMajorAndNormals, float maxRadiusLength, D3DXVECTOR3 generalNormal, D3DXVECTOR3 volumePos);
	static std::vector<ProjectStructs::SURFEL*> ResampleSurfel( ProjectStructs::SURFEL* surfel, ProjectStructs::IMPACT* impact, D3DXMATRIX surfaceWorldMatrix);
	static void Algorithms::RefineSurfel(ProjectStructs::SURFEL* surfel);
	static void Algorithms::CalculateNeighbors(ProjectStructs::SURFEL* surfel);

	static void DecreaseNeighbors( ProjectStructs::SURFEL* surfel );

	static void Draw();
	static void CleanUp();

private:
	static std::vector<PointGrid*> grids;
	const static float angleThreshold;
};

#endif