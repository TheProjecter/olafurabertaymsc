#ifndef EIGEN_PROBLEM_SOLVER_H
#define EIGEN_PROBLEM_SOLVER_H

#include <D3DX10.h>

class EigenProblemSolver
{
public:
	static void GetMaxEigenVectorAndValue(D3DXMATRIX matrix, float &maximumEigenValue, D3DXVECTOR3 &maximumEigenVector);
};

#endif