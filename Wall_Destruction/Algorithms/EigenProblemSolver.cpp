#include "EigenProblemSolver.h"
#include "MathHelper.h"
#include "eig3.h"

// code taken from an implementation http://barnesc.blogspot.com/2007/02/eigenvectors-of-3x3-symmetric-matrix.html
void EigenProblemSolver::GetMaxEigenVectorAndValue(D3DXMATRIX matrix, float &maximumEigenValue, D3DXVECTOR3 &maximumEigenVector){
	double A[3][3];
	double V[3][3];
	double d[3];

	maximumEigenValue = -FLT_MAX;
	maximumEigenVector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(MathHelper::IsFiniteNumber(matrix(i, j)))
				A[i][j] = matrix(i, j);
			else 
				return;
		}
	}

	eigen_decomposition(A, V, d);	

	D3DXVECTOR3 eigenVector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	

	int index = -1;
	for(int i = 0; i<3; i++){
		if(d[i] > maximumEigenValue){
			maximumEigenValue = (float)d[i];
			index = i;
		}
	}

	if(index != -1){
		maximumEigenVector = D3DXVECTOR3((float)V[index][0], (float)V[index][1], (float)V[index][2]);
	}	
}