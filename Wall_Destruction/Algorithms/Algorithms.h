
#ifndef RMLS_H
#define RMLS_H

#include "Structs.h"
#include "PointGrid.h"
#include "VertexBufferGrid.h"
#include <D3DX10.h>

struct AngleCheck{
	float underAngle;
	float overAngle;
	bool tooCloseToUnderAngle;
	bool tooCloseToOverAngle;
};

class Algorithms
{
public:
	static std::vector<ProjectStructs::SURFEL*> Splat(std::vector<ProjectStructs::SURFEL*> surfelsWithoutMinorMajorAndNormals, float maxRadiusLength, D3DXVECTOR3 generalNormal, D3DXVECTOR3 volumePos);
	static std::vector<ProjectStructs::SURFEL*> ResampleSurfel( ProjectStructs::SURFEL* surfel, ProjectStructs::IMPACT* impact, D3DXMATRIX surfaceWorldMatrix);
	static void Algorithms::RefineSurfel(ProjectStructs::SURFEL* surfel);
	static void Algorithms::CalculateNeighbors(ProjectStructs::SURFEL* surfel);
	static void CalculateNeighbors(ProjectStructs::SURFEL* surfel, std::vector<ProjectStructs::SURFEL*> deformedSurfels);
	static void DecreaseNeighbors( ProjectStructs::SURFEL* surfel );

	static void Draw();
	static void CleanUp();
	const static float Scale;
	static int WentThroughWholeListCount;

private:
	static void CalculateNeighbor(ProjectStructs::SURFEL* surfel, ProjectStructs::SURFEL* neighbor);
	static AngleCheck CheckAngle(float angle, ProjectStructs::SURFEL* surfel);
	static std::vector<PointGrid*> grids;
	const static float angleThreshold;
	
};

#endif