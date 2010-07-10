
#ifndef RMLS_H
#define RMLS_H

#include "Structs.h"
#include <D3DX10.h>

class Algorithms
{
public:
	// Robust implicit Moving Least Squares
	//static Structs::SURFEL_VERTEX* RIMLS(Structs::POINT_CLOUD_VERTEX *points);

	//void Algorithms::SurfaceSplatting(Structs::POINT_CLOUD_VERTEX *points);

	static ProjectStructs::SURFEL_VERTEX* APSS(ProjectStructs::POINT_CLOUD_VERTEX *points);

};

#endif